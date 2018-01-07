#ifndef __SOCKIO_H
#define __SOCKIO_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum {
    TCP_SEND = 0,
    TCP_RECV,
};

int sockio_new_sock();
int sockio_bind_sock(int sockd, char *hostname, int port);
int sockio_init_sock(struct sockaddr *saddrp, const char *hostname, int port);
int sockio_connect_sock(int sockd, const char *hostname, int port);
int sockio_brecv(int s, void *buf, int len);
int sockio_nbrecv(int s, void *buf, int len);
int sockio_bsend(int s, void *buf, int len);
int sockio_nbsend(int s, void *buf, int len);
int sockio_nbvector(int s, struct iovec* vector, int count, int send_recv);
int sockio_nbpeek(int s, void *buf, int len);
int sockio_set_tcpopt(int s, int optname, int val);
int sockio_get_sockopt(int s, int optname);
int sockio_set_sockopt(int s, int optname, int val);

#endif

/*
 * vim: ts=8 sts=4 sw=4
 */
