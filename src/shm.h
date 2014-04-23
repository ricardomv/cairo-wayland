struct window *
window_create(struct wayland_t *ui, int width, int height);
void
window_destroy(struct window *window);
void
window_resize(struct window *window, int width, int height);
void
window_redraw(struct window *window);
void
window_get_width_height(struct window *window, int *w, int *h);
