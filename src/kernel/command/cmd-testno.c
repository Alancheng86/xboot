/*
 * kernel/command/cmd-test2.c
 */

#include <command/command.h>

//extern void remove_device(struct device_t * dev);
static void usage(void)
{
	printf("usage:\r\n");
	printf("    test2 [args ...]\r\n");
}

static int do_test2(int argc, char ** argv)
{
		char json[512];
		int length;


		memset(json, 0, sizeof(json));
		length = sprintf(json, "{\"fb-pl111@0x10020000\":{\"width\":320,\"height\":240,\"physical-width\":216,\"physical-height\":135,\"bits-per-pixel\":32,\"clock-frequency\":50000000,\"hfront-porch\":1,\"hback-porch\":1,\"hsync-len\":1,\"vfront-porch\":1,\"vback-porch\":1,\"vsync-len\":1,\"hsync-active\":false,\"vsync-active\":false,\"de-active\":false,\"pixelclk-active\":false}}");
		//probe_device(json, length);
	//	probe_device(json, 425);
		//probe_device(json, 425);
		struct device_t * fb;
		const char * name="fb-pl111.0";
		fb= search_first_device (   DEVICE_TYPE_FRAMEBUFFER);
		//fb= search_device (  name, DEVICE_TYPE_FRAMEBUFFER);
		//struct device_t * fb-pl111;
		//if (argv[1]=="no")
		remove_device(fb);
		//if (argv[2]=="no")
		//		remove_device(fb);
		//probe_device(json, 425);
	return 0;
}

static struct command_t cmd_test2 = {
	.name	= "test2",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_test2,
};

static __init void test2_cmd_init(void)
{
	register_command(&cmd_test2);
}

static __exit void test2_cmd_exit(void)
{
	unregister_command(&cmd_test2);
}

command_initcall(test2_cmd_init);
command_exitcall(test2_cmd_exit);
