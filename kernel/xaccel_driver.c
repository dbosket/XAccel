#include "xaccel_driver.h"
#include "../include/xaccel_macros.h"
#include "../include/xaccel_desc.h"

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

	int ret;
	pr_info("xaccel: init\n");
	
	// Allocate Space for the Device
	pr_info("Allocating space for device object\n");
	gps_xdev = kzalloc(sizeof(struct xaccel_dev*), GFP_KERNEL);	
	
	// Initialize the Semaphore
	pr_info("Initilizing the semaphore\n");
	sema_init(gps_xdev->sem, MAX_LOCK_HOLDERS);
	//sem_wait(gps_xdev->sem);
	
	// Allocating device numbers for new character device
	pr_info("Allocating device numbers for character device\n");
	ret = alloc_chrdev_region(&(gps_xdev->base_devt), FIRST_MINOR, NUM_DEV_REQUESTED, XACCEL_NAME);
	if (ret)
	{
		pr_err("xaccel: alloc_chrdev_region failed: %d\n", ret);
		kfree(gps_xdev);
		return ret;
	}

	// Create the class for xaccel
	pr_info("Creating class for xaccel devices\n");
	gps_xdev->class = class_create(XACCEL_CLASS_NAME);
	
	//FIXME: STUB CODE TO BE REMOVED
	gps_xdev->num_functions = 1;
	gps_xdev->funcs = kcalloc(1, sizeof(struct xaccel_function*), GFP_KERNEL);
	
	//FIXME: STUB DUMMY DESCRIPTOR
	struct xaccel_function* func =  &(gps_xdev->funcs[0]);	
	func->parent = gps_xdev;
	
	func->desc.func_id = 0;
	func->desc.mmio_offset = 0;
	func->desc.mmio_size = 0x100;
	func->desc.caps = XACCEL_CAP_MMIO_RW;

	func->regs = gps_xdev->mmio_base;

	//FIXME: STUB CDEV CODE
	cdev_init(&(func->cdev), &(xaccel_fops));
	func->cdev.owner = THIS_MODULE;
	func->devt = MKDEV(MAJOR(gps_xdev->base_devt), 0);
	cdev_add(&(func->cdev), func->devt, 1);
	func->device = device_create(gps_xdev->class, NULL, func->devt, NULL, "xaccel0_func0");	
	
	/*	
	// Initialize the character fevice struct and it's owner field
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
	*/


	// Error Handling Routine
	if (IS_ERR(gps_xdev->dev))
	{	
		ret = PTR_ERR(gps_xdev->dev);
		gps_xdev->dev = NULL;
		pr_err("xaccel: device_create failed: %d\n", ret);
		class_destroy(gps_xdev->class);
		gps_xdev->class = NULL;
		//cdev_del(gps_xdev.cdev); // Remove char dev from system
		//unregister_chrdev_region(g_xaccel_dev.devt, 1);
		return ret;
	
	}
	return 0;
}


static void __exit xaccel_exit(void)
{
	// Make sure undoing anything that we've done up into this point
	// Make sure we're checking things before releasing resources
	
	/* Make sure we're relaseing device numbers when we're done
	 * count is something that would be passed in by descriptor-->how much do we need?
	 * unregister_chrdev_region(dev_t, firstminor, count, *name);
	 */
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

	/* No ioctls implemented yet */
	return -ENOTTY;
}

// File Operations
static int xaccel_open(struct inode* node, struct file* fp)
{
	/*
	struct xaccel_dev *dev;

	dev = container_of(node->i_cdev, struct xaccel_dev, cdev);
	fp->private_data = dev;
	*/
	pr_info("xaccel: device opened\n");
	return 0;
}

static int xaccel_close(struct inode* node, struct file* fp)
{
	pr_info("xaccel: device closed\n");
	return 0;
}


module_init(xaccel_init);
module_exit(xaccel_exit);
