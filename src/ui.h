struct xkb;

struct color { double r, g, b, a; };

struct rectangle {
	int x;
	int y;
	int width;
	int height;
};

struct wayland_t {
	struct wl_display *display;
	struct wl_output *output;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct wl_cursor_theme *cursor_theme;
	struct wl_cursor *default_cursor;
	struct wl_surface *cursor_surface;
	struct wl_shell *shell;
	struct wl_shell_surface *shell_surface;
	struct wl_surface *surface;
	struct wl_seat *seat;
	struct wl_keyboard *keyboard;
	struct wl_pointer *pointer;

	struct wl_data_device_manager *data_device_manager;
	struct wl_data_device *data_device;

	struct egl_ui *egl;

	struct xkb *xkb;

	struct window* window;

	float px, py; /* pointer position */
	int resize;
	struct color bg_color;
	int fullscreen;
	int need_redraw;
};

struct wayland_t *init_ui(void);
void exit_ui(struct wayland_t *ui);
void ui_redraw(struct wayland_t *ui);
