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







// Initialize the the xaccelerator object
int xaccel_core_init(struct device *dev, void __iomem *base, size_t mmio_size);

// Clean up the accelerator object 
void xaccel_core_cleanup(struct xaccel_dev *xdev);

// Parse the descriptor recieved
int xaccel_core_parse_descriptor(struct xaccel_dev *xdev);

// Create device for a particular function
int xaccel_core_create_function_devices(struct xaccel_dev *xdev);

// Destory the devices created in the function
int xaccel_core_destroy_function_device(struct xaccel_dev *xdev);


// Function to read in raw bytes, return 0 if positive or negative number otherwise
int check_descriptor_header(struct xaccel_desc_header* header);

// Read in from a MM address of x bytes to build a struct descriptor header
int build_descriptor_header(void* source_addr, struct xaccel_desc_header* head_out);


// Read in from a MM address of of x bytes based on size specified in header
int build_function_header(void* source_addr, struct xaccel_func_desc* head_out);


#endif
