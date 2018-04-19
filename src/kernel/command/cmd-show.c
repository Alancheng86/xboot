/*
 * kernel/command/cmd-show.c
 */

#include <command/command.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <framebuffer/framebuffer.h>

#include <cairo-xboot.h>
#include <cairo-ft.h>
#include <shell/ctrlc.h>
#include <input/input.h>
#include <input/keyboard.h>

extern struct render_t * fb_create(struct framebuffer_t * fb);
extern void fb_destroy(struct framebuffer_t * fb, struct render_t * render);

static void usage(void)
{
	printf("show color:\r\n");
	printf("           show r(g,b) : show full screen Red\r\n");
	printf("           show rgb r g b : show full screen color(r+g+b)\r\n");
//	system("PAUSE");
}
struct display_t {
	struct framebuffer_t * fb;
	cairo_surface_t * alone;
	cairo_surface_t * cs[2];
	cairo_t * cr[2];
	int index;
};

static struct display_t * display_alloc(const char * name)
{
	struct display_t * display;
	struct framebuffer_t * fb;

	if(!(fb = name ? search_framebuffer(name) : search_first_framebuffer()))
		return NULL;

	display = malloc(sizeof(struct display_t));
	if(!display)
		return NULL;

	display->fb = fb;
	display->alone = cairo_xboot_surface_create(display->fb, display->fb->alone);
	display->cs[0] = cairo_xboot_surface_create(display->fb, NULL);
	display->cs[1] = cairo_xboot_surface_create(display->fb, NULL);
	display->cr[0] = cairo_create(display->cs[0]);
	display->cr[1] = cairo_create(display->cs[1]);
	display->index = 0;

	return display;
}

static void display_free(struct display_t * display)
{
	if(display)
	{
		cairo_xboot_surface_present(display->alone);
		cairo_surface_destroy(display->alone);
		cairo_destroy(display->cr[0]);
		cairo_destroy(display->cr[1]);
		cairo_surface_destroy(display->cs[0]);
		cairo_surface_destroy(display->cs[1]);
	}
}

static int display_width(struct display_t * display)
{
	if(display)
		return cairo_image_surface_get_width(display->alone);
	return 0;
}

static int display_height(struct display_t * display)
{
	if(display)
		return cairo_image_surface_get_height(display->alone);
	return 0;
}

static cairo_t * display_present(struct display_t * display)
{
	cairo_xboot_surface_present(display->cs[display->index]);
	display->index = (display->index + 1) % 2;
	return display->cr[display->index];
}

static cairo_font_face_t * cairo_font_face_alloc(const char * font)
{
	cairo_font_face_t * cface;
	FT_Library library;
	FT_Face face;

	FT_Init_FreeType(&library);
	FT_New_Face(library, font, 0, &face);
	cface = cairo_ft_font_face_create_for_ft_face(face, 0);
	return cface;
}

static void cairo_show_information(cairo_t * cr, double width, double height, cairo_font_face_t * face, const char * information)
{
	cairo_text_extents_t extents;
	int info_height=32;
	cairo_save(cr);
	cairo_set_line_width(cr, 4);
	cairo_rectangle(cr, 0, height/2, width - 1, info_height);
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_fill_preserve(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_stroke(cr);
	cairo_set_font_face(cr, face);
	cairo_set_font_size(cr, 24);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_text_extents(cr, information, &extents);
	cairo_move_to(cr, (width - extents.width)/2, height/2 + info_height-(info_height-extents.height)/2);
	cairo_show_text(cr, information);
	cairo_restore(cr);
}
static void show_area(int sx,int ex, int sy,int ey,int color)
{	
	struct framebuffer_t * fb;
	struct render_t * render;
	fb =search_first_framebuffer();
	render= fb_create(fb);
	int point=0; int m,n;
	int width=fb->width;
	//int height=fb->height;
	int *pixels=render->pixels;

    for (n=sy;n<=ey;n++)
        { 
            for (m=sx;m<=ex;m++)
                {
                      point = m + n*width;
                      pixels[point] = color;
                }
        }
    fb->present(fb, render);
	fb_destroy(fb, render);
}


static void baibiankuang(void)
{	
	struct framebuffer_t * fb;
	struct render_t * render;
	fb =search_first_framebuffer();
	render= fb_create(fb);
	int point=0; int m,n;
	int width=fb->width;
	int height=fb->height;
	int *pixels=render->pixels;
	int x=render->pixlen;
	memset (pixels,0,render->pixlen); //
	{
  		n=0;
        for (m=0;m<width;m++)
            {
                point = m + n*width;
                pixels[point] = 0xffffffff;
            }
        
       	n=height-1;        
        for (m=0;m<width;m++)
            {
                point = m + n*width;
                pixels[point] = 0xffffffff;
            }
        for (n=0;n<height;n++)
        { 
            m=0;
            {
                point = m + n*width;
                pixels[point] = 0xffffffff;
            }
        }
        for (n=0;n<height;n++)
        { 
            m=width-1;
            {
                point = m + n*width;
                pixels[point] = 0xffffffff;
            }
        }

    }

	fb->present(fb, render);
	fb_destroy(fb, render);
	
}
static int do_show(int argc, char ** argv)
{
   	struct framebuffer_t * fbb;	//供开启背光用
	fbb =search_first_framebuffer(); //供开启背光用
	framebuffer_set_backlight(fbb, CONFIG_MAX_BRIGHTNESS);
	//此函数有BUG，多次执行后，系统会挂，貌似内存泄露
	int i;
	struct display_t * display = display_alloc(NULL);
	cairo_font_face_t * face = cairo_font_face_alloc("/framework/assets/fonts/Roboto-Regular.ttf");
	cairo_t * cr = display_present(display);
	int width = display_width(display);
	int height = display_height(display);

	double r, g, b; r=0;g=0;b=0;

	for(i=1; i<argc;  )   //
	{
		if (!strcmp((const char *)argv[i], "r"))
		{
			if (argc>2)
			{
				r = (double)atoi(argv[i+1])/255;
			}
			else r=1;
			
			cairo_set_source_rgb(cr, r, g, b);
			cairo_paint(cr);
			//cairo_restore(cr);
			cr = display_present(display);
			//cr = display_present(display);
			//cairo_save(cr);
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "g"))
		{
			if (argc>2)
			{
				g = (double)atoi(argv[i+1])/255;
			}
			else g=1;
			
			cairo_set_source_rgb(cr,0, g, 0);
			cairo_paint(cr);
			//cairo_restore(cr);
			cr = display_present(display);
			//cr = display_present(display);
			//cairo_save(cr);
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "b"))
		{	
			if (argc>2)
			{
				b = (double)atoi(argv[i+1])/255;
			}
			else b=1;
			
			cairo_set_source_rgb(cr, 0, 0, b);
			cairo_paint(cr);
			//cairo_restore(cr);
			cr = display_present(display);
			//cr = display_present(display);
			//cairo_save(cr);
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "w"))
		{ 
			if (argc>2)
			{
				r = g=b =(double)atoi(argv[i+1])/255;
			}
			else r=g=b=1;
			
			cairo_set_source_rgb(cr, r, g, b);
			cairo_paint(cr);
			//cairo_restore(cr);
			cr = display_present(display);
			//cr = display_present(display);
			//cairo_save(cr);
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "black"))
		{
			cairo_set_source_rgb(cr, 0, 0, 0);
			cairo_paint(cr);
			//cairo_restore(cr);
			cr = display_present(display);
			//cr = display_present(display);
			//cairo_save(cr);
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "gray"))
		{
			if (argc>2)
			{
				r = g=b =(double)atoi(argv[i+1])/255;
			}
			else r=g=b=0.5;
			
			cairo_set_source_rgb(cr, r, g, b);
			cairo_paint(cr);
			//cairo_restore(cr);
			cr = display_present(display);
			//cr = display_present(display);
			//cairo_save(cr);
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "rgb"))
		{
			if (argc == 5)
			{
				r=(double)atoi(argv[2])/255;
				g=(double)atoi(argv[3])/255;
				b=(double)atoi(argv[4])/255;
				cairo_set_source_rgb(cr, r, g, b);
				cairo_paint(cr);
			}
			else 
			{
				//r=g=b=0.5;
				cairo_show_information(cr, width, height, face, "Invalid parameter,Not enough");
			}
			cr = display_present(display);
			
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "area"))
		{
			if (argc == 7)
			{
				show_area(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6]));
			}
			else 
			{
				//r=g=b=0.5;
				cairo_show_information(cr, width, height, face, "Invalid parameter;Ex: sx,ex,sy,ey,color");
				cr = display_present(display);
			}	
			
			return 0;
		}
		else if (!strcmp((const char *)argv[i], "png"))
		{
			cairo_surface_t * png = cairo_image_surface_create_from_png("/framework/assets/images/show/1.png");
			int cwidth = cairo_image_surface_get_width(png);
			int cheight = cairo_image_surface_get_height(png);
			cairo_set_source_surface(cr, png, 0, 0);
			cairo_paint(cr);
			cairo_surface_destroy(png);
			cr = display_present(display);
			//cr = display_present(display);
			return 0;
		}
		else if(!strcmp((const char *)argv[i], "kuang"))
		{
			baibiankuang();
			return 0;
		}
		else 
		{
			cairo_show_information(cr, width, height, face, "Unknow parameter");
			cr = display_present(display);
			return 0;
		}

    }
    
	//cairo_surface_destroy(png);
	display_free(display);	
	//这里未释放占用的内存，后续需调整
	return 0;
}

static struct command_t cmd_show = {
	.name	= "show",
	.desc	= "show sth. on LCD",
	.usage	= usage,
	.exec	= do_show,
};

static __init void show_cmd_init(void)
{
	register_command(&cmd_show);
}

static __exit void show_cmd_exit(void)
{
	unregister_command(&cmd_show);
}

command_initcall(show_cmd_init);
command_exitcall(show_cmd_exit);
