#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "../kernel/xaccel_uapi.h"

int xaccel_ioc_get_info(int fd);
int xaccel_ioc_read(int fd, struct xaccel_reg_io* dest_buf);
int xaccel_ioc_write(int fd, struct xaccel_reg_io *req);

int xaccel_ioc_get_info(int fd)
{
       struct xaccel_info f_info = {-1, -1, -1, -1, -1, -1, -1};

       if (ioctl(fd, XACCEL_IOC_GET_INFO, &f_info) < 0)
       {
	       perror("ERROR: ioctl(get_info) failed...");
	       return EXIT_FAILURE;
       }

       // Printing the info recived
       fprintf(stdout, "FUNC_ID            =%d\n", f_info.func_id);
       fprintf(stdout, "FUNC_TYPE          =%d\n", f_info.func_type);
       fprintf(stdout, "FUNC_VERSION       =%d\n", f_info.func_version);
       fprintf(stdout, "IRQ_INDEX          =%d\n", f_info.irq_index);
       fprintf(stdout, "MMIO_SIZE          =%d\n", f_info.mmio_size);
       fprintf(stdout, "CAPABILITIES       =%d\n", f_info.caps);
       fprintf(stdout, "REG_LAYOUT_VERSION =%d\n", f_info.reg_layout_ver);

       return EXIT_SUCCESS;

}

int xaccel_ioc_read(int fd, struct xaccel_reg_io *req)
{
    if (ioctl(fd, XACCEL_IOC_READ_REG, req) < 0 )
    {
        perror("ERROR: ioctl(read) failed...");
	return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int xaccel_ioc_write(int fd, struct xaccel_reg_io *req)
{
    if (ioctl(fd, XACCEL_IOC_WRITE_REG, req) < 0)
    {
        perror("ERROR: ioctl(write) failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


static void print_usage(const char *prog)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s <device> info\n", prog);
    fprintf(stderr, "  %s <device> read  <offset>\n", prog);
    fprintf(stderr, "  %s <device> write <offset> <value>\n", prog);
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s /dev/xaccel0_func0 info\n", prog);
    fprintf(stderr, "  %s /dev/xaccel0_func0 read 0x0\n", prog);
    fprintf(stderr, "  %s /dev/xaccel0_func0 write 0x0 0xdeadbeef\n", prog);
}

static uint32_t parse_u32(const char *s)
{
    char *end = NULL;
    errno = 0;

    unsigned long val = strtoul(s, &end, 0);

    if (errno || end == s || *end != '\0' || val > UINT32_MAX) {
        fprintf(stderr, "ERROR: invalid u32 value: %s\n", s);
        exit(EXIT_FAILURE);
    }

    return (uint32_t)val;
}



int main (int argc, char** argv)
{
	if (argc < 3)
	{
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	const char *dev_path = argv[1];
	const char *cmd = argv[2];

	fprintf(stdout, "Device path: %s\n", dev_path);

	int fd = open(dev_path, O_RDWR);
    	if (fd < 0)
    	{
		fprintf(stderr, "ERROR: open() failed...\n");
		return EXIT_FAILURE;
   	}

	fprintf(stdout, "Opened %s successfully\n", dev_path);
	int ret = EXIT_SUCCESS;

	if (strcmp(cmd, "info") ==0)
		ret= xaccel_ioc_get_info(fd);
	else if (strcmp(cmd, "read") == 0) 
	{
		if (argc < 4)
		{
			print_usage(argv[0]);
			ret = EXIT_FAILURE;
			goto out;
		}
		struct xaccel_reg_io req = 
		{
	    		.offset = parse_u32(argv[3]),
			.value  = 0,
    		};

		ret = xaccel_ioc_read(fd, &req);

		if (ret == EXIT_SUCCESS){
			printf("READ: offset=0x%x value=0x%x\n", req.offset, req.value);
		}
	}
	else if (strcmp(cmd, "write") == 0)
	{
	       if (argc < 5)
	       {
			print_usage(argv[0]);
 			ret = EXIT_FAILURE;		
			goto out;
	       }
		struct xaccel_reg_io req = 
		{
	    		.offset = parse_u32(argv[3]),
			.value  = parse_u32(argv[4]),
    		};

		ret = xaccel_ioc_write(fd, &req);

		if (ret == EXIT_SUCCESS){
			printf("WRITE: offset=0x%x value=0x%x\n", req.offset, req.value);
		}
	}
	else
	{
 		fprintf(stderr, "ERROR: unknown command: %s\n", cmd);
        	print_usage(argv[0]);
        	ret = EXIT_FAILURE;
	}

out:
	close(fd);
	return ret;
}


