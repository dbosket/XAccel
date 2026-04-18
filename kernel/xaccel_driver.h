#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

#include "../include/xaccel_desc.h"
#include "../include/xaccel_macros.h"

#define XACCEL_NAME "xaccel"
#define XACCEL_CLASS_NAME "xaccel"

struct xaccel_dev 
{
	struct device *dev;		/* Parent Linux Device */
	void __iomem *mmio_base;	/* Base mapped MMIO region for accelerator */
	size_t mmio_size;		/* Total mapped MMIO region size */

	struct xaccel_desc_header hdr;  /* Parsed top-level descriptor header */

	u16 num_functions;		/* Number of parsed/discovered function */
	struct xaccel_function *funcs;  /* Array of runtime function objects */
	
	dev_t base_devt;		/* Base dev_t function for function devices */
	struct class *class;		/* /sys/class/xaccel */

	struct semaphore sem;		/* Projects shared device-wide state */
};

static int __init xaccel_init(void);

static void __exit xaccel_exit(void);

static ssize_t xaccel_read( struct file* fp, char __user* buf, size_t cnt, loff_t* ppos);

static ssize_t xaccel_write( struct file* fp, const char __user* buf, size_t cnt, loff_t* ppos);

static int xaccel_open(struct inode* node, struct file* fp);

static int xaccel_close(struct inode* node, struct file* fp);

static long int xaccel_ioctl(struct file* fp, unsigned int cmd, unsigned long int arg);
