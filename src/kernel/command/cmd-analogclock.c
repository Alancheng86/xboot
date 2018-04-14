/*
 * kernel/command/cmd-analogclock.c
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
	printf("    analogclock [args ...]\r\n");
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

static int do_analogclock(int argc, char ** argv)
{
	struct event_t e;
	struct display_t * display = display_alloc(NULL);
	cairo_font_face_t * face = cairo_font_face_alloc("/framework/assets/fonts/Roboto-Regular.ttf");
	cairo_t * cr = display_present(display);
	int width = display_width(display);
	int height = display_height(display) - 32;
	cairo_surface_t * clock_face = cairo_image_surface_create_from_png("/framework/assets/images/analogclock/clock-face.png");
	cairo_surface_t * clock_hour = cairo_image_surface_create_from_png("/framework/assets/images/analogclock/clock-hour-hand.png");
	cairo_surface_t * clock_minute = cairo_image_surface_create_from_png("/framework/assets/images/analogclock/clock-minute-hand.png");
	cairo_surface_t * clock_second = cairo_image_surface_create_from_png("/framework/assets/images/analogclock/clock-second-hand.png");
	cairo_surface_t * clock_oval = cairo_image_surface_create_from_png("/framework/assets/images/analogclock/clock-oval.png");
	int cwidth = cairo_image_surface_get_width(clock_face);
	int cheight = cairo_image_surface_get_height(clock_face);
	cairo_matrix_t m;
	time_t rawtime;
	struct tm * tm;

	while(1)
	{
		time(&rawtime);
		tm = localtime(&rawtime);

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

		cairo_save(cr);
		cairo_set_source_rgb(cr, 0.823529412, 0.933333333, 0.984313725);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_set_source_surface(cr, clock_face, (width - cwidth) / 2, 32 + (height - cheight) / 2);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_matrix_init_identity(&m);
		cairo_matrix_translate(&m, width / 2, 32 + height / 2);
		cairo_matrix_rotate(&m, tm->tm_hour * 2 * M_PI / 12 + tm->tm_min * 2 * M_PI / (12 * 60) + tm->tm_sec * 2 * M_PI / (12 * 60 * 60));
		cairo_matrix_translate(&m, -0.5 * 1.0 * cairo_image_surface_get_width(clock_hour), -1.0 * 1.0 * cairo_image_surface_get_height(clock_hour));
		cairo_matrix_scale(&m, 1.0, 1.0);
		cairo_set_matrix(cr, &m);
		cairo_set_source_surface(cr, clock_hour, 0, 0);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_matrix_init_identity(&m);
		cairo_matrix_translate(&m, width / 2, 32 + height / 2);
		cairo_matrix_rotate(&m, tm->tm_min * 2 * M_PI / 60 + tm->tm_sec * 2 * M_PI / (60 * 60));
		cairo_matrix_translate(&m, -0.5 * 1.0 * cairo_image_surface_get_width(clock_minute), -1.0 * 1.0 * cairo_image_surface_get_height(clock_minute));
		cairo_matrix_scale(&m, 1.0, 1.0);
		cairo_set_matrix(cr, &m);
		cairo_set_source_surface(cr, clock_minute, 0, 0);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_matrix_init_identity(&m);
		cairo_matrix_translate(&m, width / 2, 32 + height / 2);
		cairo_matrix_rotate(&m, tm->tm_sec * 2 * M_PI / 60);
		cairo_matrix_translate(&m, -0.5 * 1.0 * cairo_image_surface_get_width(clock_second), -0.766839378 * 1.0 * cairo_image_surface_get_height(clock_second));
		cairo_matrix_scale(&m, 1.0, 1.0);
		cairo_set_matrix(cr, &m);
		cairo_set_source_surface(cr, clock_second, 0, 0);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_matrix_init_identity(&m);
		cairo_matrix_translate(&m, cwidth / 2, cheight / 2);
		cairo_matrix_rotate(&m, 0 * M_PI / 30);
		cairo_matrix_translate(&m, -0.5 * 1.0 * cairo_image_surface_get_width(clock_oval), -0.5 * 1.0 * cairo_image_surface_get_height(clock_oval));
		cairo_matrix_scale(&m, 1.0, 1.0);
		cairo_set_matrix(cr, &m);
		cairo_set_source_surface(cr, clock_oval, (width - cwidth) / 2, 32 + (height - cheight) / 2);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_show_title(cr, width, 32, face, "模拟时钟");
		//cairo_show_title(cr, width, 30, face, "时钟");
		//cairo_show_title(cr, width, 32, face, "analogclock时钟");
		cr = display_present(display);
		mdelay(20);
	}

	cairo_surface_destroy(clock_face);
	cairo_surface_destroy(clock_hour);
	cairo_surface_destroy(clock_minute);
	cairo_surface_destroy(clock_second);
	cairo_surface_destroy(clock_oval);
	display_free(display);
	return 0;
}

static struct command_t cmd_analogclock = {
	.name	= "analogclock",
	.desc	= "show analogclock diagram",
	.usage	= usage,
	.exec	= do_analogclock,
};

static __init void analogclock_cmd_init(void)
{
	register_command(&cmd_analogclock);
}

static __exit void analogclock_cmd_exit(void)
{
	unregister_command(&cmd_analogclock);
}

command_initcall(analogclock_cmd_init);
command_exitcall(analogclock_cmd_exit);
