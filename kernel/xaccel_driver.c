#include "xaccel_driver.h"
#include "../include/macros.h"

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);


static struct xaccel_dev g_xaccel_dev;


// Structure is init allocates stuff and cleanup and releases them

static int __init xaccel_init(void)
{
	// Make sure we're checking return of each part
	
	// Make sure we're using goto statement to return back a certain point to release all resources we've allocated
	
	/* Based on how many devices we are allocating assign device numbers in loop
	// For num accelerator functions() allocate a char device number for each one
	//  ret_val = alloc_chrdev_region(dev_t, unsigned int count, char *name);	
	    if (ret_val)
	    {
	    	do stuff
	    }
	*/

	int ret;
	pr_info("xaccel: inti\n");

	// Allocating device numbers for new character device
	ret = alloc_chrdev_region(&g_xaccel_dev.devt, FIRST_MINOR, NUM_DEV_REQUESTED, XACCEL_NAME);
	if (ret)
	{
		pr_err("xaccel: alloc_chrdev_region failed: %d\n", ret);
		return ret;
	}
	
	// Initialize the character fevice struct and it's owner field
	cdev_init(&g_xaccel_dev.cdev, &xaccel_fops);
	g_xaccel_dev.cdev.owner = THIS_MODULE;
	// Inform the kernel about the 
	ret = cdev_add(&g_xaccel_dev.cdev, g_xaccel_dev.devt, 1);
	if (ret)
	{
		pr_err("xacccel: cdev_add failed: %d\n", ret);
		// Unregistering previous alloc device numbers
		unregister_chrdev_region(g_xaccel_dev.devt, 1);
		return ret;
	}

	g_xaccel_dev.class = class_create(XACCEL_CLASS_NAME);
	if (IS_ERR(g_xaccel_dev.device))
	{
		ret = PTR_ERR(g_xaccel_dev.device)
		g_xaccel_dev.device = NULL;
		pr_err("xaccel: device_create failed: %d\n", ret);
		class_destroy(g_xaccel_dev.class);
		g_xaccel_dev.class = NULL;
		cdev_del(&g_xaccel_dev.cdev); // Remove char dev from system
		unregister_chr


	return 0;
}


static void __exit xaccel_cleanup(void)
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


static int xaccel_ioctl( struct file* fp, unsigned int cmd, unsigned long arg)
{
	pr_info("xaccel: ioctl called, cmd=0x%x\n", cmd);

	/* No ioctls implemented yet */
	return -ENOTTY;
}

// File Operations
static int xaccel_open(struct inode* node, struct file* fp)
{
	struct xaccel_dev *dev;

	dev = container_of(inode->i_cdev, struct xaccel_dev, cdev);
	fp->private_data = dev;

	pr_info("xaccel: device opened\n");
	return 0;
}

static int xaccel_close(struct inode* node, struct file* fp)
{
	pr_info("xaccel: device closed\n");
	return 0;
}

struct file_operations xaccel_fops = {
	.owner          = THIS_MODULE,
	.llseek         = NULL,
	.read           = xaccel_read,
	.write          = xaccel_write,
	.unlocked_ioctl = xaccel_ioctl,
	.open           = xaccel_open, 
	.release        = xaccel_close, 
};


module_init(xaccel_init);
module_exit(xaccel_exit);
