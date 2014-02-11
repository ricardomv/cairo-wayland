#include <stdlib.h>
#include <stdio.h>
#include <cairo/cairo.h>

#include "ui.h"

int ndraw = 0;

void
draw_window(struct wayland_t *ui,cairo_surface_t *surface){
	cairo_t *cr;
	char str[10];
	cr = cairo_create(surface);

	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_source_rgba(cr,ui->color_scheme->bg_color->r,
							ui->color_scheme->bg_color->g,
							ui->color_scheme->bg_color->b,
							ui->color_scheme->bg_color->a);
	cairo_paint(cr);
	cairo_select_font_face(cr, ui->font->family,
							ui->font->slant,
							ui->font->weight);

	cairo_set_font_size(cr, ui->font->size);
	cairo_set_source_rgba(cr,ui->color_scheme->font_color->r,
							ui->color_scheme->font_color->g,
							ui->color_scheme->font_color->b,
							ui->color_scheme->font_color->a);
	cairo_move_to(cr,0,20);
	cairo_show_text(cr,"Terminal");
	cairo_move_to(cr,0,40);
	cairo_show_text(cr,ui->buffer);
	cairo_move_to(cr,0,60);
	sprintf(str,"%d",++ndraw);
	cairo_show_text(cr,str);
	cairo_set_antialias(cr,CAIRO_ANTIALIAS_FAST);

	cairo_destroy(cr);
}