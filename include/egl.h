#include <wayland-egl.h>
#include <cairo/cairo-gl.h>

struct egl_window {
	cairo_surface_t *cairo_surface;
	struct wl_display *display;
	struct wl_surface *surface;
	struct wl_egl_window *egl_window;
	EGLSurface egl_surface;
};
struct egl_ui {
	EGLDisplay dpy;
	EGLConfig argb_config;
	EGLContext argb_ctx;
	cairo_device_t *argb_device;
};

struct egl_ui *
init_egl(struct wayland_t *ui);
struct egl_window *
create_egl_surface(struct wayland_t *ui, struct rectangle *rectangle, uint32_t flags);
void ui_resize(struct wayland_t *ui, int edges, int width, int height);
void ui_redraw(struct wayland_t *ui);