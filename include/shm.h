struct rectangle;
struct shm_pool;
struct shm_surface_data;

cairo_surface_t *
create_shm_surface(struct wl_shm *shm,
			   struct rectangle *rectangle, uint32_t flags);

struct wl_buffer *
get_buffer_from_cairo_surface(cairo_surface_t *surface);