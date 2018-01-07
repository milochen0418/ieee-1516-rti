#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "memory_util.h"
#include "net.h"

#ifdef __NET_TCP__
#include "tcp.h"
extern struct network_fns tcp_fns;
#endif
#ifdef __NET_IB__
#include "ib.h"
extern struct network_fns ib_fns;
#endif

static int network_systems_initialized = 0;

struct network_fns *setup_network_systems(void)
{
	struct network_fns *fns = NULL;
	
#ifdef __NET_TCP__
	setup_tcp_module();
	fns = &tcp_fns;
#endif
#ifdef __NET_IB__
	setup_ib_module();
	fns = &ib_fns;
#endif

	if (fns == NULL)
		error("No network protocol is chosen.\n");

	network_systems_initialized = 1;
	
	return fns;
}

/*
 * get_network_archs_count()
 *
 * Get count of all suppoted network architecures
 */
int get_network_archs_count()
{
	int count = 0;
	
	if (network_systems_initialized == 0)
		error("Network systems must be initialized first.\n");
#ifdef __NET_TCP__
	count++;
#endif
#ifdef __NET_IB__
	count++;
#endif

	return count;
}

/*
 * get_network_archs()
 *
 * Get function pointer of all supported network architectures.
 */
void get_network_archs(struct network_fns **archs)
{
	if (network_systems_initialized == 0)
		error("Network systems must be initialized first.\n");
	if (archs == NULL)
		error("Null archs pointer.\n");
	
#ifdef __NET_TCP__
	*archs = &tcp_fns;
	archs++;
#endif
#ifdef __NET_IB__
	*archs = &ib_fns;
	archs++;
#endif
}

/*
 * vim: ts=4 sts=4 sw=4
 */
