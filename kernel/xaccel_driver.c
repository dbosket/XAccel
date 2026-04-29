#include "xaccel_driver.h"
#include "xaccel_uapi.h"
#include "../include/xaccel_macros.h"
#include "../include/xaccel_desc.h"

#define DEBUG 1

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);

static struct xaccel_dev* gps_xdev;


struct file_operations xaccel_fops = {
	.owner          = THIS_MODULE,
	.llseek         = NULL,
	.read           = xaccel_read,
	.write          = xaccel_write,
	.unlocked_ioctl = xaccel_ioctl,
	.open           = xaccel_open, 
	.release        = xaccel_close, 
};

// Structure is init allocates stuff and cleanup and releases them

static int __init xaccel_init(void)
{
	pr_info("xaccel: init\n");

#ifdef NO_HW
	
	enum test_case test= TWO_FUNCTION;
	void *mmio_buf = NULL;
	size_t buf_size = 0;
	pr_info("Populating RAM with test object to emulate MMIO\n");
	if (gen_xaccel_test_obj(test, &mmio_buf, &buf_size))
	{
	    pr_err("ERROR: Generating test scenario failed\n");
	    return -EFAULT;
	}
#else


#endif
	// Allocate Space for the Device                                                                            
        pr_debug("Allocating space for device object\n");                                                            
        gps_xdev = kzalloc(sizeof(*gps_xdev), GFP_KERNEL);

	if(!gps_xdev)
	{
		pr_err("Failed to allocate memory\n");
		return -ENOMEM;
	}
	gps_xdev->mmio_size = buf_size;
	if (xaccel_create_instance(mmio_buf, gps_xdev, &xaccel_fops))
	{
		pr_info("ERROR: Failed to create test scenario instance\n");
		return -1;
	}	
	
	xaccel_print_xaccel_instance(gps_xdev);

	// Error Handling Routine
	if (IS_ERR(gps_xdev->dev))
	{	
		int ret = PTR_ERR(gps_xdev->dev);
		pr_err("xaccel: device_create failed: %d\n", ret);
		xaccel_cleanup(gps_xdev);
		return ret;
	}
	pr_info("XACCEL_INIT() Returning Successfully...\n");	
	return 0;
}


static void __exit xaccel_exit(void)
{

	pr_info("XACCEL_EXIT() starting...");
	xaccel_cleanup(gps_xdev);
	pr_info("XACCEL_EXIT() Returning Successfully...\n");
	return;
}


static ssize_t xaccel_read( struct file* fp, char __user* buf, size_t cnt, loff_t* ppos)
{
	//Currently just a stub to see if parameters are passed correctly;
	pr_info("xaccel: read called, num=%zu, offset=%lld", cnt, *ppos);

	// No read data yet
	return 0;
}

static ssize_t xaccel_write( struct file* fp, const char __user* buf, size_t cnt, loff_t* ppos) 
{
	//Currently just a stub to see if parameters are passed correctly;
	pr_info("xaccel: write called , num=%zu, offset=%lld", cnt, *ppos);
	// Pretend we consumed all bytes
	return cnt;
}


static long int xaccel_ioctl( struct file* fp, unsigned int cmd, unsigned long int arg)
{
	pr_info("xaccel: ioctl called, cmd=0x%x\n", cmd);
	
	struct xaccel_function *func  = fp->private_data;
	void __user *argp = (void __user *)arg;
	struct xaccel_reg_io req;

	pr_debug("Function's id is %d\n", func->desc.func_id);
	pr_debug("Function's type is %d\n", func->desc.func_type);

	switch (cmd){
		case XACCEL_IOC_GET_INFO:

			struct xaccel_info f_info;
		    	f_info.func_id        = func->desc.func_id;
		    	f_info.func_type      = func->desc.func_type;
		    	f_info.func_version   = func->desc.func_version;
		    	f_info.irq_index      = func->desc.irq_index;
		    	f_info.mmio_size      = func->desc.mmio_size;
		    	f_info.caps           = func->desc.caps;
		    	f_info.reg_layout_ver = func->desc.reg_layout_ver;

		    	if (copy_to_user(argp, &f_info, sizeof(f_info)))
		        	return -EFAULT;
		    	return 0;

		case XACCEL_IOC_READ_REG:

		    	// Validate Capability
		    	if (!func->desc.caps && XACCEL_CAP_MMIO_RW )
		        	return -EOPNOTSUPP;
		    	// Copy data from userspace 
		    	if (copy_from_user(&req, argp, sizeof(req)))
	   	        	return -EFAULT;
		    	// Validate Offset
		    	if (req.offset & 0x3)
		        	return -EINVAL;
			if (req.offset + sizeof(__u32) > func->desc.mmio_size)
		        	return -EINVAL;
		    	// Read from MMIO
		    	req.value = ioread32(func->regs + req.offset);
		    	// Read from Userspace 
		    	if (copy_to_user(argp, &req, sizeof(req)))
			    return -EFAULT;
		    	return 0; 

		case XACCEL_IOC_WRITE_REG:

		    	if (!func->desc.caps && XACCEL_CAP_MMIO_RW)
		        	return -EOPNOTSUPP;
			// Copy from Userspace
		    	if (copy_from_user(&req, argp, sizeof(req)))
		        	return -EFAULT;
			// Validate Offset
			if (req.offset & 0x3)
		        	return -EINVAL;
		    	if (req.offset + sizeof(__u32) > func->desc.mmio_size)
		        	return -EINVAL;
			// Write to device
		    	iowrite32(req.value, func->regs + req.offset);
			return 0;

		default:
		    	return -ENOTTY;
	}
}

// File Operations
static int xaccel_open(struct inode* node, struct file* fp)
{
	struct xaccel_function *func;
	func = container_of(node->i_cdev, struct xaccel_function, cdev);

	if (!func || !func->parent)
	    return -ENODEV;

	mutex_lock(&func->lock);
	(func->open_count)++;
	mutex_unlock(&func->lock);
	fp->private_data = func;

	pr_info("xaccel: device opened\n");
	return 0;
}

static int xaccel_close(struct inode* node, struct file* fp)
{
	struct xaccel_function *func;
	func = container_of(node->i_cdev, struct xaccel_function, cdev);

	if (!func || !func->parent)
	    return -ENODEV;

	mutex_lock(&func->lock);
	if (func->open_count)
	    (func->open_count)--;
	mutex_unlock(&func->lock);
	fp->private_data = NULL;	

	pr_info("xaccel: device closed\n");
	return 0;
}


static int xaccel_reg_read(struct xaccel_function *func, __u32 offset, __u32 *value)
{
	// Ensure that pointers are valid
	if (!func || !value)
		return -EINVAL;
	// Ensure offset is valid for 32 bit register
	if (offset % sizeof(__u32))
		return -EINVAL;
	// Ensure MMIO regions is big enough for 32 bit access
	if ((func->desc).mmio_size < sizeof(__u32))
		return -EINVAL;
	if (offset > (func->desc).mmio_size - sizeof(__u32)) 
		return -EINVAL;

	*value = xaccel_read32(func->regs, offset);
	return 0;
}

static int xaccel_reg_write(struct xaccel_function *func, __u32 offset, __u32 value) 
{
	// Ensure that pointers are valid
	if (!func || !value)
		return -EINVAL;
	// Ensure offset is valid for 32 bit register
	if (offset % sizeof(__u32))
		return -EINVAL;
	// Ensure MMIO regions is big enough for 32 bit access
	if ((func->desc).mmio_size < sizeof(__u32))
		return -EINVAL;
	if (offset > (func->desc).mmio_size - sizeof(__u32)) 
		return  -EINVAL;
	xaccel_write32(func->regs, offset, value);
	return 0;
}

module_init(xaccel_init);
module_exit(xaccel_exit);
