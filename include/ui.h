struct rectangle {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
};

struct wayland_t {
	struct wl_display *display;
	struct wl_output *output;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct wl_seat *seat;
	struct wl_keyboard *keyboard;
	struct wl_pointer *pointer;
	struct wl_shell *shell;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	struct wl_callback *callback;
	struct rectangle *window_rectangle;

	cairo_surface_t *cairo_surface;
};


struct wayland_t *init_ui(void);
void exit_ui(struct wayland_t *ui);