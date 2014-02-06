#include <stdlib.h>
#include "ui.h"
#include "util.h"

struct app_t {
	struct wayland_t *ui;
};

int main(int argc, char const *argv[])
{
	struct app_t *term;

	term = xzalloc(sizeof *term);
	term->ui = init_ui();

	exit_ui(term->ui);
	free(term);
	return 0;
}