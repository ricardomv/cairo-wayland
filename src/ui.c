#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <wayland-client.h>
#include <wayland-client-protocol.h>

#include <cairo/cairo.h>

#include <xkbcommon/xkbcommon.h>

#include "config.h"

#include "util.h"
#include "ui.h"
#include "draw.h"

#ifdef HAVE_EGL
	#include "egl.h"
#else
	#include "shm.h"
#endif

#define MOD_MASK_ANY	UINT_MAX
#define MOD_MASK_NONE	0
#define MOD_MASK_CTRL	(1<<0)
#define MOD_MASK_ALT	(1<<1)
#define MOD_MASK_SHIFT	(1<<2)
#define MOD_MASK_LOGO	(1<<3)

struct xkb{
	struct xkb_context *ctx;
	struct xkb_keymap *keymap;
	struct xkb_state *state;
	xkb_mod_index_t ctrl, alt, shift, logo;
	unsigned int mods;
};

void
registry_handle_global(void *data, struct wl_registry *registry, uint32_t name,
	  const char *interface, uint32_t version) {
	struct wayland_t *ui = data;
	if(strcmp(interface, "wl_compositor") == 0) {
		ui->compositor = wl_registry_bind(registry, name,
					  &wl_compositor_interface, 1);
	} else if(strcmp(interface, "wl_shell") == 0) {
		ui->shell = wl_registry_bind(registry, name,
					    &wl_shell_interface, 1);
	} else if(strcmp(interface, "wl_shm") == 0) {
		ui->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	} else if(strcmp(interface, "wl_seat") == 0) {
		ui->seat = wl_registry_bind(registry, name,
					   &wl_seat_interface, 1);
	 } else if(strcmp(interface, "wl_output") == 0) {
	 	ui->output = wl_registry_bind(registry, name, &wl_output_interface, 1);
	 } else if (strcmp(interface, "wl_data_device_manager") == 0) {
		ui->data_device_manager = wl_registry_bind(registry, name,
					 &wl_data_device_manager_interface, 1);
	}
}

void
registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {
}

static const struct wl_registry_listener registry_listener = {
	registry_handle_global,
	registry_handle_global_remove
};

static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
		       uint32_t format, int fd, uint32_t size){
	struct wayland_t *ui = data;
	char *string;
	if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
		close(fd);
		return;
	}

	string = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (string == MAP_FAILED) {
		close(fd);
		return;
	}

	ui->xkb->keymap = xkb_keymap_new_from_string(ui->xkb->ctx, string,
						   XKB_KEYMAP_FORMAT_TEXT_V1, 0);
	munmap(string, size);
	close(fd);

	ui->xkb->state = xkb_state_new(ui->xkb->keymap);

	ui->xkb->ctrl = xkb_keymap_mod_get_index(ui->xkb->keymap, XKB_MOD_NAME_CTRL);
	ui->xkb->alt = xkb_keymap_mod_get_index(ui->xkb->keymap, XKB_MOD_NAME_ALT);
	ui->xkb->shift = xkb_keymap_mod_get_index(ui->xkb->keymap, XKB_MOD_NAME_SHIFT);
	ui->xkb->logo = xkb_keymap_mod_get_index(ui->xkb->keymap, XKB_MOD_NAME_LOGO);
}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface,
		      struct wl_array *keys){
	//struct wayland_t *ui = data;
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface){
	//struct wayland_t *ui = data;
}

void 
ui_toggle_fullscreen(struct wayland_t *ui){
	ui->fullscreen = !ui->fullscreen;
	if (ui->fullscreen){
		wl_shell_surface_set_fullscreen(ui->shell_surface,
							WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT,
							0, NULL);
	}else{
		wl_shell_surface_set_toplevel(ui->shell_surface);
		window_resize(ui->window, 400, 400);
		ui->need_redraw = 1;
	}
}

extern int running;

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
		    uint32_t serial, uint32_t time, uint32_t key,
		    uint32_t state_w){
	struct wayland_t *ui = data;
	xkb_keysym_t ksym;
	char buf[32];
	size_t len;

	ksym = xkb_state_key_get_one_sym(ui->xkb->state, key + 8);
	len = xkb_keysym_to_utf8(ksym, buf, sizeof buf);
	if (len > 0)
	    len--;

	if (state_w == WL_KEYBOARD_KEY_STATE_PRESSED && len != 0) {
		if (buf[0] == 'q')
			running = 0;
		if (buf[0] == 'f'){
			ui_toggle_fullscreen(ui);
			return;
		}
		if (buf[0] == 'r'){
			window_resize(ui->window, 400, 400);
		}
		ui->need_redraw = 1;
	}
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
			  uint32_t serial, uint32_t mods_depressed,
			  uint32_t mods_latched, uint32_t mods_locked,
			  uint32_t group){
	struct wayland_t *ui = data;
	xkb_mod_mask_t mod_mask;

	xkb_state_update_mask(ui->xkb->state, mods_depressed, mods_latched, mods_locked, group, 0, 0);

	mod_mask = xkb_state_serialize_mods(ui->xkb->state, XKB_STATE_MODS_EFFECTIVE);
	ui->xkb->mods = MOD_MASK_NONE;

	if (mod_mask & (1 << ui->xkb->ctrl))
		ui->xkb->mods |= MOD_MASK_CTRL;
	if (mod_mask & (1 << ui->xkb->alt))
		ui->xkb->mods |= MOD_MASK_ALT;
	if (mod_mask & (1 << ui->xkb->shift))
		ui->xkb->mods |= MOD_MASK_SHIFT;
	if (mod_mask & (1 << ui->xkb->logo))
		ui->xkb->mods |= MOD_MASK_LOGO;
}

static const struct wl_keyboard_listener keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};

static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface,
		     wl_fixed_t sx_w, wl_fixed_t sy_w){
	//struct wayland_t *ui = data;
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface){
	//struct wayland_t *ui = data;
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
		      uint32_t time, wl_fixed_t sx_w, wl_fixed_t sy_w){
	struct wayland_t *ui = data;
	//float sx = wl_fixed_to_double(sx_w);
	//float sy = wl_fixed_to_double(sy_w);
	ui->need_redraw = 1;
}

static void
pointer_handle_button(void *data, struct wl_pointer *pointer, uint32_t serial,
		      uint32_t time, uint32_t button, uint32_t state_w){
	//struct wayland_t *ui = data;	
}

static void
pointer_handle_axis(void *data, struct wl_pointer *pointer,
		    uint32_t time, uint32_t axis, wl_fixed_t value){
	//struct wayland_t *ui = data;
}

static const struct wl_pointer_listener pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

static void
surface_enter(void *data,
	      struct wl_surface *wl_surface, struct wl_output *wl_output){
	//struct wayland_t *ui = data;
}

static void
surface_leave(void *data,
	      struct wl_surface *wl_surface, struct wl_output *output){
	//struct wayland_t *ui = data;
}

static const struct wl_surface_listener surface_listener = {
	surface_enter,
	surface_leave
};

static void
handle_ping(void *data, struct wl_shell_surface *shell_surface,
							uint32_t serial)
{
	wl_shell_surface_pong(shell_surface, serial);
}

static void
handle_configure(void *data, struct wl_shell_surface *shell_surface,
		 uint32_t edges, int32_t width, int32_t height)
{
	struct wayland_t *ui = data;
	window_resize(ui->window,width,height);
	ui->need_redraw = 1;
}

static void
handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener = {
	handle_ping,
	handle_configure,
	handle_popup_done
};

static void
data_offer_offer(void *data, struct wl_data_offer *wl_data_offer, const char *type)
{
	//struct wayland_t *ui = data;
	printf("%s\n", type);
}

static const struct wl_data_offer_listener data_offer_listener = {
	data_offer_offer,
};

static void
data_device_data_offer(void *data,
		       struct wl_data_device *data_device,
		       struct wl_data_offer *_offer){
	struct wayland_t *ui = data;
	wl_data_offer_add_listener(_offer,
				   &data_offer_listener, ui);
}

static void
data_device_enter(void *data, struct wl_data_device *data_device,
		  uint32_t serial, struct wl_surface *surface,
		  wl_fixed_t x_w, wl_fixed_t y_w,
		  struct wl_data_offer *offer)
{
}

static void
data_device_leave(void *data, struct wl_data_device *data_device)
{
}

static void
data_device_motion(void *data, struct wl_data_device *data_device,
		   uint32_t time, wl_fixed_t x_w, wl_fixed_t y_w)
{
}

static void
data_device_drop(void *data, struct wl_data_device *data_device)
{
}

static void
data_device_selection(void *data,
		      struct wl_data_device *wl_data_device,
		      struct wl_data_offer *offer)
{
}

static const struct wl_data_device_listener data_device_listener = {
	data_device_data_offer,
	data_device_enter,
	data_device_leave,
	data_device_motion,
	data_device_drop,
	data_device_selection
};

struct wayland_t *
init_ui(void) {
	struct wayland_t *ui;

	ui = xzalloc(sizeof *ui);
	ui->bg_color.r = 0;
	ui->bg_color.g = 0;
	ui->bg_color.b = 0;
	ui->bg_color.a = 0.8;

	ui->display = fail_on_null(wl_display_connect(NULL));

	ui->registry = wl_display_get_registry(ui->display);
	wl_registry_add_listener(ui->registry, &registry_listener, ui);

	wl_display_roundtrip(ui->display);
	
	ui->keyboard = wl_seat_get_keyboard(ui->seat);
	wl_keyboard_add_listener(ui->keyboard, &keyboard_listener, ui);

	ui->xkb = xzalloc(sizeof *ui->xkb);
	ui->xkb->ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

	ui->pointer = wl_seat_get_pointer(ui->seat);
	wl_pointer_add_listener(ui->pointer, &pointer_listener, ui);

	ui->surface = wl_compositor_create_surface(ui->compositor);
	wl_surface_add_listener(ui->surface, &surface_listener, ui);

	ui->shell_surface = wl_shell_get_shell_surface(ui->shell,
							   ui->surface);
	wl_shell_surface_add_listener(ui->shell_surface, &shell_surface_listener, ui);

	wl_shell_surface_set_title(ui->shell_surface,"window title");
	wl_shell_surface_set_toplevel(ui->shell_surface);

	ui->data_device = wl_data_device_manager_get_data_device(ui->data_device_manager, ui->seat);
	wl_data_device_add_listener(ui->data_device, &data_device_listener,
				    ui);

#ifdef HAVE_EGL
	ui->egl = init_egl(ui);
	if (!ui->egl){
		printf("Error: initializing egl\n");
		return NULL;
	}
#endif

	ui->window = window_create(ui, 400, 300);

	ui->need_redraw = 1;
	return ui;
}

void
exit_ui(struct wayland_t *ui){
	window_destroy(ui->window);

#ifdef HAVE_EGL
	fini_egl(ui->egl);
#endif

	if (ui->shell)
		wl_shell_destroy(ui->shell);
	if (ui->shm)
		wl_shm_destroy(ui->shm);
	wl_output_destroy(ui->output);
	wl_seat_destroy(ui->seat);
	wl_compositor_destroy(ui->compositor);
	wl_registry_destroy(ui->registry);
	wl_keyboard_destroy(ui->keyboard);
	xkb_state_unref(ui->xkb->state);
	xkb_keymap_unref(ui->xkb->keymap);
	xkb_context_unref(ui->xkb->ctx);
	free(ui->xkb);
	wl_pointer_destroy(ui->pointer);
	wl_data_device_manager_destroy(ui->data_device_manager);
	wl_surface_destroy(ui->surface);
	wl_shell_surface_destroy(ui->shell_surface);

	wl_display_disconnect(ui->display);
	free(ui);
}

void
ui_redraw(struct wayland_t *ui){
	if(ui->need_redraw){
		paint_surface(window_get_cairo_surface(ui->window),ui);
		window_redraw(ui->window);
		ui->need_redraw = 0;
	}
}
