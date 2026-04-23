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
	void *mmio_buf = NULL;
	pr_info("xaccel: init\n");

#ifdef NO_HW
	
	enum test_case test= ONE_FUNCTION;
	pr_info("Populating RAM with test object to emulate MMIO\n");
	if (gen_xaccel_test_obj(test, &mmio_buf))
	{
	    pr_info("ERROR: Generating test scenario failed\n");
	    return -EFAULT;
	}
#else




#endif

	if (xaccel_create_instance(&mmio_buf, gps_xdev, xaccel_fops))
	{
		pr_info("ERROR: Failed to create test scenario instance\n");
		return -1;
	}	
	/*	
	// Allocate Space for the Device
	pr_info("Allocating space for device object\n");
	gps_xdev = kzalloc(sizeof(*gps_xdev), GFP_KERNEL);	
	
	if(!gps_xdev)
	{
	    pr_err("Failed to allocate memory");
	    return -ENOMEM;
	}
	
	// Initialize the Semaphore
	pr_info("Initilizing the semaphore\n");
	sema_init(&(gps_xdev->sem), MAX_LOCK_HOLDERS);
	down(&(gps_xdev->sem));
	
	//TODO: Add in Memory Mapping
	//TODO: Parse Descriptor Header
	//TODO: Parse Function Descriptor
	//TODO: Build Runtime Function Objects

	// FIXME: STUB CODE
	gps_xdev->num_functions = 1;
	gps_xdev->funcs = kcalloc(1, sizeof(gps_xdev->funcs[0]), GFP_KERNEL);

	
	// Allocating device numbers for new character device
	pr_info("Allocating device numbers for character device\n");
	ret = alloc_chrdev_region(&(gps_xdev->base_devt), FIRST_MINOR, gps_xdev->num_functions, XACCEL_NAME);
	if (ret)
	{
		pr_err("xaccel: alloc_chrdev_region failed: %d\n", ret);
		up(&(gps_xdev->sem));
		kfree(gps_xdev);
		return ret;
	}
	
	// Create the class for xaccel
	pr_info("Creating class for xaccel devices\n");
	gps_xdev->class = class_create(XACCEL_CLASS_NAME);
	
		
	// FIXME: STUB CODE START
	struct xaccel_function* func =  &(gps_xdev->funcs[0]);	
	func->parent = gps_xdev;
	
	func->desc.func_id = 7;
	func->desc.mmio_offset = 0;
	func->desc.mmio_size = 0x100;
	func->desc.caps = XACCEL_CAP_MMIO_RW;


	func->regs = gps_xdev->mmio_base;

	cdev_init(&(func->cdev), &(xaccel_fops));
	func->cdev.owner = THIS_MODULE;
	func->devt = MKDEV(MAJOR(gps_xdev->base_devt), 0);
	cdev_add(&(func->cdev), func->devt, 1);
	func->device = device_create(gps_xdev->class, NULL, func->devt, NULL, "xaccel0_func0");	
	gps_xdev->funcs = func;	

	
	
	cdev_init(g_xaccel_dev.cdev, &xaccel_fops);
	g_xaccel_dev.cdev.owner = THIS_MODULE;
	// Inform the kernel about the 
	ret = cdev_add(g_xaccel_dev.cdev, g_xaccel_dev.base_devt, 1);
	if (ret)
	{
		pr_err("xacccel: cdev_add failed: %d\n", ret);
		// Unregistering previous alloc device numbers
		unregister_chrdev_region(g_xaccel_dev.base_devt, 1);
		return ret;
	}


	// Error Handling Routine
	if (IS_ERR(gps_xdev->dev))
	{	
		ret = PTR_ERR(gps_xdev->dev);
		pr_err("xaccel: device_create failed: %d\n", ret);
		struct xaccel_function* temp_func;
		if (gps_xdev)
		{
		    for (int i=0; i<gps_xdev->num_functions; i++)
		    {
		        temp_func = &(gps_xdev->funcs[i]);
			device_destroy(gps_xdev->class, temp_func->devt);
	    	        cdev_del(&(temp_func->cdev));
		    }	
		}
		kfree(gps_xdev->funcs); 	
		gps_xdev->funcs = NULL;

		if (gps_xdev->class) 
		{
		    class_destroy(gps_xdev->class); // Destroy Class
		    gps_xdev->class = NULL;
		}
		unregister_chrdev_region(gps_xdev->base_devt, 1);
		up(&(gps_xdev->sem));
		kfree(gps_xdev);
		gps_xdev = NULL;
		return ret;
	}

	up(&(gps_xdev->sem));
	*/
	pr_info("XACCEL_INIT() Returning Successfully...\n");	
	return 0;
}


static void __exit xaccel_exit(void)
{

	pr_info("xaccel exit() starting...");
	xaccel_cleanup(gps_xdev);
	/*
	if (!gps_xdev) return;
	
	// Releasing function objs and cdevs
	pr_info("Releasing function objects and cdevs");
	if (gps_xdev->funcs)
	{
	    struct xaccel_function* temp_func;
	    for (int i=0; i<gps_xdev->num_functions; i++)
	    {
		
	        temp_func = &(gps_xdev->funcs[i]);
		device_destroy(gps_xdev->class, temp_func->devt);
		pr_info("Deleting char device for func[%d]\n", i);
	    	cdev_del(&(temp_func->cdev));
	    }
	kfree(gps_xdev->funcs); 	
	gps_xdev->funcs = NULL;
	}
	if (gps_xdev->class) 
	    class_destroy(gps_xdev->class); // Destroy Class
	unregister_chrdev_region(gps_xdev->base_devt, gps_xdev->num_functions); //Release Device Number Region
	kfree(gps_xdev);
	gps_xdev = NULL;
	*/
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

#ifdef DEBUG
		    pr_info("Function's id is %d\n", func->desc.func_id);
		    pr_info("Function's type is %d\n", func->desc.func_type);
#endif

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


module_init(xaccel_init);
module_exit(xaccel_exit);
