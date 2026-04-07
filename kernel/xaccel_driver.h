#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>

struct xaccel_dev 
{






	struct cdev cdev; /*Char device Structure */
};


static int xaccel_init(void);


static void xaccel_exit(void);

static void __exit xaccel_cleanup(void);

static ssize_t xaccel_read( struct file* fp, char __user* user, size_t num, loff_t* offset);

static ssize_t xaccel_write( struct file* fp, const char __user* user, size_t num, loff_t* offset);

static int xaccel_open(struct inode* node, struct file* fp);

static int release(struct inode* node, struct file* fp);
