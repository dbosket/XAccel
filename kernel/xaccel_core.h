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
#include <linux/types.h>
#include <linux/mutex.h>

#include "../include/xaccel_desc.h"
#include "../include/xaccel_macros.h"

#ifdef debug
	#define XACCEL_REG_CONTROL    0x00
	#define XACCEL_REG_STATUS     0X04
	#define XACCEL_REG_ARG0       0x08
	#define XACCEL_REG_ARG1       0X0C
	#define XACCEL_REG_ARG2       0x10
	#define XACCEL_REG_ARG3       0X14
	#define XACCEL_REG_RESULT0    0x18
	#define XACCEL_REG_RESULT1    0X1C


	#define XACCEL_CNTRL_START    (1U << 0)
	#define XACCEL_CNTRL_RESET    (1U << 1)
	
	#define XACCEL_STATUS_DONE    (1U << 0)
	#define XACCEL_STATUS_BUSY    (1U << 1)
	#define XACCEL_STATUS_ERROR   (1U << 2)

	#define XACCEL_REG_LAYOUT_V1 1


#endif


struct xaccel_function
{
	struct xaccel_dev *parent;
	struct xaccel_func_desc desc;
	void __iomem *regs;
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

        struct xaccel_desc_header hdr;  /* Parsed top-level descriptor header */

        u16 num_functions;              /* Number of parsed/discovered function */
        struct xaccel_function *funcs;  /* Array of runtime function objects */

        dev_t base_devt;                /* Base dev_t function for function devices */
        struct class *class;            /* /sys/class/xaccel */

        struct semaphore sem;           /* Projects shared device-wide state */
};


// Create instance of struct xaccel_device
int xaccel_create_instance(void* mmio_base);


// Destroy instance of struct xaccel_device
int xaccel_destroy_instance(struct xaccel_dev* xdev);


// Clean up any memory allocated by xaccel_dev 
void xaccel_cleanup(struct xaccel_dev *xdev);


// Parse the descriptor recieved
int xaccel_build_descriptor_header(void* source_addr, struct xaccel_desc_header* head_out);

// Reading a function descriptor from mmap address specified at source addr, and saves to header 
int xaccel_build_function_descriptor(void* source_addr, struct xaccel_func_desc* desc_out)

// Function to read in raw bytes, return 0 if positive or negative number otherwise
int xaccel_check_descriptor_header(struct xaccel_desc_header* header);

// Create device for a particular function
int xaccel_core_create_function_devices(struct xaccel_dev *xdev);

// Destory the devices created in the function
int xaccel_core_destroy_function_device(struct xaccel_dev *xdev);


// Write 32 bytes to either hardware or kernel memory
static inline void xaccel_write32(void* base, __u32 offset, __u32 val)
{
	#ifdef NO_HW
	*(__u32 *)((__u8 *)base + offset) = val;
	#else
	iowrite32(val, base + offset);
	#endif
}

// Write 16 bytes to either hardware or kernel memory
static inline void xaccel_write16(void* base, __u16 offset, __u16 val)
{
	#ifdef NO_HW
	*(__u16 *)((__u8 *)base + offset) = val;
	#else
	iowrite16(val, base + offset);
	#endif
}

// Read 32 bytes from hardware or kernel memory
static inline void xaccel_read32(void* base, __u32 offset)
{
	#ifdef NO_HW
	return *(__u32 *)((__u8 *)base + offset);
	#else
	return ioread32((void __iomem *)base + offset);	
	#endif
}

// Read 16 bytes from hardware or kernel memory
static inline void xaccel_read16(void* base, __u16 offset)
{
	#ifdef NO_HW
	return *(__u16 *)((__u8 *)base + offset);
	#else
	return ioread16((void __iomem *)base + offset);	
	#endif
}


#endif
