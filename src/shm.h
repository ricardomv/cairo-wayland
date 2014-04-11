struct shm_window {
	cairo_surface_t *cairo_surface;
};

struct shm_window *
create_shm_surface(struct wl_shm *shm,
			   struct rectangle *rectangle);

struct wl_buffer *
get_buffer_from_cairo_surface(cairo_surface_t *surface);

void ui_resize(struct wayland_t *ui, int width, int height);
void ui_redraw(struct wayland_t *ui);