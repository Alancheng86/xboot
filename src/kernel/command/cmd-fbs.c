/*
 * kernel/command/cmd-fbs.c
 */
#include <command/command.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "cJSON.h"
#include <framebuffer/framebuffer.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    fbs remove   :remove fb\r\n");
	printf("    fbs width n height n [vbp n vfp n vpw n hfp n hpw n hbp n pclk n...]\r\n");
	printf("    n=1,2,3....; HT= hfp+width+hbp+hpw; VT=vfp+heitht+vfp+vpw \r\n");
	printf("    fbs  : load default timing\r\n");
	printf("    fbs  bl n : set backlight(0~1000)\r\n");
}

int isalpha_string( char *s ) //判断字符串是否全是字母 
{
    int i;
    for( i=0;s[i];i++ )
    {
        if ( !islower(s[i]) && !isupper(s[i]) ) //不是小写，也不是大写，则返回非零值 
            return -1;
    }
    return 0; //全是字母返回是0
}

int isNum(char *str)  //判断字符串是否全是数字 
{   
    int i;
    for(i = 0; str[i] != 0; i++)
    {   if(!isdigit(str[i]))
         return -1;
    }
    return 0;
}  

static int do_fbs(int argc, char ** argv)
{
    cJSON *fbjson; 	
    cJSON *js_list;
	char *out; int i;
	struct framebuffer_t * fbb;	//供开启背光用

	if((argc-1)%2)  //判断参数数量是否为2的整数倍
	{
		if (!strcmp((const char *)argv[1], "remove"))
		{
			struct device_t * fb;
			//const char * name="fb-pl111.0";
			//fb= search_device (  name, DEVICE_TYPE_FRAMEBUFFER);
			fb= search_first_device (   DEVICE_TYPE_FRAMEBUFFER);
			remove_device(fb);		//// 模拟器貌似会挂
			return 0;
		}
		else
		{
			printf(" timing parameter number or type Err\r\n");
			return -1; 
		}		
	}

	for(i=1; i<argc; i++)   //判断传入的参数是否符合：（字母参数）+（数字参数的格式）
	{
		if( !isalpha_string(( char *)argv[i]) )
	         {
               i++; 
             }
		else 
             {
                   printf(" parameter: %s Not Characters\r\n",( char *)argv[i]);
                   return -1;
             }
			
		 if( isNum(( char *)argv[i] ))
		 	 {
                    printf(" parameter: %s Not Number\r\n",( char *)argv[i]);
                    return -1; 
             }
		 //TODO 待添加参数检测功能
    }
//---------------------------------------------------------------------
/*  1. cJSON_CreateObject函数可创建一个根数据项，之后便可向该根数据项中添加string或int等内容,返回的是一个 cJSON的指针，注意，在这个指针用完了以后，需要手工调用 cJSON_Delete(root); 进行内存回收。
	2. cJSON_AddNumberToObject向节点中添加子节点
	3. 需要注意的是  json 格式的数据，虽然也是一个字符串的样子，但这个时候还是无法当成普通的字符串进行使用，需要调用 cJSON_PrintUnformatted(root) 或者 cJSON_Print(root);来将json对象转换成普通的字符串，并且都是以该json对象的根为基点。两个API的区别即是：一个是没有格式的：也就是转换出的字符串中间不会有"\n" "\t"之类的东西存在，而cJSON_Print(root);打印出来是人看起来很舒服的格式。
	4. 因为函数内部封装有malloc函数，所以使用free函数释放被out占用的内存空间
	5. json压缩转义可通过https://www.sojson.com网站完成，压缩转义前请先校验	*/
	//realview:
	//out="{\"fb-pl111@0x10020000\":{\"width\":640,\"height\":480,\"physical-width\":216,\"physical-height\":135,\"bits-per-pixel\":32,\"clock-frequency\":50000000,\"hfront-porch\":1,\"hback-porch\":1,\"hsync-len\":1,\"vfront-porch\":1,\"vback-porch\":1,\"vsync-len\":1,\"hsync-active\":false,\"vsync-active\":false,\"de-active\":false,\"pixelclk-active\":false}}";
	out="{\"fb-f1c100s@0\":{\"clock-name-defe\":\"link-defe\",\"clock-name-debe\":\"link-debe\",\"clock-name-tcon\":\"link-tcon\",\"reset-defe\":46,\"reset-debe\":44,\"reset-tcon\":36,\"width\":800,\"height\":480,\"physical-width\":216,\"physical-height\":135,\"bits-per-pixel\":18,\"bytes-per-pixel\":4,\"clock-frequency\":33000000,\"hfront-porch\":40,\"hback-porch\":87,\"hsync-len\":1,\"vfront-porch\":13,\"vback-porch\":31,\"vsync-len\":1,\"hsync-active\":false,\"vsync-active\":false,\"den-active\":false,\"clk-active\":false,\"backlight\":\"led-pwm-bl.0\"}}";
	//printf(" %s \n",out);

	fbjson = cJSON_Parse(out); //解析成json形式	
	js_list= cJSON_GetObjectItem(fbjson, "fb-f1c100s@0");   // 此处注意节点名称

	for(i=1; i<argc; i++)   //
	{
		if (!strcmp((const char *)argv[i], "width"))
		{
			cJSON_DeleteItemFromObject(js_list,"width"); //删除 width
			i++;
			cJSON_AddNumberToObject(js_list,"width",atoi((const char *)argv[i])); //添加 width
			
		}
		else if (!strcmp((const char *)argv[i], "height"))
		{
			cJSON_DeleteItemFromObject(js_list,"height"); //删除 height
			i++;
			cJSON_AddNumberToObject(js_list,"height",atoi((const char *)argv[i])); //添加 height
			
		}
		else if (!strcmp((const char *)argv[i], "vfp"))
		{
			cJSON_DeleteItemFromObject(js_list,"vfront-porch"); //删除 
			i++;
			cJSON_AddNumberToObject(js_list,"vfront-porch",atoi((const char *)argv[i])); //添加 
			
		}
		else if (!strcmp((const char *)argv[i], "vpw"))
		{
			cJSON_DeleteItemFromObject(js_list,"vsync-len"); //删除 vpw
			i++;
			cJSON_AddNumberToObject(js_list,"vsync-len",atoi((const char *)argv[i])); //添加 vpw
			
		}
		else if (!strcmp((const char *)argv[i], "vbp"))
		{
			cJSON_DeleteItemFromObject(js_list,"vback-porch"); //删除 vbp
			i++;
			cJSON_AddNumberToObject(js_list,"vback-porch",atoi((const char *)argv[i])); //添加 vpb
			
		}
				
		else if (!strcmp((const char *)argv[i], "hfp"))
		{
			cJSON_DeleteItemFromObject(js_list,"hfront-porch"); //删除 
			i++;
			cJSON_AddNumberToObject(js_list,"hfront-porch",atoi((const char *)argv[i])); //添加 
			
		}
		else if (!strcmp((const char *)argv[i], "hpw"))
		{
			cJSON_DeleteItemFromObject(js_list,"hsync-len"); //删除hpw
			i++;
			cJSON_AddNumberToObject(js_list,"hsync-len",atoi((const char *)argv[i])); //添加 hpw
			
		}
		else if (!strcmp((const char *)argv[i], "hbp"))
		{
			cJSON_DeleteItemFromObject(js_list,"hback-porch"); //删除 hbp
			i++;
			cJSON_AddNumberToObject(js_list,"hback-porch",atoi((const char *)argv[i])); //添加 hbp
			
		}
		else if (!strcmp((const char *)argv[i], "pclk"))
		{
			cJSON_DeleteItemFromObject(js_list,"clock-frequency"); //删除 pclk
			i++;
			cJSON_AddNumberToObject(js_list,"clock-frequency",atoi((const char *)argv[i])); //添加 pclk
			
		}
		else if (!strcmp((const char *)argv[i], "pclkactive"))
		{
			cJSON_DeleteItemFromObject(js_list,"clk-active"); //删除 pclkactive
			i++;
			if (atoi((const char *)argv[i]))
				cJSON_AddTrueToObject(js_list,"clk-active"); //添加 pclkactive
			else
				cJSON_AddFalseToObject(js_list,"clk-active"); //添加 pclkactive
		}
		else if (!strcmp((const char *)argv[i], "hsactive"))
		{
			cJSON_DeleteItemFromObject(js_list,"hsync-active"); //删除 hsync-active
			i++;
			if (atoi((const char *)argv[i]))
				cJSON_AddTrueToObject(js_list,"hsync-active"); //添加 hsync-active
			else
				cJSON_AddFalseToObject(js_list,"hsync-active"); //添加hsync-active
		}
		else if (!strcmp((const char *)argv[i], "vsactive"))
		{
			cJSON_DeleteItemFromObject(js_list,"vsync-active"); //删除 vsync-active
			i++;
			if (atoi((const char *)argv[i]))
				cJSON_AddTrueToObject(js_list,"vsync-active"); //添加vsync-active
			else
				cJSON_AddFalseToObject(js_list,"vsync-active"); //添加vsync-active
		}
		else if (!strcmp((const char *)argv[i], "denactive"))
		{
			cJSON_DeleteItemFromObject(js_list,"den-active"); //删除 den-active
			i++;
			if (atoi((const char *)argv[i]))
				cJSON_AddTrueToObject(js_list,"den-active"); //添加 den-active
			else
				cJSON_AddFalseToObject(js_list,"den-active"); //添加 den-active
		}

		else if (!strcmp((const char *)argv[i], "rgbswap"))
		{
			cJSON_DeleteItemFromObject(js_list,"rgb-swap"); //删除 rgb-swap
			i++;
			if (atoi((const char *)argv[i]))
				cJSON_AddTrueToObject(js_list,"rgb-swap"); //添加 rgb-swap
			else
				cJSON_AddFalseToObject(js_list,"rgb-swap"); //添加 rgb-swap
		}

		else if (!strcmp((const char *)argv[i], "remove"))
		{
			struct device_t * fb;
			//const char * name="fb-pl111.0";
			//fb= search_device (  name, DEVICE_TYPE_FRAMEBUFFER);
			fb= search_first_device (   DEVICE_TYPE_FRAMEBUFFER);
			remove_device(fb);		//// 模拟器貌似会挂
			cJSON_Delete(fbjson);
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "bl"))
		{
			fbb =search_first_framebuffer(); //供开启背光用
			i++;
			framebuffer_set_backlight(fbb, atoi(argv[i]));
			return 0;
		}
		else 
		{
			printf(" parameter: %s Undefined \r\n",( char *)argv[i]);	
		    cJSON_Delete(fbjson);
		   	return 0; //return 0 ? or -1 ?
		}

    }
//-----------------------------------------------------------------
    {        
		//out = cJSON_Print(fbjson);   //json转换漂亮格式字符串(带\n \t)
       	//	printf(" %s \n",out);
		out = cJSON_PrintUnformatted(fbjson);  //json 转换回原字符串
       	printf(" %s \r\n",out);
    }  
    cJSON_Delete(fbjson);  //释放内存    
      
//---------------------------------------------------------------------
	char json[600];
	int length;
	memset(json, 0, sizeof(json));
	length = sprintf(json, out);
	//free(out);   //free (out);  //?
	//struct framebuffer_t * fbb;	//供开启背光用
	
	struct device_t * fb; //供remove device用
	//const char * name="fb-pl111.0";
	//fb= search_device (  name, DEVICE_TYPE_FRAMEBUFFER);
	fb= search_first_device (   DEVICE_TYPE_FRAMEBUFFER);
	if(fb) 
	{remove_device(fb);		//// 模拟器貌似会挂
	}
	else printf("------no fb-------\r\n");
	
	probe_device(out, length); //
	printf("length:%d\r\n",length);
	fbb =search_first_framebuffer(); //供开启背光用
	framebuffer_set_backlight(fbb, CONFIG_MAX_BRIGHTNESS);
	return 0;
}

static struct command_t cmd_fbs = {
	.name	= "fbs",
	.desc	= "edit tcon timing",
	.usage	= usage,
	.exec	= do_fbs,
};

static __init void fbs_cmd_init(void)
{
	register_command(&cmd_fbs);
}

static __exit void fbs_cmd_exit(void)
{
	unregister_command(&cmd_fbs);
}

command_initcall(fbs_cmd_init);
command_exitcall(fbs_cmd_exit);
