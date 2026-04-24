#include "xaccel_core.h"


// Creates an instance of xaccel device base on mmio region
int xaccel_create_instance(void* base_addr, struct xaccel_dev* xdev, struct file_operations xaccel_fops)
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

	xdev->funcs = NULL; //Init to null for later checks
	
#ifndef NO_HW
	//TODO: Initialize mmio_base, mmio_size, from pdev?
#endif
	
	// Initialize Semaphore
	pr_info("Initializaing the semaphore\n");
	sema_init(&(xdev->sem), MAX_LOCK_HOLDERS);
	down(&(xdev->sem));

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
		pr_info("ERROR: Failed to build descripter header...");
		xaccel_cleanup(xdev);
		return -1;
		
	}	
	xdev->hdr = desc_head;
	

	// Allocating space for function array
	xdev->funcs = kcalloc(desc_head->num_functions, sizeof(struct xaccel_function), GFP_KERNEL);
	if (!xdev->funcs)
		{
			pr_info("ERROR: Failed to allocate memory for function array\n");
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
	xdev->class = class_create(XACCEL_CLASS_NAME);


	struct xaccel_func_desc *func_desc_cur;
	struct xaccel_function  *func_cur;
	
	// Initializing function array
	pr_info("Initializing the function array for this xaccel_device\n");
	void* func_desc_base = (__u8 *)(base_addr) + FUNC_DESC_OFFSET;
	
	for(__u32 i=0; i<xdev->num_functions; i++){

		// Allocate space and initialize current function descriptor	
		func_desc_cur = kmalloc(sizeof(struct xaccel_func_desc), GFP_KERNEL);
		if (!func_desc_cur)
		{
        		pr_err("Failed to Allocate Memory\n");
			return -ENOMEM;
		}

		if (xaccel_build_function_descriptor(func_desc_base, func_desc_cur))
		{
			pr_err("ERROR: Failed to allocate memory for function header array\n");
			xaccel_cleanup(xdev);
			return -ENOMEM;

		}
		func_desc_base = (__u8 *)(func_desc_base) + sizeof(struct xaccel_func_desc);

		if (xaccel_create_function_device(xdev, func_desc_cur, func_cur))
		{
			pr_err("ERROR: Failed to build function devices\n");
			xaccel_cleanup(xdev);
			return -1;
		}

		// Initialize character device for current function
		
		cdev_init(&(func_cur->cdev), &(xaccel_fops));
		func_cur->cdev.owner = THIS_MODULE;
		func_cur->devt = MKDEV(MAJOR(xdev->base_devt), 0);
		cdev_add(&(func_cur->cdev), func_cur->devt, 1);
		func_cur->device = device_create(xdev->class, NULL, func_cur->devt, NULL, "xaccel%d_func%d", 0, i);
		xdev->funcs[i] = *func_cur;
	}
	
	up(&(xdev->sem));
	pr_info("XACCEL_CREATE_INSTANCE() returning successfully...\n");
	return 0;
}

void xaccel_cleanup(struct xaccel_dev *xdev)
{
	pr_info("xaccel_cleanup() starting...");
	if(!xdev) return;

	// Release function objects and cdevs	
	if (xdev->funcs)
	{
		struct xaccel_function* temp_func;
		for (__u32 i=0; i<xdev->num_functions; i++)
		{
			temp_func = &(xdev->funcs[i]);
			device_destroy(xdev->class, temp_func->devt);
			pr_info("Deleting char device for func[%d]\n", i);
			cdev_del(&(temp_func->cdev));
		}
		kfree(xdev->funcs);
		xdev->funcs = NULL;
	}	
	if (xdev->class)
		class_destroy(xdev->class);
	unregister_chrdev_region(xdev->base_devt, xdev->num_functions);
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
    
	pr_info("ERROR: Invalid Header\n");
	return -EFAULT;
}


// Populate a descriptor header object based on address specified at source addr 
int xaccel_build_header(void* source_addr, struct xaccel_desc_header* desc_head)
{
	if (!desc_head || !source_addr)	return -EFAULT;
	pr_info("Populating runtime descriptor header based on emulated mmap region...");

#ifdef DEBUG 
	printk("The value read at 0x%p, offset %x, is %x...", 
		source_addr, 
		XACCEL_MAGIC_OFFSET, 
		xaccel_read32(source_addr, XACCEL_MAGIC_OFFSET));
#endif

	(desc_head)->magic         = xaccel_read32(source_addr, XACCEL_MAGIC_OFFSET);
	(desc_head)->version       = xaccel_read16(source_addr, XACCEL_VERSION_OFFSET);
    	(desc_head)->header_size   = xaccel_read16(source_addr, XACCEL_HEAD_SIZE_OFFSET); 
    	(desc_head)->total_size    = xaccel_read32(source_addr, XACCEL_TOT_SIZE_OFFSET);
    	(desc_head)->num_functions = xaccel_read32(source_addr, XACCEL_NUM_FUNC_OFFSET);
    	(desc_head)->flags         = xaccel_read16(source_addr, XACCEL_FLAG_OFFSET);
    	(desc_head)->checksum      = xaccel_read32(source_addr, XACCEL_CHECKSUM_OFFSET);
    	(desc_head)->device_id     = xaccel_read32(source_addr, XACCEL_DEVICE_ID_OFFSET);
		
#ifdef DEBUG 
	pr_info("MAGIC Value is: %x...\n", (desc_head)->magic);
#endif
  	return 0;
}

// Populate a function descriptor object based on address specified at source_addr
int xaccel_build_function_descriptor(void* source_addr, struct xaccel_func_desc* func_desc)
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
    	return 0;
}



int xaccel_create_function_device(struct xaccel_dev *xdev, struct xaccel_func_desc *fdesc, struct xaccel_function* func_obj)
{
    return 0;
}

int xaccel_destroy_function_device(struct xaccel_dev *xdev)
{
    return 0;
}


