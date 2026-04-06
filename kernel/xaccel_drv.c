#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux.cdev.h>

#include "../include/xaccel_desc.h"

MODULE_LICENSE("Dual BSD/GPL")






 xaccel_dev 
{






	struct cdev cdev; /*Char device Structure */
}

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

static int xaccel_exit(void)
{
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


struct file_operations xaccel_fops = {
	.owner   = THIS_MODULE,
	.llseek  = xaccel_llseek,
	.read    = xaccel_read,
	.write   = xaccel_write,
	.ioctl   = xaccel_ioctl, 
	.open    = xaccel_open, 
	.release = xaccel_release, 
};


xaccel_init(module_init);
xaccel_exit(module_exit);

