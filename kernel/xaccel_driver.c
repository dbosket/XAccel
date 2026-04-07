#include "xaccel_driver.h"
#include "../include/macros.h"
MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);






// Structure is init allocates stuff and cleanup and releases them

static int xaccel_init(void)
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
	return 0;
}

static void xaccel_exit(void)
{
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


static ssize_t xaccel_read( struct file* fp, char __user* user, size_t num, loff_t* offset)
{
	return 0;
}

static ssize_t xaccel_write( struct file* fp, const char __user* user, size_t num, loff_t* offset) 
{
	return 0;	
}


static int xaccel_ioctl(void)
{
	return 0;
}

static int xaccel_open(struct inode* node, struct file* fp)
{
	return 0;
}

static int xaccel_release(struct inode* node, struct file* fp)
{
	return 0;
}

struct file_operations xaccel_fops = {
	.owner   = THIS_MODULE,
	.llseek  = NULL,
	.read    = xaccel_read,
	.write   = xaccel_write,
	.open    = xaccel_open, 
	.release = xaccel_release, 
};


module_init(xaccel_init);
module_exit(xaccel_exit);
