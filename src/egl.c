#include <wayland-egl.h>
#include <cairo/cairo.h>
#include <cairo/cairo-gl.h>

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#include "ui.h"
#include "draw.h"
#include "egl.h"
#include "util.h"

struct egl_ui {
	EGLDisplay dpy;
	EGLConfig argb_config;
	EGLContext argb_ctx;
	cairo_device_t *argb_device;
};

struct window{
	int width, height;
	struct wayland_t *ui;

	struct wl_egl_window *egl_window;
	EGLSurface egl_surface;
	EGLDisplay dpy;

	cairo_surface_t *cairo_surface;
};

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

void
fini_egl(struct egl_ui *egl){
	cairo_device_destroy(egl->argb_device);
	eglMakeCurrent(egl->dpy, EGL_NO_SURFACE, EGL_NO_SURFACE,
							EGL_NO_CONTEXT);
	eglTerminate(egl->dpy);
	eglReleaseThread();
	free(egl);
}

struct window *
window_create(struct wayland_t *ui, int width, int height){
	struct window *window;
	
	window = xzalloc(sizeof *window);

	window->width = width;
	window->height = height;

	window->ui = ui;
	window->dpy = ui->egl->dpy;

	window->egl_window = wl_egl_window_create(ui->surface,
							window->width,
							window->height);

	window->egl_surface = eglCreateWindowSurface(ui->egl->dpy,
							ui->egl->argb_config,
							window->egl_window,
							NULL);

	window->cairo_surface =
		cairo_gl_surface_create_for_egl(ui->egl->argb_device,
							window->egl_surface,
							window->width,
							window->height);
	return window;
}

void
window_destroy(struct window *window){
	wl_egl_window_destroy(window->egl_window);
	eglDestroySurface(window->dpy,window->egl_surface);
	cairo_surface_destroy(window->cairo_surface);
	free(window);
}

void
window_resize(struct window *window, int width, int height){
	window->width = width;
	window->height = height;
	wl_egl_window_resize(window->egl_window, width, height, 0, 0);
	cairo_gl_surface_set_size(window->cairo_surface,width, height);
}

void
window_redraw(struct window *window){
	paint_surface(window->cairo_surface,window->ui);
	cairo_gl_surface_swapbuffers(window->cairo_surface);
}

void
window_get_width_height(struct window *window, int *w, int *h){
	*w = window->width;
	*h = window->height;
}
