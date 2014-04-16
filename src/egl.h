struct egl_ui *
init_egl(struct wayland_t *ui);
void
fini_egl(struct egl_ui *egl);
struct window *
window_create(struct wayland_t *ui, int width, int height);
void
window_destroy(struct window *window);
void
window_resize(struct window *window, int width, int height);
void
window_redraw(struct window *window);
cairo_surface_t *
window_get_cairo_surface(struct window *window);