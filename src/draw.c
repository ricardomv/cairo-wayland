#include <cairo/cairo.h>
#include "ui.h"

int nredraw = 0;

void
paint_surface(cairo_surface_t *cairo_surface, struct wayland_t *ui){
	cairo_t *cr;
	cr = cairo_create(cairo_surface);

	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_source_rgba(cr,ui->bg_color.r,
							ui->bg_color.g,
							ui->bg_color.b,
							ui->bg_color.a);
	cairo_paint(cr);

	cairo_set_source_rgba(cr,0,1,0,1);
	cairo_rectangle (cr, 0, 0, nredraw, 3);
	if (nredraw++ > 400)
		nredraw = 0;
	cairo_fill(cr);
	cairo_destroy(cr);
}
