#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>
#include <wayland-client-protocol.h>

#include "util.h"
#include "ui.h"

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
	//struct wayland_t *ui = data;
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

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
		    uint32_t serial, uint32_t time, uint32_t key,
		    uint32_t state_w){
	//struct wayland_t *ui = data;
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
			  uint32_t serial, uint32_t mods_depressed,
			  uint32_t mods_latched, uint32_t mods_locked,
			  uint32_t group){
	//struct wayland_t *ui = data;
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
	//struct wayland_t *ui = data;
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

struct wayland_t *
init_ui(void) {
	struct wayland_t *ui;

	ui = xzalloc(sizeof *ui);

	ui->display = fail_on_null(wl_display_connect(NULL));

	ui->registry = wl_display_get_registry(ui->display);
	wl_registry_add_listener(ui->registry, &registry_listener, ui);

	wl_display_roundtrip(ui->display);
	
	ui->keyboard = wl_seat_get_keyboard(ui->seat);
	wl_keyboard_add_listener(ui->keyboard, &keyboard_listener, ui);

	ui->pointer = wl_seat_get_pointer(ui->seat);
	wl_pointer_add_listener(ui->pointer, &pointer_listener, ui);

	ui->surface = wl_compositor_create_surface(ui->compositor);
	wl_surface_add_listener(ui->surface, &surface_listener, ui);

	ui->shell_surface = wl_shell_get_shell_surface(ui->shell,
							   ui->surface);

	wl_shell_surface_set_title(ui->shell_surface,"shm surface");
	wl_shell_surface_set_toplevel(ui->shell_surface);

	return ui;
}

void
exit_ui(struct wayland_t *ui){
	if (ui->shell)
		wl_shell_destroy(ui->shell);
	if (ui->shm)
		wl_shm_destroy(ui->shm);
	wl_output_destroy(ui->output);
	wl_seat_destroy(ui->seat);
	wl_compositor_destroy(ui->compositor);
	wl_registry_destroy(ui->registry);
	wl_keyboard_destroy(ui->keyboard);
	wl_pointer_destroy(ui->pointer);
	wl_surface_destroy(ui->surface);
	wl_shell_surface_destroy(ui->shell_surface);

	wl_display_disconnect(ui->display);
	free(ui);
}