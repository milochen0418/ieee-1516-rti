#include <stdlib.h>
#include "debug.h"
#include "memory_util.h"

void *Malloc(int size){
	void *buf;

	if (size == 0){
		warning("malloc zero byte\n");
		return NULL;
	}
	buf = (void *)malloc(size);

	return buf;
}

void Memfree(void *buf){
	free(buf);
}

/*
 * vim: ts=4 sts=4 sw=4
 */
