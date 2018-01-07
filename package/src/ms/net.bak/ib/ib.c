#include "net.h"
#include "ib.h"

struct buffer_msg_t
{
    int type;
    void *buffer;
    comm_size_t size;
    void **buffer_list;		    /* buffer list */
    comm_size_t *size_list;	    /* size list */
    int list_count;		    /* count of buffer  */
    int iov_idx;		    /* index to the io vector */
    int iov_len_done;		    /* size remained in current index 'iov_idx' */
    comm_size_t request_size;	    /* total request size from user */
    comm_size_t complete_size;	    /* total size we have processed */
    Status *status;		    /* queue status (only useful for nonblocking send operations) */
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
int IB_Cancel();
char *IB_GetAddress(int sock);

int check_new_connection();
void move_data(void **buffer_list, comm_size_t *size_list, int list_count, comm_size_t old_size, 
	comm_size_t request_size, struct buffer_msg_t *data);

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
    IB_Cancel,
    IB_GetAddress,
};

int IB_Initialize(char *hostname, int port, int init_flags)
{
    return 0;
}

int IB_Finalize(void)
{
    return 0;
}

int IB_Connect(struct host_addr_t *host_addr)
{
    return 0;
}

int IB_Send(int dest, void *buffer, comm_size_t size, comm_tag_t tag)
{
    return 0;
}

int IB_Recv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Status *status)
{
    return 0;
}

int IB_Isend(int dest, void *buffer, comm_size_t size, comm_tag_t tag, Request *request)
{
    return 0;
}

int IB_Irecv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Request *request)
{
    return 0;
}

int IB_Send_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag)
{
    return 0;
}

int IB_Recv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag, Status *status)
{
    return 0;
}

int IB_Isend_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, 
		comm_tag_t tag, Request *request)
{
    return 0;
}

int IB_Irecv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count,
		comm_tag_t tag, Request *request)
{
    return 0;
}

int IB_Probe(int src, comm_tag_t tag, Status *status)
{
    return 0;
}

int IB_Iprobe(int src, comm_tag_t tag, int *flag, Status *status)
{
    return 0;
}

int IB_Test(Request *request, Status *status)
{
    return 0;
}

int IB_Wait(Request *request, Status *status)
{
    return 0;
}

int IB_Cancel()
{
    return 0;
}

char *IB_GetAddress(int sock)
{
    return 0;
}

int check_new_connection()
{
    return 0;
}

void move_data(void **buffer_list, comm_size_t *size_list, int list_count, comm_size_t old_size, 
		comm_size_t request_size, struct buffer_msg_t *data)
{
}

/*
 * vim: ts=8 sts=4 sw=4
 */
