#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void *
fail_on_null(void *p)
{
	if (p == NULL) {
//		fprintf(stderr, "%s: out of memory\n", program_invocation_short_name);
		exit(EXIT_FAILURE);
	}
	return p;
}

static inline void *
zalloc(size_t size)
{
	return calloc(1, size);
}

void *
xzalloc(size_t s)
{
	return fail_on_null(zalloc(s));
}