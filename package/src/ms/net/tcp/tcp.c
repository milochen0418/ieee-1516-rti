#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include "debug.h"
#include "memory_util.h"
#include "uuid_gen.h"
#include "net.h"
#include "sock.h"
#include "tcp.h"

#define TCP_BACKLOG	256
#define TCP_THREADS	1

#define TCP_HEADER_SIZE		sizeof(struct msg_header_t)

struct queue_item_t sendq[SENDQ_MAX];
struct queue_item_t recvq[RECVQ_MAX];
struct queue_item_t socketq;
pthread_mutex_t sendq_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recvq_mutex = PTHREAD_MUTEX_INITIALIZER;

static int socket_count = 0;
static struct pollfd *recv_pollfd = NULL;

/* threads to processing TCP/IP operations */
static pthread_t tcp_threads[TCP_THREADS];

static int module_initialized = 0;

/* queue to record socket and ip address pair */
struct queue_item_t ipq;
struct ipaddr_t
{
	list_t list;
	int sock;
	char *ip;
};

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
	comm_size_t *size_list;		/* size list */
	int list_count;				/* count of buffer  */
	int iov_idx;				/* index to the io vector */
	int iov_len_done;			/* size remained in current index 'iov_idx' */
	comm_size_t request_size;	/* total request size from user */
	comm_size_t complete_size;	/* total size we have processed */
	Status *status;				/* queue status (only useful for nonblocking send operations) */
};

int TCP_Initialize(char *hostname, int port, int init_flags);
int TCP_Finalize(void);
int TCP_Connect(struct host_addr_t *host_addr);
int TCP_Send(int dest, void *buffer, comm_size_t size, comm_tag_t tag);
int TCP_Recv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Status *status);
int TCP_Isend(int dest, void *buffer, comm_size_t size, comm_tag_t tag, Request *request);
int TCP_Irecv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Request *request);
int TCP_Send_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag);
int TCP_Recv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag, Status *status);
int TCP_Isend_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, 
		comm_tag_t tag, Request *request);
int TCP_Irecv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count,
		comm_tag_t tag, Request *request);
int TCP_Probe(int src, comm_tag_t tag, Status *status);
int TCP_Iprobe(int src, comm_tag_t tag, int *flag, Status *status);
int TCP_Test(Request *request, Status *status);
int TCP_Wait(Request *request, Status *status);
int TCP_Close(int sock);
int TCP_Cancel();
char *TCP_GetAddress(int sock);

int check_new_connection();
static void move_data(void **buffer_list, comm_size_t *size_list, int list_count, comm_size_t old_size, 
		comm_size_t request_size, struct buffer_msg_t *data);

struct network_fns tcp_fns = {
	"tcp",
	TCP_Initialize,
	TCP_Finalize,
	TCP_Connect,
	TCP_Send,
	TCP_Recv,
	TCP_Isend,
	TCP_Irecv,
	TCP_Send_list,
	TCP_Recv_list,
	TCP_Isend_list,
	TCP_Irecv_list,
	TCP_Probe,
	TCP_Iprobe,
	TCP_Test,
	TCP_Wait,
	TCP_Close,
	TCP_Cancel,
	TCP_GetAddress,
};

/*
 * ANY_cmp()
 *
 * Compare ANY_SOURCE and ANY_TAG
 *
 * Return 0 if matching. Otherwise return 1.
 */
int ANY_cmp(void *list_key, void *user_key){
	struct network_key_t *key1 = (struct network_key_t *)list_key;
	struct network_key_t *key2 = (struct network_key_t *)user_key;
	
	if (key1->network_arch != key2->network_arch)
		return 1;
	if (key1->socket != ANY_SOURCE && key2->socket != ANY_SOURCE 
			&& key1->socket != key2->socket)
		return 1;
	if (key1->tag != ANY_TAG && key2->tag != ANY_TAG 
			&& key1->tag != key2->tag)
		return 1;

	return 0;
}

void process_send_messages()
{
	int i, ret=0, count, start;
	struct queue_item_t *item = NULL;
	struct network_key_t *network_key = NULL;
	struct buffer_msg_t *data = NULL;
	struct iovec io_vector[MAX_TCP_IOV_COUNT+1];
	comm_size_t total_size = 0;
	list_t *l;

	list_for_each(l, &sendq[SENDQ_INPROGRESS].list)
	{
		item = (struct queue_item_t *)l;
		network_key = (struct network_key_t *)item->key;
		data = (struct buffer_msg_t *)item->data;
		
		/* fill payload in io vector */
		count = 0;
		if (data->iov_len_done != 0){
			io_vector[count].iov_base = (char *)data->buffer_list[data->iov_idx] + data->iov_len_done;
			io_vector[count].iov_len = data->size_list[data->iov_idx] - data->iov_len_done;
			start = data->iov_idx + 1;
			count++;
		}
		else
			start = data->iov_idx;
		
		for (i=start; i<data->list_count; i++){
			io_vector[count].iov_base = data->buffer_list[i];
			io_vector[count].iov_len = data->size_list[i];
			count++;
		}
		
		ret = sock_nbvector(network_key->socket, io_vector, count, TCP_SEND);
		if (ret < 0){
			error("%s: internel error\n", __func__);
		}

		/* 
		 * Update iov_idx and iov_len_done and complete size 
		 */
		total_size = data->complete_size;
		data->complete_size += ret;

		if (data->complete_size == data->request_size){
			/* we have finished all data sending of this send request */
			data->status->count = data->request_size;
			data->status->source = network_key->socket;
			data->status->tag = network_key->tag;
			data->status->error = STATUS_COMPLETE;

			free(data);

			remove_queue(item);
			delete_item(item);
			break;
		}

		if (data->iov_len_done != 0){
			if (ret >= data->size_list[data->iov_idx] - data->iov_len_done){
				total_size += (data->size_list[data->iov_idx] - data->iov_len_done);
				data->iov_len_done = 0;
				data->iov_idx++;
			}
			else{
				data->iov_len_done += ret;
				break;
			}
		}
		
		for (i=data->iov_idx; i<data->list_count; i++){
			total_size += data->size_list[i];
			
			if (total_size > data->complete_size){
				data->iov_idx = i;
				data->iov_len_done = data->size_list[i] - (total_size - data->complete_size);
				break;
			}
			else if (total_size == data->complete_size){
				data->iov_idx = i+1;
				data->iov_len_done = 0;
				break;
			}
		}
		
		break;
	}
}

void poll_recv_messages()
{
	int i, ret, count = 0;
	int payload_size=0;
	struct msg_header_t tcp_header;
	int header_size = sizeof(struct msg_header_t);
	struct network_key_t key_s;		/* key value for item searching */
	struct network_key_t *key = NULL;
	struct queue_item_t *item = NULL;
	struct buffer_msg_t *data = NULL;

	do {
		count = poll(recv_pollfd, socket_count, 10);
	} while (count < 0 && errno == EINTR);

	if (count > 0){
		/* Some message has arrived, we need to check it */
		for (i=0; i<socket_count; i++){
			if (recv_pollfd[i].revents && (recv_pollfd[i].revents & POLLIN)){

				/* Pick up the pending recv request with this socket number */
				key_s.network_arch = ARCH_TCP;
				key_s.socket = recv_pollfd[i].fd;
				key_s.tag = ANY_TAG;
				item = search_queue_cmp(&recvq[RECVQ_INPROGRESS], &key_s, ANY_cmp);
				
				if (item != NULL){
					/* 
					 * The previous receive request for this socket number is not completed. 
					 * It is because the recv request begins before the send request.
					 * We need to continue processing it.
					 */
					data = (struct buffer_msg_t *)item->data;
					
					ret = sock_nbrecv(recv_pollfd[i].fd, &((char *)data->buffer)[data->complete_size], 
							data->size - data->complete_size);
					
					if (ret < 0)
					{
						error("%s: message out of order(in-progressed message)\n", __func__);
					}
					
					data->complete_size += ret;
					
					if (data->complete_size == data->size)
					{
						remove_queue(item);
						insert_queue(&recvq[RECVQ_COMPLETE], item->key, item->data);
						free(item);
						
					}
					continue;
				}
				
				/* 
				 * No any previous recv request is pending.
				 * It is because the send request begins before the recv request.
				 * And a new receive request for this socket number arrives.
				 */
				ret = sock_nbpeek(recv_pollfd[i].fd, &tcp_header, TCP_HEADER_SIZE);
				if (ret < header_size){
					/* the header does not fully arrive */
					continue;
				}
				
				if (tcp_header.magic_nr != COMM_MAGIC_NR){
					error("%s: magic number not the same\n", __func__);
				}

				/* 
				 * Sender-side issues send request before receiver-side side issues receive request.
				 * We just buffer it to a contiguous memory region.
				 */
				key = Malloc(sizeof(struct network_key_t));
				data = Malloc(sizeof(struct buffer_msg_t));
			
				/* blocking recv to retrieve the header */
				memset(&tcp_header, 0, sizeof(tcp_header));
				ret = sock_brecv(recv_pollfd[i].fd, &tcp_header, TCP_HEADER_SIZE);
				payload_size = tcp_header.size;

				debug(DEBUG_NETWORK, "%s: one incomming request size = %d tag = %lld\n",
						__func__, payload_size, tcp_header.tag);
					
				key->socket = recv_pollfd[i].fd;
				key->network_arch = ARCH_TCP;
				key->tag = tcp_header.tag;
					
				data->type = MSG_RECV;
				data->buffer = Malloc(payload_size);
				data->size = payload_size;
				data->buffer_list = &data->buffer;
				data->size_list = &data->size;
				data->list_count = 1;

				ret = sock_nbrecv(recv_pollfd[i].fd, data->buffer, data->size);
				if (ret < 0)
				{
					error("%s: message out of order\n", __func__);
				}
				data->request_size = 0;
				data->complete_size = ret;

				if (data->complete_size == payload_size){
					insert_queue(&recvq[RECVQ_COMPLETE], key, data);
				}
				else{
					insert_queue(&recvq[RECVQ_INPROGRESS], key, data);
				}
			}
		}
	}
}

void *tcp_queue_func(void *argv)
{
	while (1)
	{
		/* 
		 * Check any incoming connection request 
		 */
		pthread_mutex_lock(&recvq_mutex);
		check_new_connection();
		pthread_mutex_unlock(&recvq_mutex);

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

int setup_tcp_module(void)
{
	int i, ret;
	
	if (module_initialized == 1)
		return 0;
	
	debug(DEBUG_NETWORK, "Initializing TCP/IP module.\n");
	
	INIT_LIST_HEAD(&sendq[SENDQ_INPROGRESS].list);
	INIT_LIST_HEAD(&sendq[SENDQ_PENDING].list);
	
	INIT_LIST_HEAD(&recvq[RECVQ_INPROGRESS].list);
	INIT_LIST_HEAD(&recvq[RECVQ_COMPLETE].list);
	INIT_LIST_HEAD(&recvq[RECVQ_PENDING].list);
	INIT_LIST_HEAD(&socketq.list);
	
	INIT_LIST_HEAD(&service_list.list);
	INIT_LIST_HEAD(&ipq.list);

	for (i=0; i<TCP_THREADS; i++)
	{
		ret = pthread_create(&tcp_threads[i], NULL, tcp_queue_func, NULL);
		if (ret < 0)
			error("thread creation failed\n");
	}
	
	module_initialized = 1;

	return 0;
}
/*
 * init_server()
 * 
 * Initialize server socket
 *
 * On success, a valid socket number will return
 */
static int init_server(struct server_addr_t *addr){
	int oldfl = 0;
	int sock;
	
	/* create a socket */
	if ((sock = sock_create()) < 0) {
		error("initialize server socket failed\n");
	}

	/* set it to non-blocking operation */
	oldfl = fcntl(sock, F_GETFL, 0);
	if (!(oldfl & O_NONBLOCK)){
		fcntl(sock, F_SETFL, oldfl | O_NONBLOCK);
	}

	/* setup for a fast restart to avoid bind addr in use errors */
	sock_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 1);

	/* bind it to the appropriate port */
	if ((sock_bind(sock, addr->hostname, addr->port)) == -1)
		return -1;
	
	/* go ahead and listen to the socket */
	if (listen(sock, TCP_BACKLOG) != 0){
		error("server socket listening failed\n");
	}

	return sock;
}

/*
 * TCP_Initialize()
 *
 * Create server service.
 */
int TCP_Initialize(char *hostname, int port, int init_flags)
{
	int sock = -1;
	struct server_addr_t *addr = NULL;
	struct service_list_t *service = NULL;
	struct service_list_t *tmp_service;
	struct ipaddr_t *ipaddr = NULL;
	list_t *l = NULL;

	pthread_mutex_lock(&sendq_mutex);
	pthread_mutex_lock(&recvq_mutex);
	
	if (init_flags & INIT_VSERVER)
	{
		if (module_initialized == 0)
			error("TCP/IP module is not initialized\n");

		if (port < 0)
			error("Invalid port number\n");
		
		list_for_each(l, &service_list.list)
		{
			tmp_service = (struct service_list_t *)l;
			if (tmp_service->server.port == port)
			{
				warning("service port %d has been used\n", port);
				pthread_mutex_unlock(&recvq_mutex);
				pthread_mutex_unlock(&sendq_mutex);
				return -1;
			}
		}

		service = Malloc(sizeof(struct service_list_t));
		INIT_LIST_HEAD(&service->list);

		addr = &service->server;
		memset(addr, 0, sizeof(struct server_addr_t));
		
		addr->hostname = hostname;
		addr->port = port;
		
		sock = init_server(addr);
		if (sock == -1)
		{
			/* duplicated port number */
			free(service);
			pthread_mutex_unlock(&recvq_mutex);
			pthread_mutex_unlock(&sendq_mutex);
			return -1;
		}
		
		addr->socket = sock;
		addr->is_initialized = 1;
		list_add_tail(&service->list, &service_list.list);
		
		debug(DEBUG_NETWORK, "%s: create listen socket %d\n", __func__, sock);
		
		/* add this IP address to ipq */
		if (hostname == NULL)
			addr->hostname = strdup("localhost");
		else
			addr->hostname = strdup(hostname);

		ipaddr = (struct ipaddr_t *)Malloc(sizeof(struct ipaddr_t));
		INIT_LIST_HEAD(&ipaddr->list);
		ipaddr->sock = addr->socket;
		ipaddr->ip = strdup(addr->hostname);
		list_add_tail(&ipaddr->list, &ipq.list);
	}

	pthread_mutex_unlock(&recvq_mutex);
	pthread_mutex_unlock(&sendq_mutex);
	
	return sock;
}

/*
 * TCP_Finalize()
 *
 * Close TCP/IP module.
 */
int TCP_Finalize()
{
	return 0;
}

/*
 * TCP_Connect()
 *
 * Establish a connection
 */
int TCP_Connect(struct host_addr_t *addr)
{
	int ret;
	int oldfl = 0;
	struct pollfd poll_conn;
	struct pollfd *tmp;
	struct network_key_t *key;
	struct ipaddr_t *ipaddr = NULL;

	if (addr == NULL)
		error("NULL hosst addr pointer\n");

	if (addr->is_connected)
		return 1;

	/* socket has existed */
	if (addr->socket > -1)
	{
		poll_conn.fd = addr->socket;
		poll_conn.events = POLLOUT;
		ret = poll(&poll_conn, 1, 2);
		if ((ret < 0) || (poll_conn.revents & POLLERR)){
			error("poll: %s\n", strerror(errno));
		}
		if (poll_conn.revents & POLLOUT){
			addr->is_connected = 1;
		}

		if (sock_setsockopt(addr->socket, SOL_SOCKET, SO_SNDBUF, SOCK_BUFF_SIZE) < 0)
			error("%s: set send buffer size\n", __func__);
		
		if (sock_setsockopt(addr->socket, SOL_SOCKET, SO_RCVBUF, SOCK_BUFF_SIZE) < 0)
			error("%s: set recv buffer size\n", __func__);
		
		debug(DEBUG_NETWORK, "\treset send/recv bufsize from (%d, %d) to (%d, %d)\n", 
				__func__, sock_getsockopt(addr->socket, SO_SNDBUF), sock_getsockopt(addr->socket, SO_RCVBUF),
				SOCK_BUFF_SIZE, SOCK_BUFF_SIZE);


		pthread_mutex_lock(&recvq_mutex);
		key = Malloc(sizeof(struct network_key_t));
		key->network_arch = ARCH_TCP;
		key->socket = addr->socket;
		key->tag = -1;
		insert_queue(&socketq, key, NULL);
		
		tmp = Malloc((socket_count+1) * sizeof(struct pollfd));
		if (socket_count == 0){
			tmp[0].fd = addr->socket;
			tmp[0].events = POLLIN;
			recv_pollfd = tmp;
		}
		else if (socket_count > 0){
			memcpy(tmp, recv_pollfd, socket_count *sizeof(struct pollfd));
			tmp[socket_count].fd = addr->socket;
			tmp[socket_count].events = POLLIN;
			free(recv_pollfd);
			recv_pollfd = tmp;
		}
		socket_count++;

		/* add this IP address to ipq */
		ipaddr = (struct ipaddr_t *)Malloc(sizeof(struct ipaddr_t));
		INIT_LIST_HEAD(&ipaddr->list);
		ipaddr->sock = addr->socket;
		ipaddr->ip = strdup(addr->hostname);
		list_add_tail(&ipaddr->list, &ipq.list);

		pthread_mutex_unlock(&recvq_mutex);

		return SUCCESS;
	}

	if (addr->hostname == NULL)
	{
		warning("The hostname will be set to 'localhost'\n");
		addr->hostname = strdup("localhost");
	}
	if (addr->port < 0)
		error("invalid port number\n");

	if ((addr->socket = sock_create()) < 0)
		error("%s: initialize socket\n", __func__);
	
	/* set it to non-blocking operation */
	oldfl = fcntl(addr->socket, F_GETFL, 0);
	if (!(oldfl & O_NONBLOCK)){
		fcntl(addr->socket, F_SETFL, oldfl | O_NONBLOCK);
	}

	/* Don't delay send to coalesce packets  */
	if (sock_setsockopt(addr->socket, IPPROTO_TCP, TCP_NODELAY, 1) < 0)
		error("failed to set TCP_NODELAY option.\n");

	ret = sock_connect(addr->socket, addr->hostname, addr->port);
	if (ret < 0){
		if (errno == EINPROGRESS)
			return 0;
		else
			error("sock_connect: %s\n", strerror(errno));
	}
	
	if (sock_setsockopt(addr->socket, SOL_SOCKET, SO_SNDBUF, SOCK_BUFF_SIZE) < 0)
		error("%s: set send buffer size\n", __func__);
	
	if (sock_setsockopt(addr->socket, SOL_SOCKET, SO_RCVBUF, SOCK_BUFF_SIZE) < 0)
		error("%s: set recv buffer size\n", __func__);
	
	debug(DEBUG_NETWORK, "\treset send/recv bufsize from (%d, %d) to (%d, %d)\n", 
			__func__, sock_getsockopt(addr->socket, SO_SNDBUF), sock_getsockopt(addr->socket, SO_RCVBUF),
			SOCK_BUFF_SIZE, SOCK_BUFF_SIZE);

	pthread_mutex_lock(&recvq_mutex);
	key = Malloc(sizeof(struct network_key_t));
	key->network_arch = ARCH_TCP;
	key->socket = addr->socket;
	key->tag = -1;
	insert_queue(&socketq, key, NULL);
	
	tmp = Malloc((socket_count+1) * sizeof(struct pollfd));
	if (socket_count == 0){
		tmp[0].fd = addr->socket;
		tmp[0].events = POLLIN;
		recv_pollfd = tmp;
	}
	else if (socket_count > 0){
		memcpy(tmp, recv_pollfd, socket_count *sizeof(struct pollfd));
		tmp[socket_count].fd = addr->socket;
		tmp[socket_count].events = POLLIN;
		free(recv_pollfd);
		recv_pollfd = tmp;
	}
	socket_count++;

	/* add this IP address to ipq */
	ipaddr = (struct ipaddr_t *)Malloc(sizeof(struct ipaddr_t));
	INIT_LIST_HEAD(&ipaddr->list);
	ipaddr->sock = addr->socket;
	ipaddr->ip = strdup(addr->hostname);
	list_add_tail(&ipaddr->list, &ipq.list);

	pthread_mutex_unlock(&recvq_mutex);

	addr->is_connected = 1;

	return SUCCESS;
}

/*
 * net_accept()
 * 
 * Accept an incomming connection
 */
int net_accept(int server_sock)
{
	int sock;
	struct sockaddr_in cin;
	int cin_len = sizeof(struct sockaddr_in);
	struct network_key_t *key;
	struct pollfd *tmp = NULL;
	struct ipaddr_t *ipaddr = NULL;
	
	memset(&cin, 0, sizeof(struct sockaddr_in));
	sock = accept(server_sock, (struct sockaddr *)&cin, &cin_len);
	if (sock < 0){
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == ENETDOWN) 
				|| (errno == EPROTO) || (errno == ENOPROTOOPT) || (errno == EHOSTDOWN) 
				||	(errno == ENONET) || (errno == EHOSTUNREACH) || (errno == EOPNOTSUPP) 
				|| (errno == ENETUNREACH))
		{
			return 0;
		}
		else
			error("accept failed\n");
	}

	debug(DEBUG_NETWORK, "Socket %d accepts connection %d from %s:%d\n", 
			server_sock, sock, inet_ntoa(cin.sin_addr), cin.sin_port);

	if (sock_setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, 1) < 0)
		error("%s: set socket to no delay\n", __func__);

	if (sock_setsockopt(sock, SOL_SOCKET, SO_SNDBUF, SOCK_BUFF_SIZE) < 0)
		error("%s: set send buffer size\n", __func__);

	if (sock_setsockopt(sock, SOL_SOCKET, SO_RCVBUF, SOCK_BUFF_SIZE) < 0)
		error("%s: set recv buffer size\n", __func__);

	debug(DEBUG_NETWORK, "\treset send/recv bufsize from (%d, %d) to (%d, %d)\n", 
			__func__, sock_getsockopt(sock, SO_SNDBUF), sock_getsockopt(sock, SO_RCVBUF),
			SOCK_BUFF_SIZE, SOCK_BUFF_SIZE);

	key = Malloc(sizeof(struct network_key_t));
	key->network_arch = ARCH_TCP;
	key->socket = sock;
	key->tag = -1;
	insert_queue(&socketq, key, NULL);

	tmp = Malloc((socket_count+1) * sizeof(struct pollfd));
	if (socket_count == 0){
		tmp[0].fd = sock;
		tmp[0].events = POLLIN;
		recv_pollfd = tmp;
	}
	else if (socket_count > 0){
		memcpy(tmp, recv_pollfd, socket_count *sizeof(struct pollfd));
		tmp[socket_count].fd = sock;
		tmp[socket_count].events = POLLIN;
		free(recv_pollfd);
		recv_pollfd = tmp;
	}

	socket_count++;
	
	/* add this IP address to ipq */
	ipaddr = (struct ipaddr_t *)Malloc(sizeof(struct ipaddr_t));
	INIT_LIST_HEAD(&ipaddr->list);
	ipaddr->sock = sock;
	ipaddr->ip = strdup(inet_ntoa(cin.sin_addr));
	list_add_tail(&ipaddr->list, &ipq.list);

	return sock;
}

int TCP_Send(int dest, void *buffer, comm_size_t size, comm_tag_t tag)
{
	int ret;
	Request request;
	Status status;
	
	if (buffer == NULL)
		error("%s: null buffer pointer\n", __func__);
	if (size == 0){
		warning("%s: no data be be sent\n", __func__);
		return E_REQUEST;
	}

	if (dest == ANY_SOURCE || tag == ANY_TAG){
		warning("%s: not support ANY_SOURCE or ANY_TAG for send request\n", __func__);
		return E_REQUEST;
	}

	ret = TCP_Isend(dest, buffer, size, tag, &request);
	if (ret != SUCCESS)
		return ret;

	ret = TCP_Wait(&request, &status);
	
	return ret;
}

/*
 * TCP_Recv()
 * 
 * Blocking reveive of one region of data.
 */
int TCP_Recv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Status *status)
{
	int ret;
	Request request;

	if (buffer == NULL)
		error("%s: null buffer pointer\n", __func__);
	if (status == NULL)
		error("%s: null request pointer\n", __func__);
	if (size == 0){
		warning("%s: no data be be received\n", __func__);
		return E_REQUEST;
	}
	
	ret = TCP_Irecv(src, buffer, size, tag, &request);
	if (ret != SUCCESS)
		return ret;

	ret = TCP_Wait(&request, status);
	
	return ret;
}

/*
 * TCP_Isend()
 *
 * Nonblocking send of one region of data
 *
 * On success, total size of data sent will be returned.
 */
int TCP_Isend(int dest, void *buffer, comm_size_t size, comm_tag_t tag, Request *request)
{
	int ret;
	struct msg_header_t tcp_header;
	struct queue_item_t *item;
	struct pending_key_t *pending_key = NULL;
	struct network_key_t *network_key = NULL;
	struct buffer_msg_t *data = NULL;
	int cmp_size;

	if (buffer == NULL)
		error("%s: null buffer pointer\n", __func__);
	if (request == NULL)
		error("%s: null request pointer\n", __func__);
	if (size == 0){
		warning("%s: no data be be sent\n", __func__);
		return E_REQUEST;
	}

	if (dest == ANY_SOURCE || tag == ANY_TAG){
		warning("%s: not support ANY_SOURCE or ANY_TAG for send request\n", __func__);
		return E_REQUEST;
	}
	/* 
	 * Search sendq to check if there exists any pending send-request with 
	 *		the same socket 'dest'.
	 */
	network_key = Malloc(sizeof(struct network_key_t));
	network_key->network_arch = ARCH_TCP;
	network_key->socket = dest;
	network_key->tag = tag;
	
	data = Malloc(sizeof(struct buffer_msg_t));
	data->type = MSG_SEND;
	data->buffer = buffer;
	data->size = size;
	data->buffer_list = &data->buffer;
	data->size_list = &data->size;
	data->list_count = 1;
	data->iov_idx = 0;
	data->iov_len_done = 0;
	data->request_size = size;
	data->complete_size = 0;

	pthread_mutex_lock(&sendq_mutex);

	cmp_size = sizeof(network_key->network_arch) + sizeof(network_key->socket);
	item = search_queue(&sendq[SENDQ_INPROGRESS], network_key, cmp_size);

	if (item != NULL){
		/* 
		 * Some send operation is already in the send queue. We need to queue this send request 
		 * in order not to ruin the sending order.
		 */
		insert_queue(&sendq[SENDQ_INPROGRESS], network_key, data);
		
		*request = uuid_gen();
		pending_key = Malloc(sizeof(struct pending_key_t));
		pending_key->id = *request;
		pending_key->status.count = size;
		pending_key->status.source = dest;
		pending_key->status.tag = tag;
		pending_key->status.error = STATUS_INPROGRESS;
		pending_key->network_key = network_key;

		data->status = &pending_key->status;
		
		insert_queue(&sendq[SENDQ_PENDING], pending_key, NULL);
		
		pthread_mutex_unlock(&sendq_mutex);

		return SUCCESS;
	}
	
	/* 
	 * If we go here, no send request with same 'dest' is in sendq.
	 * We are safe to send the message.
	 */
	tcp_header.magic_nr = COMM_MAGIC_NR;
	tcp_header.size = size;
	tcp_header.tag = tag;

	/* In order to make the header are fully transferred, Use blocking send to send it */
	ret = sock_bsend(dest, &tcp_header, TCP_HEADER_SIZE);
	if (ret != TCP_HEADER_SIZE)
		error("internal error\n");
	
	/* The data payload are sent by nonblocking send */
	ret = sock_nbsend(dest, buffer, size);
	
	if (ret < 0){
		error("%s: internel error\n", __func__);
	}
	
	/* save the status of this send request */
	*request = uuid_gen();
	debug(DEBUG_NETWORK, "%s: post a send request %lld with size %d tag %lld\n", 
			__func__, *request, size, tag);
	
	pending_key = Malloc(sizeof(struct pending_key_t));
	pending_key->id = *request;
	pending_key->type = MSG_SEND;
	pending_key->network_key = network_key;
	pending_key->status.count = size;
	pending_key->status.source = dest;
	pending_key->status.tag = tag;

	if (ret == size){
		/* All data are transferred */
		free(network_key);
		free(data);
		pending_key->status.error = STATUS_COMPLETE;
	}
	else{
		/* Not complete and queue it. */
		pending_key->status.error = STATUS_INPROGRESS;

		data->complete_size = ret;
		data->iov_len_done = ret;
		data->status = &pending_key->status;
		
		insert_queue(&sendq[SENDQ_INPROGRESS], network_key, data);
	}

	insert_queue(&sendq[SENDQ_PENDING], pending_key, NULL);
	pthread_mutex_unlock(&sendq_mutex);
	
	return SUCCESS;
}

/*
 * TCP_Irecv()
 *
 * Nonblocking reveive of one region of data.
 */
int TCP_Irecv(int src, void *buffer, comm_size_t size, comm_tag_t tag, Request *request)
{
	struct queue_item_t *item;
	struct pending_key_t *pending_key = NULL;
	struct network_key_t *network_key = NULL;
	struct buffer_msg_t *data=NULL, *user_data=NULL;
	comm_size_t total_size=0, size_remain=0;
	
	if (buffer == NULL)
		error("%s: null buffer pointer\n", __func__);
	if (request == NULL)
		error("%s: null request pointer\n", __func__);
	if (size == 0){
		warning("%s: no data be be received\n", __func__);
		return E_REQUEST;
	}

	/* 
	 * Search recvq to find first-match completing recv-request with 
	 *		the same socket 'src' and tag 'tag'.
	 */
	network_key = Malloc(sizeof(struct network_key_t));
	network_key->network_arch = ARCH_TCP;
	network_key->socket = src;
	network_key->tag = tag;
	
	user_data = Malloc(sizeof(struct buffer_msg_t));
	user_data->type = MSG_RECV;
	user_data->buffer = buffer;
	user_data->size = size;
	user_data->buffer_list = &user_data->buffer;
	user_data->size_list = &user_data->size;
	user_data->list_count = 1;
	user_data->iov_idx = 0;
	user_data->iov_len_done = 0;
	user_data->request_size = size;
	user_data->complete_size = 0;

	total_size = size;
	
	/* quickly search for complete queue once to fill in buffer */
	pthread_mutex_lock(&recvq_mutex);
	do {
		/* find if there is any complete message with the same src and tag */
		if (network_key->socket == ANY_SOURCE || network_key->tag == ANY_TAG){
			item = search_queue_cmp(&recvq[RECVQ_COMPLETE], network_key, ANY_cmp);

			if (item != NULL){
				/* 
				 * When we find first-match message, we need to change values of ANY_SOURCE and ANY_TAG. 
				 * So that next time we can dedicate to receive the messages of these new src and tag values.
				 */
				if (network_key->socket == ANY_SOURCE)
					network_key->socket = ((struct network_key_t *)item->key)->socket;
				if (network_key->tag == ANY_TAG)
					network_key->tag = ((struct network_key_t *)item->key)->tag;
			}
		}
		else
			item = search_queue(&recvq[RECVQ_COMPLETE], network_key, sizeof(struct network_key_t));
		
		if (item != NULL){	/* find one completing recv-request */
			data = (struct buffer_msg_t *)item->data;
			
			size_remain = data->complete_size - data->request_size;
			
			if (total_size > size_remain){
				/* we are safe to take the receive message and remove it form complete list */
				move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
						user_data->complete_size, size_remain, data);

				user_data->complete_size += size_remain;
				total_size -= size_remain;

				/* the data buffer has been copied, we are safe to free it */
				free(data->buffer);
				free(data);

				remove_queue(item);
				delete_item(item);
			}
			else if (total_size == size_remain){
				move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
						user_data->complete_size, total_size, data);
				
				user_data->complete_size += size_remain;
				total_size = 0;
				/* the data buffer has been copied, we are safe to free it */
				free(data->buffer);
				free(data);
				
				remove_queue(item);
				delete_item(item);
			}
			else {
				move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
						user_data->complete_size, total_size, data);
				
				user_data->complete_size += total_size;
				total_size = 0;
				/* we only used part of the data buffer, so reset its status */
			}
		}

	} while (item != NULL && total_size != 0);
	
	/* No other matched message in the completion queue, just keep track it in the pending queue */
	*request = uuid_gen();
	debug(DEBUG_NETWORK, "%s: post a receive request %lld with size %d tag %lld\n", 
			__func__, *request, size, tag);
	
	pending_key = Malloc(sizeof(struct pending_key_t));
	pending_key->id = *request;
	pending_key->type = MSG_RECV;
	pending_key->network_key = network_key;
	pending_key->status.count = user_data->request_size;
	pending_key->status.source = network_key->socket;
	pending_key->status.tag = network_key->tag;

	if (total_size == 0){
		pending_key->status.error = STATUS_COMPLETE;
		free(network_key);
		free(user_data);
	}
	else
		pending_key->status.error = STATUS_INPROGRESS;

	user_data->status = &pending_key->status;
	
	insert_queue(&recvq[RECVQ_PENDING], pending_key, user_data);

	pthread_mutex_unlock(&recvq_mutex);

	return SUCCESS;
}

int TCP_Send_list(int dest, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag)
{
	int i, ret;
	comm_size_t total_size = 0;
	Request request;
	Status status;
	
	if (buffer_list == NULL)
		error("%s: null buffer list pointer\n", __func__);
	if (size_list == NULL)
		error("%s: null size list pointer\n", __func__);

	if (dest == ANY_SOURCE || tag == ANY_TAG){
		warning("%s: not support ANY_SOURCE or ANY_TAG for send request\n", __func__);
		return E_REQUEST;
	}

	for (i=0; i< list_count; i++)
		total_size += size_list[i];
	
	if (total_size == 0){
		warning("%s: no data be be sent\n", __func__);
		return E_REQUEST;
	}

	ret = TCP_Isend_list(dest, buffer_list, size_list, list_count, tag, &request);
	if (ret != SUCCESS)
		return ret;

	ret = TCP_Wait(&request, &status);
	
	return ret;
}

int TCP_Recv_list(int src, void **buffer_list, comm_size_t *size_list, int list_count, comm_tag_t tag, Status *status)
{
	int ret, i;
	comm_size_t total_size = 0;
	Request request;

	if (buffer_list == NULL)
		error("%s: null buffer list pointer\n", __func__);
	if (size_list == NULL)
		error("%s: null size list pointer\n", __func__);
	if (status == NULL)
		error("%s: null request pointer\n", __func__);

	for (i=0; i<list_count; i++)
		total_size += size_list[i];

	if (total_size == 0){
		warning("%s: no data be be received\n", __func__);
		return E_REQUEST;
	}
	
	ret = TCP_Irecv_list(src, buffer_list, size_list, list_count, tag, &request);
	if (ret != SUCCESS)
		return ret;
	
	ret = TCP_Wait(&request, status);
	
	return ret;
}

/*
 * TCP_Isend_list()
 *
 * Nonblocking send of a list of data regions
 *
 * On success, total size of data sent will be returned.
 */
int TCP_Isend_list(int dest, void **buffer_list, comm_size_t *size_list, 
		int list_count, comm_tag_t tag, Request *request)
{
	int i, ret;
	struct msg_header_t tcp_header;
	struct iovec io_vector[MAX_TCP_IOV_COUNT+1];
	struct queue_item_t *item;
	struct pending_key_t *pending_key = NULL;
	struct network_key_t *network_key = NULL;
	struct buffer_msg_t *data = NULL;
	comm_size_t total_size=0;
	int cmp_size;

	if (buffer_list == NULL)
		error("%s: null buffer list pointer\n", __func__);
	if (size_list == NULL)
		error("%s: null size list pointer\n", __func__);
	if (request == NULL)
		error("%s: null request pointer\n", __func__);

	if (dest == ANY_SOURCE || tag == ANY_TAG){
		warning("%s: not support ANY_SOURCE or ANY_TAG for send request\n", __func__);
		return E_REQUEST;
	}

	/* 
	 * Search sendq to check if there exists any pending send-request with 
	 *		the same socket 'dest'.
	 */
	for (i=0; i< list_count; i++)
		total_size += size_list[i];
	
	if (total_size == 0){
		warning("%s: no data be be sent\n", __func__);
		return E_REQUEST;
	}

	network_key = Malloc(sizeof(struct network_key_t));
	network_key->network_arch = ARCH_TCP;
	network_key->socket = dest;
	network_key->tag = tag;
	
	data = Malloc(sizeof(struct buffer_msg_t));
	data->type = MSG_SEND;
	data->buffer = NULL;
	data->size = 0;
	data->buffer_list = buffer_list;
	data->size_list = size_list;
	data->list_count = list_count;
	data->iov_idx = 0;
	data->iov_len_done = 0;
	data->request_size = total_size;
	data->complete_size = 0;

	pthread_mutex_lock(&sendq_mutex);
	cmp_size = sizeof(network_key->network_arch) + sizeof(network_key->socket);
	item = search_queue(&sendq[SENDQ_INPROGRESS], network_key, cmp_size);

	if (item != NULL){
		/* 
		 * Some send operation is already in the send queue. We need to queue this send request 
		 * in order not to ruin the sending order.
		 */
		insert_queue(&sendq[SENDQ_INPROGRESS], network_key, data);
		
		*request = uuid_gen();
		pending_key = Malloc(sizeof(struct pending_key_t));
		pending_key->id = *request;
		pending_key->status.count = total_size;
		pending_key->status.source = dest;
		pending_key->status.tag = tag;
		pending_key->status.error = STATUS_INPROGRESS;
		pending_key->network_key = network_key;

		data->status = &pending_key->status;
		
		insert_queue(&sendq[SENDQ_PENDING], pending_key, NULL);
		
		pthread_mutex_unlock(&sendq_mutex);

		return SUCCESS;
	}
	
	/* 
	 * If we go here, no send request with same 'dest' is in sendq.
	 * We are safe to send the message.
	 */
	tcp_header.magic_nr = COMM_MAGIC_NR;
	tcp_header.size = total_size;
	tcp_header.tag = tag;

	/* In order to make the header are fully transferred, Use blocking send to send it */
	ret = sock_bsend(dest, &tcp_header, TCP_HEADER_SIZE);
	if (ret != TCP_HEADER_SIZE)
		error("internal error\n");
	
	/* The data payload are sent by nonblocking send */
	for (i=0; i<list_count; i++){
		io_vector[i].iov_base = buffer_list[i];
		io_vector[i].iov_len = size_list[i];
	}
	ret = sock_nbvector(dest, io_vector, list_count, TCP_SEND);
	
	if (ret < 0){
		error("%s: internel error\n", __func__);
	}

	/* save the status of this send request */
	*request = uuid_gen();
	debug(DEBUG_NETWORK, "%s: post a send request %lld with size %d tag %lld\n", 
			__func__, *request, data->request_size, tag);
	
	pending_key = Malloc(sizeof(struct pending_key_t));
	pending_key->id = *request;
	pending_key->type = MSG_SEND;
	pending_key->network_key = network_key;
	pending_key->status.count = total_size;
	pending_key->status.source = dest;
	pending_key->status.tag = tag;
	pending_key->status.error = STATUS_INPROGRESS;

	if (ret == total_size){
		/* All data are transferred */
		free(network_key);
		free(data);
		pending_key->status.error = STATUS_COMPLETE;
	}
	else{
		/* Not complete and queue it. */
		pending_key->status.error = STATUS_INPROGRESS;

		data->complete_size = ret;
		data->iov_len_done = ret;
		data->status = &pending_key->status;

		total_size = 0;
		for (i=0; i<list_count; i++){
			total_size += size_list[i];
			if (total_size > data->complete_size){
				data->iov_idx = i;
				data->iov_len_done = size_list[i] - (total_size - data->complete_size);
				break;
			}
			else if (total_size == data->complete_size){
				data->iov_idx = i+1;
				data->iov_len_done = 0;
				break;
			}
		}

		insert_queue(&sendq[SENDQ_INPROGRESS], network_key, data);
	}

	insert_queue(&sendq[SENDQ_PENDING], pending_key, NULL);
	pthread_mutex_unlock(&sendq_mutex);
	
	return SUCCESS;
}
/*
 * TCP_Irecv_list()
 *
 * Nonblocking receive of a list of data regions
 */
int TCP_Irecv_list(int src, void **buffer_list, comm_size_t *size_list, 
		int list_count, comm_tag_t tag, Request *request)
{
	int i;
	struct queue_item_t *item;
	struct pending_key_t *pending_key = NULL;
	struct network_key_t *network_key = NULL;
	struct buffer_msg_t *data=NULL, *user_data=NULL;
	comm_size_t total_size=0, size_remain=0;
	
	if (buffer_list == NULL)
		error("%s: null buffer list pointer\n", __func__);
	if (size_list == NULL)
		error("%s: null size list pointer\n", __func__);
	if (request == NULL)
		error("%s: null request pointer\n", __func__);

	/* 
	 * Search recvq to find first-match completing recv-request with 
	 *		the same socket 'src' and tag 'tag'.
	 */
	for (i=0; i<list_count; i++)
		total_size += size_list[i];

	if (total_size == 0){
		warning("%s: no data be be received\n", __func__);
		return E_REQUEST;
	}

	network_key = Malloc(sizeof(struct network_key_t));
	network_key->network_arch = ARCH_TCP;
	network_key->socket = src;
	network_key->tag = tag;

	user_data = Malloc(sizeof(struct buffer_msg_t));
	user_data->type = MSG_RECV;
	user_data->buffer = NULL;
	user_data->size = 0;
	user_data->buffer_list = buffer_list;
	user_data->size_list = size_list;
	user_data->list_count = list_count;
	user_data->iov_idx = 0;
	user_data->iov_len_done = 0;
	user_data->request_size = total_size;
	user_data->complete_size = 0;
	user_data->status = NULL;
	
	/* quickly search for complete queue once to fill in buffer */
	pthread_mutex_lock(&recvq_mutex);
	do {
		/* find if there is any complete message with the same src and tag */
		if (network_key->socket == ANY_SOURCE || network_key->tag == ANY_TAG){
			item = search_queue_cmp(&recvq[RECVQ_COMPLETE], network_key, ANY_cmp);

			if (item != NULL){
				/* 
				 * When we find first-match message, we need to change values of ANY_SOURCE and ANY_TAG. 
				 * So that next time we can dedicate to receive the messages of these new src and tag values.
				 */
				if (network_key->socket == ANY_SOURCE)
					network_key->socket = ((struct network_key_t *)item->key)->socket;
				if (network_key->tag == ANY_TAG)
					network_key->tag = ((struct network_key_t *)item->key)->tag;
			}
		}
		else
			item = search_queue(&recvq[RECVQ_COMPLETE], network_key, sizeof(struct network_key_t));
		
		if (item != NULL){	/* find one completing recv-request */
			data = (struct buffer_msg_t *)item->data;
			
			size_remain = data->complete_size - data->request_size;
			
			if (total_size > size_remain){
				/* we are safe to take the receive message and remove it form complete list */
				move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
						user_data->complete_size, size_remain, data);

				user_data->complete_size += size_remain;
				total_size -= size_remain;

				/* the data buffer has been copied, we are safe to free it */
				free(data->buffer);
				free(data);
				
				remove_queue(item);
				delete_item(item);
			}
			else if (total_size == size_remain){
				move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
						user_data->complete_size, total_size, data);
				
				user_data->complete_size += size_remain;
				total_size = 0;
				
				/* the data buffer has been copied, we are safe to free it */
				free(data->buffer);
				free(data);

				remove_queue(item);
				delete_item(item);
			}
			else {
				move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
						user_data->complete_size, total_size, data);
				
				user_data->complete_size += total_size;
				total_size = 0;
				/* we only used part of the data buffer, so reset its status */
			}
		}

	} while (item != NULL && total_size != 0);
	
	/* No other matched message in the completion queue, just keep track it in the pending queue */
	*request = uuid_gen();
	debug(DEBUG_NETWORK, "%s: post a receive request %lld with size %d tag %lld\n", 
			__func__, *request, user_data->request_size, tag);

	pending_key = Malloc(sizeof(struct pending_key_t));
	pending_key->id = *request;
	pending_key->type = MSG_RECV;
	pending_key->network_key = network_key;
	pending_key->status.count = user_data->request_size;
	pending_key->status.source = network_key->socket;
	pending_key->status.tag = network_key->tag;

	if (total_size == 0){
		pending_key->status.error = STATUS_COMPLETE;
		free(network_key);
		free(user_data);
	}
	else
		pending_key->status.error = STATUS_INPROGRESS;

	user_data->status = &pending_key->status;

	insert_queue(&recvq[RECVQ_PENDING], pending_key, user_data);
	
	pthread_mutex_unlock(&recvq_mutex);

	return SUCCESS;
}

/*
 * check_new_connection()
 *
 * Check if there is any new connection.
 *
 * Return 0 if no any new connection. Otherwise, return 1.
 */
int check_new_connection()
{
	struct service_list_t *service;
	list_t *l;


	list_for_each(l, &service_list.list)
	{
		service = (struct service_list_t *)l;
		if (net_accept(service->server.socket) > 0)
			return 1;
	}
	
	return 0;
}

/*
 * TCP_Probe()
 *
 * Blocking probe for incoming request.
 */
int TCP_Probe(int src, comm_tag_t tag, Status *status)
{
	struct queue_item_t *item;
	struct network_key_t key_s;	/* key value for search */
	struct network_key_t *network_key = NULL;
	struct buffer_msg_t *data = NULL;
	
	if (status == NULL)
		error("%s: null status pointer\n", __func__);

	/* 
	 * Search recvq to find first-match completing recv-request with 
	 *		the same socket 'src' and tag 'tag'.
	 */
	key_s.network_arch = ARCH_TCP;
	key_s.socket = src;
	key_s.tag = tag;
	
	while (1){
		pthread_mutex_lock(&recvq_mutex);
		
		/* quick search for complete queue */
		if (key_s.socket == ANY_SOURCE || key_s.tag == ANY_TAG)
			item = search_queue_cmp(&recvq[RECVQ_COMPLETE], &key_s, ANY_cmp);
		else
			item = search_queue(&recvq[RECVQ_COMPLETE], &key_s, sizeof(struct network_key_t));
			
		if (item != NULL){
			network_key = (struct network_key_t *)item->key;
			data = (struct buffer_msg_t *)item->data;
			
			status->count = data->size;
			status->source = network_key->socket;
			status->tag = network_key->tag;
			status->error = STATUS_COMPLETE;
			
			pthread_mutex_unlock(&recvq_mutex);
			return SUCCESS;
		}
		
		/* quick search for in progress queue */
		if (key_s.socket == ANY_SOURCE || key_s.tag == ANY_TAG)
			item = search_queue_cmp(&recvq[RECVQ_INPROGRESS], &key_s, ANY_cmp);
		else
			item = search_queue(&recvq[RECVQ_INPROGRESS], &key_s, sizeof(struct network_key_t));
			
		if (item != NULL){
			network_key = (struct network_key_t *)item->key;
			data = (struct buffer_msg_t *)item->data;
			
			status->count = data->size;
			status->source = network_key->socket;
			status->tag = network_key->tag;
			status->error = STATUS_INPROGRESS;
				
			pthread_mutex_unlock(&recvq_mutex);
			return SUCCESS;
		}
		
		pthread_mutex_unlock(&recvq_mutex);

		/* sleep for a while */
		usleep(10);
	}
}

/*
 * TCP_Iprobe()
 *
 * Nonblocking probe for incoming request.
 */
int TCP_Iprobe(int src, comm_tag_t tag, int *flag, Status *status)
{
	struct queue_item_t *item;
	struct network_key_t key_s;	/* key value for search */
	struct network_key_t *network_key = NULL;
	struct buffer_msg_t *data = NULL;
	
	if (flag == NULL)
		error("%s: null error pointer\n", __func__);
	if (status == NULL)
		error("%s: null status pointer\n", __func__);

	/* 
	 * Search recvq to find first-match completing recv-request with 
	 *		the same socket 'src' and tag 'tag'.
	 */
	key_s.network_arch = ARCH_TCP;
	key_s.socket = src;
	key_s.tag = tag;
	
	pthread_mutex_lock(&recvq_mutex);
		
	/* quick search for complete queue */
	if (key_s.socket == ANY_SOURCE || key_s.tag == ANY_TAG)
		item = search_queue_cmp(&recvq[RECVQ_COMPLETE], &key_s, ANY_cmp);
	else
		item = search_queue(&recvq[RECVQ_COMPLETE], &key_s, sizeof(struct network_key_t));
		
	if (item != NULL){
		network_key = (struct network_key_t *)item->key;
		data = (struct buffer_msg_t *)item->data;
			
		status->count = data->size;
		status->source = network_key->socket;
		status->tag = network_key->tag;
		status->error = STATUS_COMPLETE;

		*flag = 1;
			
		pthread_mutex_unlock(&recvq_mutex);
		return SUCCESS;
	}
		
	/* quick search for in progress queue */
	if (key_s.socket == ANY_SOURCE || key_s.tag == ANY_TAG)
		item = search_queue_cmp(&recvq[RECVQ_INPROGRESS], &key_s, ANY_cmp);
	else
		item = search_queue(&recvq[RECVQ_INPROGRESS], &key_s, sizeof(struct network_key_t));
		
	if (item != NULL){
		network_key = (struct network_key_t *)item->key;
		data = (struct buffer_msg_t *)item->data;
			
		status->count = data->size;
		status->source = network_key->socket;
		status->tag = network_key->tag;
		status->error = STATUS_INPROGRESS;

		*flag = 1;
			
		pthread_mutex_unlock(&recvq_mutex);
		return SUCCESS;
	}
		
	pthread_mutex_unlock(&recvq_mutex);

	*flag = 0;

	return SUCCESS;
}

int TCP_Test(Request *request, Status *status)
{
	if (request == NULL)
		error("%s: null request pointer\n", __func__);
	if (status == NULL)
		error("%s: null status pointer\n", __func__);

	return SUCCESS;
}

/*
 * TCP_Wait()
 *
 * Blocking wait for completion of receive request.
 *
 */
int TCP_Wait(Request *request, Status *status)
{
	int flag = 0;
	struct network_key_t *network_key;
	struct pending_key_t *pending_key;
	struct queue_item_t *item = NULL;
	struct queue_item_t *request_item = NULL;
	struct buffer_msg_t *data = NULL, *user_data = NULL;
	comm_size_t size_remain=0, total_size=0;

	if (request == NULL)
		error("%s: null request pointer\n", __func__);
	if (status == NULL)
		error("%s: null status pointer\n", __func__);
	
	/* 
	 * Check if this request is a send operation 
	 */
	do {
		pthread_mutex_lock(&sendq_mutex);
		if (request_item == NULL)
			request_item = search_queue(&sendq[SENDQ_PENDING], request, sizeof(*request));
		
		if (request_item != NULL){
			flag = 1;
			pending_key = (struct pending_key_t *)request_item->key;

			if (pending_key->status.error == STATUS_COMPLETE){
				memcpy(status, &pending_key->status, sizeof(Status));
				
				remove_queue(request_item);
				delete_item(request_item);
				pthread_mutex_unlock(&sendq_mutex);

				return SUCCESS;
			}
		}
		pthread_mutex_unlock(&sendq_mutex);

		usleep(10);
	} while (flag == 1);
		
	
	/* 
	 * Check if this request is a recv operation 
	 */
	do {
		pthread_mutex_lock(&recvq_mutex);

		if (request_item == NULL)
			request_item = search_queue(&recvq[RECVQ_PENDING], request, sizeof(*request));

		if (request_item != NULL){
			pending_key = (struct pending_key_t *)request_item->key;

			if (pending_key->status.error == STATUS_COMPLETE){
				/* this request is finished inflight */
				memcpy(status, &pending_key->status, sizeof(Status));
				
				remove_queue(request_item);
				delete_item(request_item);
				pthread_mutex_unlock(&recvq_mutex);

				return SUCCESS;
			}
			
			/* 
			 * Search recvq to find first-match completing recv-request with 
			 * the same socket 'src' and tag 'tag'.
			 */
			
			network_key = pending_key->network_key;
			user_data = (struct buffer_msg_t *)request_item->data;
			total_size = user_data->request_size - user_data->complete_size;

			/* find if there is any complete message with the same src and tag */
			if (network_key->socket == ANY_SOURCE || network_key->tag == ANY_TAG){
				item = search_queue_cmp(&recvq[RECVQ_COMPLETE], network_key, ANY_cmp);
				
				if (item != NULL){
					/* 
					 * When we find first-match message, we need to change values of ANY_SOURCE and ANY_TAG. 
					 * So that next time we can dedicate to receive the messages of these new src and tag values.
					 */
					
					if (network_key->socket == ANY_SOURCE)
						network_key->socket = ((struct network_key_t *)item->key)->socket;
					if (network_key->tag == ANY_TAG)
						network_key->tag = ((struct network_key_t *)item->key)->tag;
				}
			}
			else
				item = search_queue(&recvq[RECVQ_COMPLETE], network_key, sizeof(struct network_key_t));
			
			if (item != NULL){	/* find one completing recv-request */
				data = (struct buffer_msg_t *)item->data;
				
				size_remain = data->complete_size - data->request_size;

				if (total_size > size_remain){
					/* we are safe to take the receive message and remove it form complete list */
					move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
							user_data->complete_size, size_remain, data);
					
					user_data->complete_size += size_remain;
					total_size -= size_remain;
					
					/* the data buffer has been copied, we are safe to free it */
					free(data->buffer);
					free(data);
					
					remove_queue(item);
					delete_item(item);
				}
				else if (total_size == size_remain){
					move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
							user_data->complete_size, total_size, data);
					
					user_data->complete_size += size_remain;
					total_size = 0;
					
					/* the data buffer has been copied, we are safe to free it */
					free(data->buffer);
					free(data);
					
					remove_queue(item);
					delete_item(item);
				}
				else {
					move_data(user_data->buffer_list, user_data->size_list, user_data->list_count, 
							user_data->complete_size, total_size, data);
					
					user_data->complete_size += total_size;
					total_size = 0;
					/* we only used part of the data buffer, so reset its status */
				}
			}
			if (total_size == 0){
				/* remove current request from list */
				free(network_key);
				free(user_data);
				
				status->count = user_data->request_size;
				status->source = network_key->socket;
				status->tag = network_key->tag;
				status->error = STATUS_COMPLETE;

				remove_queue(request_item);
				delete_item(request_item);

				pthread_mutex_unlock(&recvq_mutex);
				return SUCCESS;
			}
		}
		pthread_mutex_unlock(&recvq_mutex);
			
		usleep(10);

	} while (request_item != NULL);

	warning("no information for this request\n");
	return E_REQUEST;
}

/* 
 * Close a socket connection 
 */
int TCP_Close(int sock)
{
	struct queue_item_t *item = NULL;
	struct network_key_t key;
	struct pollfd *tmp = NULL;
	int key_size;
	int i;
	list_t *l;
	
	key.network_arch = ARCH_TCP;
	key.socket = sock;
	key_size = sizeof(key.network_arch) + sizeof(key.socket);

	pthread_mutex_lock(&sendq_mutex);
	pthread_mutex_lock(&recvq_mutex);

	/* 
	 * search socket queue 
	 */
	item = search_queue(&socketq, &key, key_size);
	if (item)
	{
		/* Remove record in socketq, recv_pollfd and ipq */
		remove_queue(item);
		delete_item(item);

		for (i=0; i<socket_count; i++)
			if (recv_pollfd[i].fd == sock)
				break;

		if (socket_count == 1)
		{
			free(recv_pollfd);
			socket_count = 0;
		}
		else
		{
			tmp = Malloc((socket_count-1) * sizeof(struct pollfd));
			if (i != 0)
				memcpy(tmp, recv_pollfd, i * sizeof(struct pollfd));
			if (i != socket_count-1)
				memcpy(&tmp[i], &recv_pollfd[i+1], (socket_count-i-1)*sizeof(struct pollfd));
			free(recv_pollfd);
			recv_pollfd = tmp;

			socket_count--;
		}
		/* search iqp */
		list_for_each(l, &ipq.list)
			if (((struct ipaddr_t *)l)->sock == sock)
				break;
		list_del(l);

		pthread_mutex_unlock(&recvq_mutex);
		pthread_mutex_unlock(&sendq_mutex);
		
		sock_close(sock);
		return SUCCESS;
	}
	
	/* 
	 * search service of server 
	 */
	list_for_each(l, &service_list.list)
		if (((struct service_list_t *)l)->server.socket == sock)
			break;
	list_del(l);

	list_for_each(l, &ipq.list)
		if (((struct ipaddr_t *)l)->sock == sock)
			break;
	list_del(l);
		
	pthread_mutex_unlock(&recvq_mutex);
	pthread_mutex_unlock(&sendq_mutex);

	sock_close(sock);
	return SUCCESS;
}

/*
 * TCP_Cancel()
 *
 * Cancel a processing of a job
 */
int TCP_Cancel()
{
	return SUCCESS;
}

/*
 * move_data()
 *
 * Copy data from receive message to a list of buffers.
 * 
 * buffer_list:		pointers to a list of buffers
 * size_list:		poniters to a list of sizes of buffers
 * list_count:		count of buffer list
 * old_size:		data size had previously filled in
 * request_size:	data size requested in this copy operation
 * data:			data structure describing the receive message
 */
static void move_data(void **buffer_list, comm_size_t *size_list, int list_count, comm_size_t old_size, 
		comm_size_t request_size, struct buffer_msg_t *data)
{
	int i, index;
	char *buf;
	int total_size=0, size_remain=0;
	
	debug(DEBUG_NETWORK, "%s: count %d old_size %d request %d\n", __func__, list_count, old_size, request_size);
	
	if (buffer_list == NULL)
		error("null buffer_list pointer\n");
	if (size_list == NULL)
		error("null size_list pointer\n");
	if (data == NULL)
		error("null data pointer\n");

	if (request_size == 0){
		warning("copy zero size. Nothing to do.\n");
		return;
	}
	
	for (i=0; i<list_count; i++){
		total_size += size_list[i];
		if (total_size > old_size){
			/* fill in the remaining size in this buffer index */
			size_remain = total_size - old_size;
			
			buf = (char *)data->buffer;
			if (size_remain >= request_size){
				memcpy(buffer_list[i], &buf[data->request_size], request_size);
				data->request_size += request_size;
				request_size = 0;
			}
			else{
				memcpy(buffer_list[i], &buf[data->request_size], size_remain);
				data->request_size += size_remain;
				request_size -= size_remain;
			}
			break;
		}
		else if (total_size == old_size)
			break;
	}

	if (request_size == 0)
		return;
	
	index = i+1;

	for (i=index; i<list_count; i++){
		buf = (char *)data->buffer;
		if (request_size >= size_list[i]){
			memcpy(buffer_list[i], &buf[data->request_size], size_list[i]);
			data->request_size += size_list[i];
			request_size -= size_list[i];
		}
		else{
			memcpy(buffer_list[i], &buf[data->request_size], request_size);
			data->request_size += request_size;
			request_size = 0;
		}
		if (request_size == 0)
			break;
	}
}

/*
 * TCP_GetAddress()
 *
 * Get IP address with indicated sock number
 *
 * On success, the IP address is returned, otherwise, return NULL
 */
char *TCP_GetAddress(int sock)
{
	struct ipaddr_t *ipaddr;
	char *ip = NULL;
	list_t *l;

	pthread_mutex_lock(&recvq_mutex);
	list_for_each(l, &ipq.list)
	{
		ipaddr = (struct ipaddr_t *)l;
		if (ipaddr->sock == sock)
			ip = ipaddr->ip;
	}
	pthread_mutex_unlock(&recvq_mutex);

	return ip;
}
/*
 * vim: sts=4 ts=4 sw=4
 */
