#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <wayland-client.h>

#include <cairo/cairo.h>

#include "util.h"
#include "shm.h"
#include "ui.h"

struct app_t {
	struct wayland_t *ui;
};

int running = 1;

static void
signal_int(int signum)
{
	running = 0;
}

void
set_random_color(cairo_t *cr)
{
	cairo_set_source_rgba(cr,
			      0.5 + (random() % 50) / 49.0,
			      0.5 + (random() % 50) / 49.0,
			      0.5 + (random() % 50) / 49.0,
			      0.5 + (random() % 100) / 99.0);
}

void
color_test(struct wayland_t *ui,cairo_surface_t *surface){
	cairo_t *cr;
	cr = cairo_create(surface);


	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	set_random_color(cr);
	cairo_paint(cr);
	cairo_select_font_face(cr, "Terminus",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 15);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_move_to(cr,0,20);
	cairo_show_text(cr,ui->buffer);
	cairo_set_antialias(cr,CAIRO_ANTIALIAS_FAST);

	cairo_destroy(cr);
}

static const struct wl_callback_listener frame_listener;

static void
redraw(void *data, struct wl_callback *callback, uint32_t time){
	struct wayland_t *ui = data;

	color_test(ui,ui->cairo_surface);
	
	wl_surface_attach(ui->surface,display_get_buffer_for_surface(ui->display,ui->cairo_surface),0,0);
		/* repaint all the pixels in the surface, change size to only repaint changed area*/
	wl_surface_damage(ui->surface, ui->window_rectangle->x, 
					ui->window_rectangle->y, 
					ui->window_rectangle->width, 
					ui->window_rectangle->height);

	ui->callback = wl_surface_frame(ui->surface);
	wl_callback_add_listener(ui->callback, &frame_listener, ui);

	wl_surface_commit(ui->surface);
}

static const struct wl_callback_listener frame_listener = {
	redraw
};

int main(int argc, char const *argv[])
{
	struct app_t *term;
	struct sigaction sigint;
	int ret = 0;

	term = xzalloc(sizeof *term);
	term->ui = init_ui();

	term->ui->window_rectangle = xzalloc(sizeof *term->ui->window_rectangle);

	term->ui->window_rectangle->x = 0;
	term->ui->window_rectangle->y = 0;
	term->ui->window_rectangle->width = 200;
	term->ui->window_rectangle->height = 200;

	term->ui->cairo_surface = display_create_surface(term->ui->shm, term->ui->surface, term->ui->window_rectangle,2);
	

	redraw(term->ui, NULL, 0);

	sigint.sa_handler = signal_int;
	sigemptyset(&sigint.sa_mask);
	sigint.sa_flags = SA_RESETHAND;
	sigaction(SIGINT, &sigint, NULL);
	
	for(;running && ret != -1;) {
		term->ui->window_rectangle->width += 1;
		ret = wl_display_dispatch(term->ui->display);
	}

	exit_ui(term->ui);
	free(term);
	return 0;
}
