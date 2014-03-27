#include <wayland-egl.h>
#include <cairo/cairo.h>
#include <cairo/cairo-gl.h>

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#include "ui.h"
#include "egl.h"
#include "draw.h"
#include "util.h"

struct egl_ui *
init_egl(struct wayland_t *ui){
	struct egl_ui *egl;
	EGLint major, minor;
	EGLint n;
	EGLint *context_attribs = NULL;

	static const EGLint argb_cfg_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 1,
		EGL_DEPTH_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_NONE
	};


	egl = xzalloc(sizeof *egl);

	egl->dpy = eglGetDisplay(ui->display);
	if (!eglInitialize(egl->dpy, &major, &minor)) {
		fprintf(stderr, "failed to initialize EGL\n");
		return NULL;
	}

	if (!eglBindAPI(EGL_OPENGL_API)) {
		fprintf(stderr, "failed to bind EGL client API\n");
		return NULL;
	}

	if (!eglChooseConfig(egl->dpy, argb_cfg_attribs,
			     &egl->argb_config, 1, &n) || n != 1) {
		fprintf(stderr, "failed to choose argb EGL config\n");
		return NULL;
	}
	egl->argb_ctx = eglCreateContext(egl->dpy, egl->argb_config,
				       EGL_NO_CONTEXT, context_attribs);
	if (!egl->argb_ctx) {
		fprintf(stderr, "failed to create EGL context\n");
		return NULL;
	}
	egl->argb_device = cairo_egl_device_create(egl->dpy, egl->argb_ctx);
	if (cairo_device_status(egl->argb_device) != CAIRO_STATUS_SUCCESS) {
		fprintf(stderr, "failed to get cairo EGL argb device\n");
		return NULL;
	}
	return egl;
}

struct egl_window *
create_egl_surface(struct wayland_t *ui,
			   struct rectangle *rectangle){
	struct egl_window *surface;
	
	surface = xzalloc(sizeof *surface);

	surface->display = ui->display;
	surface->surface = ui->surface;
	surface->egl_window = wl_egl_window_create(surface->surface,
							rectangle->width,
							rectangle->height);


	surface->egl_surface = eglCreateWindowSurface(ui->egl->dpy,
							ui->egl->argb_config,
							surface->egl_window,
							NULL);

	surface->cairo_surface =
		cairo_gl_surface_create_for_egl(ui->egl->argb_device,
							surface->egl_surface,
							rectangle->width,
							rectangle->height);
	return surface;
}

void
ui_resize(struct wayland_t *ui, int edges, int width, int height){
	wl_egl_window_resize(ui->egl_surface->egl_window, width, height, 0, 0);
	cairo_gl_surface_set_size(ui->egl_surface->cairo_surface,width, height);
}

void
ui_redraw(struct wayland_t *ui){
	draw_window(ui,ui->egl_surface->cairo_surface);
	cairo_gl_surface_swapbuffers(ui->egl_surface->cairo_surface);
	ui->need_redraw = 0;
}
