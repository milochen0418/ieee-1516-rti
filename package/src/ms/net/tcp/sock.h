#ifndef __SOCKIO_H
#define __SOCKIO_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum 
{
    TCP_SEND = 0,
    TCP_RECV,
};

int sock_create();
int sock_bind(int sockd, char *hostname, int port);
int sock_close(int sock);
int sock_connect(int sockd, const char *hostname, int port);
int sock_brecv(int s, void *buf, int len);
int sock_nbrecv(int s, void *buf, int len);
int sock_bsend(int s, void *buf, int len);
int sock_nbsend(int s, void *buf, int len);
int sock_nbvector(int s, struct iovec* vector, int count, int send_recv);
int sock_nbpeek(int s, void *buf, int len);
int sock_getsockopt(int s, int optname);
int sock_setsockopt(int s, int level, int optname, int val);

#endif

/*
 * vim: ts=8 sts=4 sw=4
 */
