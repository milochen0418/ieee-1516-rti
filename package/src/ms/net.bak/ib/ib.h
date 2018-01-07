#ifndef __IB_H
#define __IB_H

#include "net-types.h"

struct server_addr_t
{
	char *hostname;
	int port;
	int socket;
	int is_initialized;
};

int setup_ib_module(void);

#endif
/*
 * vim: ts=4 sts=4 sw=4
 */
