#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <wayland-client.h>

#include <cairo/cairo.h>

#include "util.h"
#include "ui.h"

struct app_t {
	struct wayland_t *ui;
};

int running = 1;

static void
signal_int(int signum)
{
	running = 0;
}

int main(int argc, char const *argv[])
{
	struct app_t *term;
	struct sigaction sigint;
	int ret = 0;

	term = xzalloc(sizeof *term);
	term->ui = init_ui();

	sigint.sa_handler = signal_int;
	sigemptyset(&sigint.sa_mask);
	sigint.sa_flags = SA_RESETHAND;
	sigaction(SIGINT, &sigint, NULL);
	
	for(;running && ret != -1;) {
		if (term->ui->need_redraw)
			redraw(term->ui);
		ret = wl_display_dispatch(term->ui->display);
	}

	exit_ui(term->ui);
	free(term);
	return 0;
}
