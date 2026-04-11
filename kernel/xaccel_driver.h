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
#include <asm/uaccess.h>

#define XACCEL_NAME "xaccel"
#define XACCEL_CLASS_NAME "xaccel"

struct xaccel_dev 
{
	dev_t devt;
	struct cdev cdev; /*char device structure */
	struct class *class;
	struct device *device;
};


static int __init xaccel_init(void);

static void __exit xaccel_cleanup(void);

static ssize_t xaccel_read( struct file* fp, char __user* buf, size_t cnt, loff_t* ppos);

static ssize_t xaccel_write( struct file* fp, const char __user* buf, size_t cnt, loff_t* ppos);

static int xaccel_open(struct inode* node, struct file* fp);

static int xaccel_close(struct inode* node, struct file* fp);
