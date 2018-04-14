/*
 * kernel/command/cmd-color.c
 */

#include <cairo-xboot.h>
#include <cairo-ft.h>
#include <shell/ctrlc.h>
#include <input/input.h>
#include <input/keyboard.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    color [args ...]\r\n");
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

static void cairo_show_title(cairo_t * cr, double width, double height, cairo_font_face_t * face, const char * title)
{
	cairo_text_extents_t extents;

	cairo_save(cr);
	cairo_set_line_width(cr, 3);
	cairo_rectangle(cr, 0, 0, width - 1, height - 1);
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_fill_preserve(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_stroke(cr);
	cairo_set_font_face(cr, face);
	cairo_set_font_size(cr, 24);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_text_extents(cr, title, &extents);
	cairo_move_to(cr, (width - extents.width) / 2, (height - extents.height) / 2 + 20);
	cairo_show_text(cr, title);
	cairo_restore(cr);
}

static int do_color(int argc, char ** argv)
{
	struct event_t e;
	struct display_t * display = display_alloc(NULL);
	cairo_font_face_t * face = cairo_font_face_alloc("/framework/assets/fonts/Roboto-Regular.ttf");
	cairo_t * cr = display_present(display);
	int width = display_width(display);
	int height = display_height(display) - 32;
	cairo_surface_t * blue_bar = cairo_image_surface_create_from_png("/framework/assets/images/color/blue-bar.png");
	cairo_surface_t * blue_dot = cairo_image_surface_create_from_png("/framework/assets/images/color/blue-dot.png");
	cairo_surface_t * green_bar = cairo_image_surface_create_from_png("/framework/assets/images/color/green-bar.png");
	cairo_surface_t * green_dot = cairo_image_surface_create_from_png("/framework/assets/images/color/green-dot.png");
	cairo_surface_t * red_bar = cairo_image_surface_create_from_png("/framework/assets/images/color/red-bar.png");
	cairo_surface_t * red_dot = cairo_image_surface_create_from_png("/framework/assets/images/color/red-dot.png");
	int cwidth = cairo_image_surface_get_width(blue_bar);
	int cheight = cairo_image_surface_get_height(blue_bar);
	double r, g, b;
	double c = 1;
	double t;

	while(1)
	{
		if(pump_event(runtime_get()->__event_base, &e))
		{
			if(e.type == EVENT_TYPE_KEY_UP)
			{
				if(e.e.key_up.key == KEY_EXIT)
					break;
			}
		}
		if(ctrlc())
			break;

		c = c + 1.0;
		t = c / 10;
		r = (sin(t * 0.5 + 0.3) + 1) / 2;
		g = (sin(t * 0.8 + 0.2) + 1) / 2;
		b = (sin(t * 1.3 + 0.6) + 1) / 2;

		cairo_save(cr);
		cairo_set_source_rgb(cr, 0.349019608, 0.352941176, 0.82745098);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_set_source_surface(cr, red_bar, 91, 230);
		cairo_paint(cr);
		cairo_set_source_surface(cr, green_bar, 91, 260);
		cairo_paint(cr);
		cairo_set_source_surface(cr, blue_bar, 91, 290);
		cairo_paint(cr);
		cairo_set_source_surface(cr, red_dot, 90 + r * 280, 222);
		cairo_paint(cr);
		cairo_set_source_surface(cr, green_dot, 90 + g * 280, 252);
		cairo_paint(cr);
		cairo_set_source_surface(cr, blue_dot, 90 + b * 280, 282);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_set_line_width(cr, 6);
		cairo_rectangle(cr, 500, 180, 200, 200);
		cairo_set_source_rgb(cr, r, g, b);
		cairo_fill_preserve(cr);
		cairo_set_source_rgb(cr, 1, 1, 1);
		cairo_stroke(cr);
		cairo_restore(cr);

		cairo_show_title(cr, width, 32, face, "颜色测试");
		//cairo_show_title(cr, width, 32, face, "color test");
		cr = display_present(display);
		mdelay(20);
	}

	cairo_surface_destroy(blue_bar);
	cairo_surface_destroy(blue_dot);
	cairo_surface_destroy(green_bar);
	cairo_surface_destroy(green_dot);
	cairo_surface_destroy(red_bar);
	cairo_surface_destroy(red_dot);
	display_free(display);
	return 0;
}

static struct command_t cmd_color = {
	.name	= "color",
	.desc	= "show color diagram",
	.usage	= usage,
	.exec	= do_color,
};

static __init void color_cmd_init(void)
{
	register_command(&cmd_color);
}

static __exit void color_cmd_exit(void)
{
	unregister_command(&cmd_color);
}

command_initcall(color_cmd_init);
command_exitcall(color_cmd_exit);
