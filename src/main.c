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

struct rectangle {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
};

void
draw(struct wayland_t *ui){
	cairo_surface_t *surface;
	cairo_t *cr;
	struct rectangle *rectangle;
	float color;

	rectangle = xzalloc(sizeof *rectangle);

	rectangle->x = 0;
	rectangle->y = 0;
	rectangle->width = 200;
	rectangle->height = 200;

	surface = display_create_surface(ui->shm, ui->surface, rectangle,2);

	cr = cairo_create(surface);

	for(color = 0;color<10; color++){
		cairo_set_source_rgba(cr, 0, color/10, 0, 0.5);
		cairo_paint(cr);
	
		wl_surface_attach(ui->surface,display_get_buffer_for_surface(ui->display,surface),0,0);
		/* repaint all the pixels in the surface, change size to only repaint changed area*/
		wl_surface_damage(ui->surface, rectangle->x, 
					rectangle->y, 
					rectangle->width, 
					rectangle->height);
		wl_surface_commit(ui->surface);
		wl_display_flush(ui->display);
		
		sleep(1);
	}

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}


int main(int argc, char const *argv[])
{
	struct app_t *term;

	term = xzalloc(sizeof *term);
	term->ui = init_ui();

	draw(term->ui);

	exit_ui(term->ui);
	free(term);
	return 0;
}