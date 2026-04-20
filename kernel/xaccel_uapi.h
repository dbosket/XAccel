/*
 * Author: Taco
 * Purpose: This fuction is the userspace contract with kernel space
 * Date Created: 30 March 2026
 */

#ifndef XACCEL_UAPI_H
#define XACCEL_UAPI_H

#ifdef __KERNEL__ 
    #include <linux/ioctl.h>
#else
    #include <sys/ioctl.h>
#endif

#include "../include/xaccel_desc.h"

#define XACCEL_IOC_MAGIC        'x'
#define XACCEL_IOC_GET_INFO	_IOR(XACCEL_IOC_MAGIC, 0, struct xaccel_info)
#define XACCEL_IOC_READ_REG 	_IOWR(XACCEL_IOC_MAGIC, 1, struct xaccel_reg_io)
#define XACCEL_IOC_WRITE_REG	_IOW(XACCEL_IOC_MAGIC, 2, struct xaccel_reg_io)

struct xaccel_reg_io
{
    uint32_t offset; 	// offset within this functions MMIO window
    uint32_t value;        // value to be read back or value to write to 
};

struct xaccel_info
{
    uint16_t func_id;
    uint16_t func_type;
    uint16_t func_version;
    uint16_t irq_index;

    uint32_t mmio_size;
    uint32_t caps;
    uint32_t reg_layout_ver;
};

#endif
