/*
 * Author: Taco
 * Purpose: This fuction is the userspace contract with kernel space
 * Date Created: 30 March 2026
 */

#ifndef XACCEL_UAPI_H
#define XACCEL_UAPI_H

#include "linux/ioctl.h"
#include "../include/xaccel_desc.h"

#define XACCEL_IOC_MAGIC        'x'
#define XACCEL_IOC_GET_INFO	_IOR(XACCEL_IOC_MAGIC, 0, struct xaccel_info)
#define XACCEL_IOC_READ_REG 	_IOWR(XACCEL_IOC_MAGIC, 1, struct xaccel_reg_io)
#define XACCEL_IOC_WRITE_REG	_IOW(XACCEL_IOC_MAGIC, 2, struct xaccel_reg_io)

struct xaccel_reg_io
{
    __u32 offset; 	// offset within this functions MMIO window
    __u32 value;        // value to be read back or value to write to 
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
