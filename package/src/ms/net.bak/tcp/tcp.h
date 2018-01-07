#ifndef __TCP_H
#define __TCP_H

#include "net-types.h"

struct server_addr_t
{
	char *hostname;
	int port;
	int socket;
	int is_initialized;
};

int setup_tcp_module(void);

#endif
/*
 * vim: ts=4 sts=4 sw=4
 */
