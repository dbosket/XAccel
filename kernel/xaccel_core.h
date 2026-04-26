/*
 * Author: Taco
 * Purpose: This file contains the internal framework interface
 * 		aka the brain.
 *
 * Data Created: 30 March 2026
 *
 */

#ifndef XACCEL_CORE_H
#define XACCEL_CORE_H


#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/mutex.h>

#include "../include/xaccel_desc.h"
#include "../include/xaccel_macros.h"

#define FUNC_DESC_OFFSET sizeof(struct xaccel_desc_header)

struct xaccel_function
{
	struct xaccel_dev *parent;
	struct xaccel_func_desc desc;
#ifdef NO_HW
	void *regs;
#else
	void __iomem *regs;
#endif
	struct cdev cdev;
	dev_t devt;
	struct device *device;
	struct mutex lock;
	__u16 open_count;
}; 

struct xaccel_dev
{
        struct device *dev;             /* Parent Linux Device */
        void __iomem *mmio_base;        /* Base mapped MMIO region for accelerator */
        size_t mmio_size;               /* Total mapped MMIO region size */

        struct xaccel_desc_header* hdr;  /* Parsed top-level descriptor header */

        u16 num_functions;              /* Number of parsed/discovered function */
        struct xaccel_function *funcs;  /* Array of runtime function objects */

        dev_t base_devt;                /* Base dev_t function for function devices */
        struct class *class;            /* /sys/class/xaccel */

        struct semaphore sem;           /* Projects shared device-wide state */
};

// Create instance of struct xaccel_device
int xaccel_create_instance(void* mmio_base, struct xaccel_dev* xdev, struct file_operations* fops);


// Destroy instance of struct xaccel_device
int xaccel_destroy_instance(struct xaccel_dev* xdev);


// Clean up any memory allocated by xaccel_dev 
void xaccel_cleanup(struct xaccel_dev *xdev);


// Parse the descriptor recieved
int xaccel_build_header(void* source_addr, struct xaccel_desc_header* head_out);


// Reading a function descriptor from mmap address specified at source addr, and saves to header 
int xaccel_build_function_descriptor(void* source_addr, struct xaccel_dev *xdev, struct xaccel_func_desc* desc_out);


// Function to read in raw bytes, return 0 if positive or negative number otherwise
int xaccel_check_header(struct xaccel_desc_header* header);


// Create device for a particular function
int xaccel_create_function_device(struct xaccel_dev *xdev, struct xaccel_func_desc *fdesc, struct xaccel_function* func_obj);


// Destory the devices created in the function
int xaccel_destroy_function_device(struct xaccel_function *func);


// Verifying function's registers are valid
int xaccel_verify_func_regs(struct xaccel_dev *xdev, struct xaccel_func_desc *func_desc);


// Print the xaccel instance (descriptor and functions)
void xaccel_print_xaccel_instance(struct xaccel_dev* xdev);


// Print the xaccel descriptor header
void xaccel_print_desc_header(struct xaccel_desc_header* head);


// Print the xaccel function descriptors
void xaccel_print_func_desc(struct xaccel_func_desc desc);


#endif
