/*
 * Author: Taco
 * Purpose: This fuction is the userspace contract with kernel space
 * Date Created: 30 March 2026
 */

#ifndef XACCEL_UAPI_H
#define XACCEL_UAPI_H

#ifdef __KERNEL__ 
    #include <linux/ioctl.h>
    #include <linux/types.h>
#else
    #include <sys/ioctl.h>
    #include <stdint.h>

typedef uint16_t __u16;
typedef uint32_t __u32;
#endif

#include "../include/xaccel_desc.h"

#define XACCEL_IOC_MAGIC        'X'

// Current capabilties that driver can expose to user space (To be expanded)
#define XACCEL_IOC_GET_INFO	_IOR(XACCEL_IOC_MAGIC, 0, struct xaccel_info)
#define XACCEL_IOC_READ_REG 	_IOWR(XACCEL_IOC_MAGIC, 1, struct xaccel_reg_io)
#define XACCEL_IOC_WRITE_REG	_IOW(XACCEL_IOC_MAGIC, 2, struct xaccel_reg_io)

#define XACCEL_FUNC_TYPE_INVALID 	0
#define XACCEL_FUNC_TYPE_VENDOR_DEFINED 0xFFFF
// Demo reference function types
#define XACCEL_FUNC_TYPE_VECTOR_ADD     2
#define XACCEL_FUNC_TYPE_SHA256_BLOCK   3
#define XACCEL_FUNC_TYPE_VENDOR_DEFINED 0xFFFF

struct xaccel_reg_io
{
    __u32 offset; 	// Offset within this functions MMIO window
    __u32 value;        // Value to be read back or value to write to 
};

struct xaccel_info
{
    __u16 func_id;
    __u16 func_type;
    __u16 func_version;
    __u16 irq_index;

    __u32 mmio_size;
    __u32 caps;
    __u32 reg_layout_ver;
};

#endif
