char rcsid_zalloc[] = "$Id$";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "b.h"

int
fatal(char *name, int line)
{
	fprintf(stderr, "assertion failed: file %s, line %d\n", name, line);
	exit(1);
	return 0;
}

void *
zalloc(size_t size)
{
	void *t = (void *) malloc(size);
	if (!t) {
		fprintf(stderr, "Malloc failed---PROGRAM ABORTED\n");
		exit(1);
	}
	memset(t, 0, size);
	return t;
}

void
zfree(void *p)
{
	free(p);
}
