#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include "net-types.h"

/*
 * List of supported network architectures
 */
#ifdef __NET_TCP__
#define MAX_TCP_IOV_COUNT   32
extern struct network_fns tcp_fns;
int setup_tcp_module(void);
#endif
#ifdef __NET_IB__
extern struct network_fns ib_fns;
int setup_ib_module(void);
#endif

/* Initialize basic network setups */
int setup_network_systems(void);

struct network_fns
{
    /* Network method name */
    char *name;
    
    /* Initialize specific network setups */
    int (*Initialize)(char *hostname, int port, int init_flags);
    
    /* Close network module */
    int (*Finalize)(void);

    /* Establish a connection to host_addr */
    int (*Connect)(struct host_addr_t *host_addr);

    /* Blocking send to node with socket 'dest' */
    int (*Send)(int dest, void *buffer, comm_size_t size, comm_tag_t tag);

    /* Blocking recv from node with socket 'src' */
    int (*Recv)(int src, void *buffer, comm_size_t size, comm_tag_t tag, Status *status);
    
    /* Nonblocking send to node with socket 'dest' */
    int (*Isend)(int dest, void *buffer, comm_size_t size, comm_tag_t tag, Request *request);

    /* Nonblocking recv from node with socket 'src' */
    int (*Irecv)(int src, void *buffer, comm_size_t size, comm_tag_t tag, Request *request);

    /* Blocking send a list of data to node with socket 'dest' */
    int (*Send_list)(int dest, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag);

    /* Blocking recv a list of data from node with socket 'src' */
    int (*Recv_list)(int src, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag, Status *status);

    /* Nonblocking send a list of data to node with socket 'dest' */
    int (*Isend_list)(int dest, void **buffer_list, comm_size_t *size_list, int list_count, 
	    comm_tag_t tag, Request *request);

    /* Nonblocking recv a list of data from node with socket 'src' */
    int (*Irecv_list)(int src, void **buffer_list, comm_size_t *size_list, int list_count,
	    comm_tag_t tag, Request *request);

    /* Blocking probe for incoming request */
    int (*Probe)(int src, comm_tag_t tag, Status *status);

    /* Nonblocking probe for incoming request */
    int (*Iprobe)(int src, comm_tag_t tag, int *flag, Status *status);

    /* Test if nonblocking send/recv is completed */
    int (*Test)(Request *request, Status *status);
    
    /* Wait for completion of nonblocking send/recv */
    int (*Wait)(Request *request, Status *status);
    
    /* Cancel a network operation */
    int (*Cancel)();

    /* Get IP address */
    char *(*GetAddress)(int sock);
};

#endif

/*
 * vim: ts=8 sts=4 sw=4
 */
