#include <pthread.h>
#include "debug.h"
#include "net.h"

#ifdef __NET_TCP__
#include "tcp.h"
#endif
#ifdef __NET_IB__
#include "ib.h"
#endif

struct queue_item_t sendq[SENDQ_MAX];
struct queue_item_t recvq[RECVQ_MAX];
struct queue_item_t socketq;

pthread_mutex_t sendq_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recvq_mutex = PTHREAD_MUTEX_INITIALIZER;

int setup_network_systems(void)
{
	init_time();
	
	INIT_LIST_HEAD(&sendq[SENDQ_INPROGRESS].list);
	INIT_LIST_HEAD(&sendq[SENDQ_PENDING].list);
	
	INIT_LIST_HEAD(&recvq[RECVQ_INPROGRESS].list);
	INIT_LIST_HEAD(&recvq[RECVQ_COMPLETE].list);
	INIT_LIST_HEAD(&recvq[RECVQ_PENDING].list);
	INIT_LIST_HEAD(&socketq.list);
	
#ifdef __NET_TCP__
	setup_tcp_module();
#endif
#ifdef __NET_IB__
	setup_ib_module();
#endif
	return 1;
}

/*
 * vim: ts=4 sts=4 sw=4
 */
