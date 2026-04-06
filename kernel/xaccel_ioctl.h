/*
 * Author: Taco
 * Purpose: This fuction is the userspace contract with kernel space
 * Date Created: 30 March 2026
 */

#include "../include/xaccel_desc.h"

#define XACCEL_IOC_GET_INFO	__IOR(...)
#define XACCEL_IOC_READ_REG 	__IOWr(...)
#define XACCEL_IOC_WRITE_REG	__IOw(...)


struct xaccel_reg_io;
struct xaccel_info;
