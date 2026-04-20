
int main (int argc, char* argv)
{
    int fd = open("/dev/xaccel0_func0", O_RDWR);
    if (fd < 0)
	    return EXIT_FAILURE;
    
    struct xaccel_reg_io_req = {
	    .offset = 0x0,
    };

    ioctl(fd, XACCEL_IOC_READ_REG, &req);
    printf("val = 0x%x\n", req.value);
    return 0;
