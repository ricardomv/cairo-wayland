struct rectangle;
struct shm_pool;
struct shm_surface_data;

cairo_surface_t *
display_create_surface(struct wl_shm *shm,
		       struct wl_surface *surface,
		       struct rectangle *rectangle,
		       uint32_t flags);

struct wl_buffer *
display_get_buffer_for_surface(struct wl_display *display,
			       cairo_surface_t *surface);