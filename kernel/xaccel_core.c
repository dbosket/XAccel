#include "xaccel_core.h"


// Creates an instance of xaccel device base on mmio region
int xaccel_create_instance(void* base_addr, struct xaccel_dev* xdev, struct file_operations *xaccel_fops)
{
	pr_info("Creating instance of xaccel device");

	if (!base_addr)
	{
		pr_err("ERROR: Invalid Base Addr\n");
		return -EFAULT;
	}
	if(!xdev)
	{
		pr_err("ERROR: Invalid Pointer to xdev obj passed\n");
		return -EFAULT;
	}
	xdev->funcs = NULL;
	
#ifndef NO_HW
	//TODO: Initialize mmio_base, mmio_size, from pdev?
#endif
	
	// Initialize Semaphore
	pr_info("Initializaing the semaphore\n");
	sema_init(&(xdev->sem), MAX_LOCK_HOLDERS);
	//down(&(xdev->sem));

	// Build and Parse Descriptor Header
	struct xaccel_desc_header* desc_head;
	desc_head = kzalloc(sizeof(struct xaccel_desc_header), GFP_KERNEL);
	if (!desc_head)
	{
		pr_err("Failed to Allocate Memory\n");
		xaccel_cleanup(xdev);
		return -ENOMEM;
	}

	if (xaccel_build_header(base_addr, desc_head) || xaccel_check_header(desc_head))
	{
		pr_err("ERROR: Failed to build descripter header...");
		xaccel_cleanup(xdev);
		return -1;
		
	}	
	xdev->hdr = desc_head;
	xdev->num_functions = desc_head->num_functions;
	xdev->mmio_base = base_addr;

	// Allocating space for function array
	xdev->funcs = kcalloc(desc_head->num_functions, sizeof(struct xaccel_function), GFP_KERNEL);
	if (!xdev->funcs)
		{
			pr_err("ERROR: Failed to allocate memory for function array\n");
			xaccel_cleanup(xdev);
			return -ENOMEM;
		}

	pr_info("Allocating device numbers for character devices\n");
	if (alloc_chrdev_region(&(xdev->base_devt), FIRST_MINOR, xdev->num_functions, XACCEL_NAME))
		{
			pr_err("ERROR: alloc_chrdev_region_failed...\n");
			xaccel_cleanup(xdev);
			return -1;
		}

	// Creating class for xaccel device
	pr_info("Creating class for xaccel devices\n");

	// Initialize reminader of xaccel device instance
	xdev->class = class_create(XACCEL_CLASS_NAME);

	// Temporary variables for function descriptors and function objects
	struct xaccel_func_desc *func_desc_cur;
	struct xaccel_function  *func_cur;
	
	// Initializing function array
	pr_info("Initializing the function array for this xaccel_device\n");
	void* func_desc_base = (__u8 *)(base_addr) + FUNC_DESC_OFFSET;
	
	for(__u32 i=0; i<xdev->num_functions; i++){

		func_desc_cur = &(xdev->funcs[i].desc);

		// For the function descriptor
		if (xaccel_build_function_descriptor(func_desc_base, xdev, func_desc_cur))
		{
			pr_err("ERROR: Failed to build function descriptor\n");
			xaccel_cleanup(xdev);
			return -ENOMEM;

		}
		// For the runtime function object
		func_desc_base = (__u8 *)(func_desc_base) + sizeof(struct xaccel_func_desc);

		func_cur = &(xdev->funcs[i]);
		if (xaccel_create_function_device(xdev, func_desc_cur, func_cur))
		{
			pr_err("ERROR: Failed to build function devices\n");
			xaccel_cleanup(xdev);
			return -1;
		}
		// Initialize character device for current function
		pr_info("Initializing cdev...");	
		cdev_init(&(func_cur->cdev), xaccel_fops);
		func_cur->cdev.owner = THIS_MODULE;
		func_cur->devt = MKDEV(MAJOR(xdev->base_devt), MINOR(xdev->base_devt) + i);
		pr_info("Addding cdev...");
		int ret = cdev_add(&(func_cur->cdev), func_cur->devt, 1);
		if (ret)
		{
			pr_err("cdev_add failed for func %u: %d\n", i, ret);
			return -1;
		}
		// Creating Device for the function
		pr_info("Creating device for function [%d] i",i);
		func_cur->device = device_create(xdev->class, NULL, func_cur->devt, NULL, "xaccel%d_func%d", 0, i);

		if (IS_ERR(func_cur->device)) {
    			ret = PTR_ERR(func_cur->device);
    			pr_err("device_create failed for func %d: %d\n", i, ret);
    			cdev_del(&func_cur->cdev);
    			xaccel_cleanup(xdev);
    			return ret;
		}
	}
	//up(&(xdev->sem));
	pr_info("XACCEL_CREATE_INSTANCE() returning successfully...\n");
	return 0;
}


void xaccel_cleanup(struct xaccel_dev *xdev)
{
	pr_info("XACCEL_CLEANUP()...");
	if(!xdev) return;

	// Release function objects and cdevs	
	if (xdev->funcs)
	{
		struct xaccel_function* func_cur;
		for (__u32 i=0; i<xdev->num_functions; i++)
		{
			func_cur = &(xdev->funcs[i]);
			xaccel_destroy_function_device(func_cur);
		}
		kfree(xdev->funcs);
		xdev->funcs = NULL;
	}	
	if (xdev->class)
		class_destroy(xdev->class);
	unregister_chrdev_region(xdev->base_devt, xdev->num_functions);
	if (xdev->hdr)
		kfree(xdev->hdr);
	
	kfree(xdev);
	xdev = NULL;
	pr_info("XACCEL_CLEANUP() Returning Successfully...\n");
	return;
}


// Is this descriptor valid
int xaccel_check_header(struct xaccel_desc_header* header)
{
	if (!header) return -EFAULT;
	if (header->magic == XACCEL_DESC_MAGIC) return 0;
    
	pr_err("ERROR: Invalid Header\n");
	return -EFAULT;
}


// Populate a descriptor header object based on address specified at source addr 
int xaccel_build_header(void* source_addr, struct xaccel_desc_header* desc_head)
{
	if (!desc_head || !source_addr)	return -EFAULT;
	pr_info("Populating runtime descriptor header based on emulated mmap region...");

	pr_info("The value read at 0x%p, offset %x, is %x...", 
		source_addr, 
		XACCEL_MAGIC_OFFSET, 
		xaccel_read32(source_addr, XACCEL_MAGIC_OFFSET));

	(desc_head)->magic         = xaccel_read32(source_addr, XACCEL_MAGIC_OFFSET);
	(desc_head)->version       = xaccel_read16(source_addr, XACCEL_VERSION_OFFSET);
    	(desc_head)->header_size   = xaccel_read16(source_addr, XACCEL_HEAD_SIZE_OFFSET); 
    	(desc_head)->total_size    = xaccel_read32(source_addr, XACCEL_TOT_SIZE_OFFSET);
    	(desc_head)->num_functions = xaccel_read32(source_addr, XACCEL_NUM_FUNC_OFFSET);
    	(desc_head)->flags         = xaccel_read16(source_addr, XACCEL_FLAG_OFFSET);
    	(desc_head)->checksum      = xaccel_read32(source_addr, XACCEL_CHECKSUM_OFFSET);
    	(desc_head)->device_id     = xaccel_read32(source_addr, XACCEL_DEVICE_ID_OFFSET);
		
	pr_info("MAGIC Value is: %x...\n", (desc_head)->magic);

  	return 0;
}

// Populate a function descriptor object based on address specified at source_addr
int xaccel_build_function_descriptor(void* source_addr, struct xaccel_dev* xdev, struct xaccel_func_desc* func_desc)
{
	if (!func_desc || !source_addr) return -EFAULT;
	
	(func_desc)->func_id        = xaccel_read16(source_addr, XACCEL_FUNC_ID_OFFSET);
    	(func_desc)->func_type      = xaccel_read16(source_addr, XACCEL_FUNC_TYPE_OFFSET);
    	(func_desc)->func_version   = xaccel_read16(source_addr, XACCEL_FUNC_VERS_OFFSET);
    	(func_desc)->irq_index      = xaccel_read16(source_addr, XACCEL_IRQ_INDEX);
    	(func_desc)->mmio_offset    = xaccel_read32(source_addr, XACCEL_MMIO_OFFSET);
    	(func_desc)->mmio_size      = xaccel_read32(source_addr, XACCEL_MMIO_SIZE);
    	(func_desc)->caps           = xaccel_read32(source_addr, XACCEL_CAPS);
    	(func_desc)->reg_layout_ver = xaccel_read32(source_addr, XACCEL_REG_LAYOUT_VER);
    	(func_desc)->ext_offset     = xaccel_read32(source_addr, XACCEL_EXT_OFFSET);
    	(func_desc)->ext_size       = xaccel_read32(source_addr, XACCEL_EXT_SIZE);

	if (xaccel_verify_func_regs(xdev, func_desc)) return -EINVAL;
    	return 0;
}



int xaccel_create_function_device(struct xaccel_dev *xdev, struct xaccel_func_desc *func_desc, struct xaccel_function* func)
{
	if (!xdev || !func_desc || !func) return -EFAULT;

	func->parent     = xdev;
	func->desc       = *func_desc;
	func->regs       = (__u8 *)xdev->mmio_base + func_desc->mmio_offset;
	func->open_count = 0;
	mutex_init(&(func->lock));
	return 0;
}


// This function will clean up the resources allocated for one runtime function object
int xaccel_destroy_function_device(struct xaccel_function* func)
{
	if (!func) return -EFAULT;
    	// Clean of character device registration
    	device_destroy(func->parent->class, func->devt);
	cdev_del(&(func->cdev));
    
    	// Disassociate the shared header from this function device
    	func->parent = NULL;	
    return 0;
}

int xaccel_verify_func_regs(struct xaccel_dev *xdev, struct xaccel_func_desc *func_desc)
{
	if (!xdev || !func_desc) return -EFAULT;
	
	__u8 *mmap_boundary     = (__u8 *) xdev->mmio_base + xdev->mmio_size;
	__u8 *cur_func_base     = (__u8 *) xdev->mmio_base + func_desc->mmio_offset;
	__u8 *cur_func_boundary = (__u8 *) cur_func_base + func_desc->mmio_size;
	__u8 *ext_base          = (__u8 *) xdev->mmio_base + func_desc->ext_offset;
	__u8 *ext_boundary	= (__u8 *) ext_base + func_desc->ext_size;
	
	// Verify that Function's registers do not extend beyond total mmaped region
	pr_info("Checking that function boundary is not beyond mmio boundary...");
	if (mmap_boundary < cur_func_boundary) 
	{
		pr_info("MMIO boundary %p, Function Boundary %p...", mmap_boundary, cur_func_base);
		pr_err("Current function boundary is beyond the mmio_boundary");	
		return -EINVAL;	
	}
	
	if (func_desc->ext_size > 0 )
	{
		pr_info("Checking that extension block is not beyond mmio boundary...");
		if (mmap_boundary < ext_boundary) return -EINVAL;
	}
	return 0;
}
	
void xaccel_print_desc_header(struct xaccel_desc_header* head)
{
	if (!head) return;
	pr_info("Printing Descriptor Header...");
	pr_info("Header: Magic Number %x", head->magic);
	pr_info("Header: Version %x", head->version);
	pr_info("Header: Header Size %x", head->header_size);
	pr_info("Header: Total Size %x", head->total_size);
	pr_info("Header: Num Functions %d", head->num_functions);
	pr_info("Header: Flag  %x", head->flags);
	pr_info("Header: Checksum %x", head->checksum);
	pr_info("Header: Device Id %x", head->device_id);
}


void xaccel_print_func_desc(struct xaccel_func_desc desc)
{
	pr_info("Function: ID %d", desc.func_id);
	pr_info("Function: Type %x", desc.func_type);
	pr_info("Function: Version %d", desc.func_version);
	pr_info("Function: IRQ Index %d", desc.irq_index);
	pr_info("Function: MMIO Offset %x", desc.mmio_offset);
	pr_info("Function: MMIO Size %x", desc.mmio_size);
	pr_info("Function: Caps  %d", desc.caps);
	pr_info("Function: Reg Layout Ver %d", desc.reg_layout_ver);
	pr_info("Function: Ext Offset %x", desc.ext_offset);
	pr_info("Function: Ext Size %x", desc.ext_size);
}


void xaccel_print_xaccel_instance(struct xaccel_dev* xdev)
{
	xaccel_print_desc_header(xdev->hdr);

	pr_info("Printing Function Descriptors...");
	for (int i=0; i<xdev->num_functions; i++)
	{
		pr_info("Function [%d]...", i);
		xaccel_print_func_desc(((xdev)->funcs[i]).desc);
	}
}


