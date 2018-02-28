/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>

//extern void remove_device(struct device_t * dev);
static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

static int do_test(int argc, char ** argv)
{
	s32_t base_addr = 0, nbytes = 64;
		s32_t i, size = 1;
		u8_t linebuf[16], line_len;

		if(argc < 2)
		{
			usage();
			return -1;
		}

		for(i=1; i<argc; i++)
		{
			if( !strcmp((const char *)argv[i],"-b") )
				size = 1;
			else if( !strcmp((const char *)argv[i],"-w") )
				size = 2;
			else if( !strcmp((const char *)argv[i],"-l") )
				size = 4;
			else if( !strcmp((const char *)argv[i],"-c") && (argc > i+1))
			{
				nbytes = strtoul((const char *)argv[i+1], NULL, 0);
				i++;
				printf("%d\r\n" ,size);
			}
			else if(*argv[i] == '-')
			{
				usage();
				return (-1);
			}
			else if(*argv[i] != '-' && strcmp((const char *)argv[i], "-") != 0)
			{
				base_addr = strtoul((const char *)argv[i], NULL, 0);
			}
		}

		if(size == 2)
		{
			base_addr = base_addr & (~0x00000001);
		}
		else if(size == 4)
		{
			base_addr = base_addr & (~0x00000003);
		}
		printf("nb%d\r\n ",nbytes);
		nbytes = nbytes * size;
		printf("nb*size%d\r\n" ,nbytes);

	/*char json[512];
		int length;


		memset(json, 0, sizeof(json));
		length = sprintf(json, "{\"fb-pl111@0x10020000\":{\"width\":320,\"height\":240,\"physical-width\":216,\"physical-height\":135,\"bits-per-pixel\":32,\"clock-frequency\":50000000,\"hfront-porch\":1,\"hback-porch\":1,\"hsync-len\":1,\"vfront-porch\":1,\"vback-porch\":1,\"vsync-len\":1,\"hsync-active\":false,\"vsync-active\":false,\"de-active\":false,\"pixelclk-active\":false}}");
		//probe_device(json, length);
		probe_device(json, 425);
		//probe_device(json, 425);
		struct device_t * fb;
		const char * name="fb-pl111.0";
		fb= search_first_device (   DEVICE_TYPE_FRAMEBUFFER);
		fb= search_device (  name, DEVICE_TYPE_FRAMEBUFFER);
		//struct device_t * fb-pl111;
		if (*argv[1]=="no")
		remove_device(fb);
		if (*argv[2]=="no")
				remove_device(fb);
		//probe_device(json, 425);	*/
	return 0;
}

static struct command_t cmd_test = {
	.name	= "test",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_test,
};

static __init void test_cmd_init(void)
{
	register_command(&cmd_test);
}

static __exit void test_cmd_exit(void)
{
	unregister_command(&cmd_test);
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);
