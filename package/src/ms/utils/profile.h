#ifndef __PROFILE_H
#define __PROFILE_H

#include "list.h"

struct profile_t {
	list_t list;
	char *name;
	void *data;
};

#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
