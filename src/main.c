#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <wayland-client.h>

#include <cairo/cairo.h>

#include "util.h"
#include "shm.h"
#include "ui.h"

struct app_t {
	struct wayland_t *ui;
};
int aux = 0;

static const struct wl_callback_listener frame_listener;

static void
redraw(void *data, struct wl_callback *callback, uint32_t time){
	struct wayland_t *ui = data;
	cairo_t *cr;

	cr = cairo_create(ui->cairo_surface);

		cairo_set_source_rgba(cr, 1, 0.5, 1, 0.5);
		cairo_paint(cr);


	
	wl_surface_attach(ui->surface,display_get_buffer_for_surface(ui->display,ui->cairo_surface),0,0);
		/* repaint all the pixels in the surface, change size to only repaint changed area*/
	wl_surface_damage(ui->surface, ui->window_rectangle->x, 
					ui->window_rectangle->y, 
					ui->window_rectangle->width, 
					ui->window_rectangle->height);

	//ui->callback = wl_surface_frame(ui->surface);
	//wl_callback_add_listener(ui->callback, &frame_listener, ui);

	wl_surface_commit(ui->surface);
	cairo_destroy(cr);
	aux++;
}

static const struct wl_callback_listener frame_listener = {
	redraw
};

int main(int argc, char const *argv[])
{
	struct app_t *term;

	term = xzalloc(sizeof *term);
	term->ui = init_ui();

	term->ui->window_rectangle = xzalloc(sizeof *term->ui->window_rectangle);

	term->ui->window_rectangle->x = 0;
	term->ui->window_rectangle->y = 0;
	term->ui->window_rectangle->width = 200;
	term->ui->window_rectangle->height = 200;

	term->ui->cairo_surface = display_create_surface(term->ui->shm, term->ui->surface, term->ui->window_rectangle,2);
	

	redraw(term->ui, NULL, 0);

	for(;aux < 10;) {
		wl_display_dispatch(term->ui->display);
	}

	exit_ui(term->ui);
	free(term);
	return 0;
}