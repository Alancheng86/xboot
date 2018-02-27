/*
 * kernel/command/cmd-taichi.c
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
	printf("    taichi [args ...]\r\n");
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

static cairo_t * cairo_shape_taichi(double size)
{
	double cx = size / 2;
	double cy = size / 2;
	double r =  size / 2 - 4;
	cairo_rectangle_t extents = {
		.x = 0,
		.y = 0,
		.width = size,
		.height = size,
	};
	cairo_surface_t * surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &extents);
	cairo_t * cr = cairo_create(surface);

	cairo_save(cr);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST);

	cairo_set_line_width(cr, 2.0);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, cx, cy, r, 0, 2 * M_PI);
	cairo_stroke(cr);

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, cx, cy, r, M_PI / 2, 1.5 * M_PI);
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, cx, cy, r, -M_PI / 2, M_PI / 2);
	cairo_fill(cr);

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, cx, cy - r / 2, r / 2, -M_PI / 2, M_PI / 2);
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, cx, cy + r / 2, r / 2, M_PI / 2, 1.5 * M_PI);
	cairo_fill(cr);

	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, cx, cy - r / 2, r / 6, 0, 2 * M_PI);
	cairo_fill(cr);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_arc(cr, cx, cy + r / 2, r / 6, 0, 2 * M_PI);
	cairo_fill(cr);

	cairo_restore(cr);
	return cr;
}

static void cairo_shape_destroy(cairo_t * shape)
{
	cairo_surface_destroy(cairo_get_target(shape));
	cairo_destroy(shape);
}

static double cairo_shape_width(cairo_t * shape)
{
	cairo_rectangle_t extents;
	cairo_recording_surface_get_extents(cairo_get_target(shape), &extents);
	return extents.width;
}

static double cairo_shape_height(cairo_t * shape)
{
	cairo_rectangle_t extents;
	cairo_recording_surface_get_extents(cairo_get_target(shape), &extents);
	return extents.height;
}

static void cairo_shape_paint(cairo_t * cr, cairo_t * shape, cairo_matrix_t * m, double alpha)
{
	cairo_save(cr);
	cairo_set_matrix(cr, m);
	cairo_surface_t * surface = cairo_surface_reference(cairo_get_target(shape));
	cairo_set_source_surface(cr, surface, 0, 0);
	cairo_surface_destroy(surface);
	cairo_paint_with_alpha(cr, alpha);
	cairo_restore(cr);
}

static int do_taichi(int argc, char ** argv)
{
	struct event_t e;
	struct display_t * display = display_alloc(NULL);
	cairo_font_face_t * face = cairo_font_face_alloc("/framework/assets/fonts/Roboto-Regular.ttf");
	cairo_t * cr = display_present(display);
	int width = display_width(display);
	int height = display_height(display) - 32;
	cairo_t * shape = cairo_shape_taichi((width < height ? width : height) * 9.0 / 10.0);
	cairo_matrix_t m;
	double angle = 0;

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

		cairo_save(cr);
		cairo_set_source_rgb(cr, 0.639215686, 0.894117647, 0.529411765);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_matrix_init_identity(&m);
		cairo_matrix_translate(&m, width / 2, 32 + height / 2);
		cairo_matrix_rotate(&m, (angle++) * (M_PI / 180));
		cairo_matrix_translate(&m, -0.5 * 1.0 * cairo_shape_width(shape), -0.5 * 1.0 * cairo_shape_height(shape));
		cairo_matrix_scale(&m, 1.0, 1.0);
		cairo_shape_paint(cr, shape, &m, 1.0);
		cairo_restore(cr);

		cairo_show_title(cr, width, 32, face, "太极八卦图");
		cr = display_present(display);
		mdelay(1);
	}

	cairo_shape_destroy(shape);
	display_free(display);
	return 0;
}

static struct command_t cmd_taichi = {
	.name	= "taichi",
	.desc	= "show taichi diagram",
	.usage	= usage,
	.exec	= do_taichi,
};

static __init void taichi_cmd_init(void)
{
	register_command(&cmd_taichi);
}

static __exit void taichi_cmd_exit(void)
{
	unregister_command(&cmd_taichi);
}

command_initcall(taichi_cmd_init);
command_exitcall(taichi_cmd_exit);
