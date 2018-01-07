#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <vapi.h>
#include <vapi_common.h>
#include "debug.h"
#include "memory_util.h"
#include "net.h"
#include "ib.h"

/* InfiniBand Resources */
static char *DeviceID = "InfiniHost0";
static int port_num = 1;
static uint32_t num_cqe = 5000;

static VAPI_hca_hndl_t hca_hndl;
static VAPI_hca_port_t hca_port;
static VAPI_pd_hndl_t pd_hndl;
static VAPI_cq_hndl_t cq_hndl;

#define IB_THREADS 1

static struct queue_item_t sendq[SENDQ_MAX];
static struct queue_item_t recvq[RECVQ_MAX];
static struct queue_item_t socketq;
static pthread_mutex_t sendq_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t recvq_mutex = PTHREAD_MUTEX_INITIALIZER;

static int ib_basic_initialized = 0;
/* threads to processing TCP/IP operations */
static pthread_t ib_threads[IB_THREADS];

static int module_initialized = 0;

struct service_list_t 
{
	list_t list;
	struct server_addr_t server;
};

/* list to keep track of what hostname/port pair server has created */
static struct service_list_t service_list;

struct buffer_msg_t
{
    int type;
    void *buffer;
    comm_size_t size;
    void **buffer_list;			/* buffer list */
    comm_size_t *size_list;	    /* size list */
    int list_count;				/* count of buffer  */
    int iov_idx;				/* index to the io vector */
    int iov_len_done;		    /* size remained in current index 'iov_idx' */
    comm_size_t request_size;	/* total request size from user */
    comm_size_t complete_size;	/* total size we have processed */
    Status *status;				/* queue status (only useful for nonblocking send operations) */
};

int IB_Initialize(char *hostname, int port, int init_flags);
int IB_Finalize(void);
int IB_Connect(struct host_addr_t *host_addr);
int IB_Send(int dest, void *buffer, comm_size_t size, comm_tag_t tag);
int IB_Recv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Status *status);
int IB_Isend(int dest, void *buffer, comm_size_t size, comm_tag_t tag, Request *request);
int IB_Irecv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Request *request);
int IB_Send_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag);
int IB_Recv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag, Status *status);
int IB_Isend_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, 
	comm_tag_t tag, Request *request);
int IB_Irecv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count,
	comm_tag_t tag, Request *request);
int IB_Probe(int src, comm_tag_t tag, Status *status);
int IB_Iprobe(int src, comm_tag_t tag, int *flag, Status *status);
int IB_Test(Request *request, Status *status);
int IB_Wait(Request *request, Status *status);
int IB_Close(int sock);
int IB_Cancel();
char *IB_GetAddress(int sock);

static int check_new_connection();
#if 0
static void move_data(void **buffer_list, comm_size_t *size_list, int list_count, comm_size_t old_size, 
	comm_size_t request_size, struct buffer_msg_t *data);
#endif

struct network_fns ib_fns = {
    "ib",
    IB_Initialize,
    IB_Finalize,
    IB_Connect,
    IB_Send,
    IB_Recv,
    IB_Isend,
    IB_Irecv,
    IB_Send_list,
    IB_Recv_list,
    IB_Isend_list,
    IB_Irecv_list,
    IB_Probe,
    IB_Iprobe,
    IB_Test,
    IB_Wait,
	IB_Close,
    IB_Cancel,
    IB_GetAddress,
};

static void process_send_messages()
{
}

static void poll_recv_messages()
{
}

void *ib_queue_func(void *argv)
{
	while (1)
	{
		/* 
		 * check out any incoming connection request 
		 */
		check_new_connection();
		
		/*
		 * Process one pending send request
		 */
		pthread_mutex_lock(&sendq_mutex);
		process_send_messages();
		pthread_mutex_unlock(&sendq_mutex);
		
		/*
		 * Polling and buffering for incoming messages
		 */
		pthread_mutex_lock(&recvq_mutex);
		poll_recv_messages();
		pthread_mutex_unlock(&recvq_mutex);
		
		usleep(1);
	}
	pthread_exit(NULL);
}

int setup_ib_module(void)
{
	int i, ret;

	if (module_initialized == 1)
		return 0;
	
    debug(DEBUG_NETWORK, "Initializing IB module.\n");
    
	INIT_LIST_HEAD(&sendq[SENDQ_INPROGRESS].list);
	INIT_LIST_HEAD(&sendq[SENDQ_PENDING].list);

	INIT_LIST_HEAD(&recvq[RECVQ_INPROGRESS].list);
	INIT_LIST_HEAD(&recvq[RECVQ_COMPLETE].list);
	INIT_LIST_HEAD(&recvq[RECVQ_PENDING].list);
	INIT_LIST_HEAD(&socketq.list);

	INIT_LIST_HEAD(&service_list.list);
//	INIT_LIST_HEAD(&ipq.list);

	for (i=0; i<IB_THREADS; i++)
	{
		ret = pthread_create(&ib_threads[i], NULL, ib_queue_func, NULL);
		if (ret < 0)
			error("Thread creation failed\n");
	}
	
	module_initialized = 1;

    return 0;
}

static void async_events_handler( VAPI_hca_hndl_t hca_hndl, VAPI_event_record_t *event_p, void* priv_data)
{
	debug(DEBUG_NETWORK, "Got async. event: %s (0x%x - %s)\n",
			VAPI_event_record_sym(event_p->type),
			event_p->syndrome,
			VAPI_event_syndrome_sym(event_p->syndrome));
}

static void ib_basic_init()
{
	int act_num_cqe;

	VAPI_ret_t ret;
	EVAPI_async_handler_hndl_t async_hndl;

	/* Create handle */
	ret = EVAPI_get_hca_hndl(DeviceID, &hca_hndl);
    if (ret != VAPI_OK) 
		error("Opening HCA : %s\n", VAPI_strerror(ret));

	/* Get lid */
	ret = VAPI_query_hca_port_prop(hca_hndl,(IB_port_t)port_num, ( VAPI_hca_port_t *)&hca_port);
	if (ret != VAPI_OK)
		error("Opening HCA : %s\n", VAPI_strerror(ret));

	debug(DEBUG_NETWORK, "lid = 0x%x max_mtu = %d max_msg_sz = %u.\n",
			hca_port.lid, hca_port.max_mtu, hca_port.max_msg_sz);

	/* Get protection domain (PD) */
	ret = EVAPI_alloc_pd(hca_hndl, 256, &pd_hndl);
	if (ret != VAPI_OK)
		error("Allocating PD : %s\n", VAPI_strerror(ret));

	/* Create Completion Queue (CQ) */
	ret = VAPI_create_cq(hca_hndl, num_cqe, &cq_hndl, &act_num_cqe);
	if (ret != VAPI_OK)
		error("Creating CQ : %s\n", VAPI_strerror(ret));

	/* Set event handler */
	ret = EVAPI_set_async_event_handler(hca_hndl, async_events_handler, NULL, &async_hndl);
	if (ret != VAPI_OK)
		error("Creating in EVAPI_set_async_event_handler : %s\n", VAPI_strerror(ret));
}

// aaa
int IB_Initialize(char *hostname, int port, int init_flags)
{
	struct server_addr_t *addr = NULL;

	pthread_mutex_lock(&sendq_mutex);
	pthread_mutex_lock(&recvq_mutex);
	
	if (ib_basic_initialized == 0)
	{
		ib_basic_init();
		ib_basic_initialized = 1;
	}
	
	if (init_flags & INIT_VSERVER)
	{
		if (module_initialized == 0)
			error("IB module is not initialized\n");
		
		if (port < 0)
			error("Invalid port number\n");

		list_for_each(l, &service_list.list)
		{
			tmp_service = (struct service_list_t *)l;
			if (tmp_service->server.port == port)
			{

			}
		}
//		addr = &service->server;
	}

	pthread_mutex_unlock(&recvq_mutex);
	pthread_mutex_unlock(&sendq_mutex);
	
	return 0;
}

int IB_Finalize(void)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Connect(struct host_addr_t *host_addr)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Send(int dest, void *buffer, comm_size_t size, comm_tag_t tag)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Recv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Status *status)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Isend(int dest, void *buffer, comm_size_t size, comm_tag_t tag, Request *request)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Irecv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Request *request)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Send_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Recv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag, Status *status)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Isend_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, 
		comm_tag_t tag, Request *request)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Irecv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count,
		comm_tag_t tag, Request *request)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Probe(int src, comm_tag_t tag, Status *status)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Iprobe(int src, comm_tag_t tag, int *flag, Status *status)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Test(Request *request, Status *status)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Wait(Request *request, Status *status)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Close(int sock)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

int IB_Cancel()
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

char *IB_GetAddress(int sock)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);

	return 0;
}

static int check_new_connection()
{
	return 0;
}

#if 0
static void move_data(void **buffer_list, comm_size_t *size_list, int list_count, comm_size_t old_size, 
		comm_size_t request_size, struct buffer_msg_t *data)
{
	debug(DEBUG_NETWORK, "%s entered.\n", __func__);
}
#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
