#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>

#include "debug.h"
#include "sockio.h"

/* 
 * sockio_new_sock()
 * 
 *  create an endpoint for communication 
 */
int sockio_new_sock()
{
    static int p_num = -1;
    struct protoent *pep;
    
    if (p_num == -1){
	if ((pep = getprotobyname("tcp")) == NULL){
	    perror("Kernel does not support tcp");
	    return -1;
	}
	p_num = pep->p_proto;
    }
    return (socket(AF_INET, SOCK_STREAM, p_num));
}

/*
 * sockio_bind_sock()
 *
 * bind a port to a socket
 */
int sockio_bind_sock(int sockd, char *hostname, int port)
{
    struct sockaddr_in saddr;
    
    memset((char *) &saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    
    if (hostname)
	saddr.sin_addr.s_addr = inet_addr(hostname);
    else
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

bind_sock_restart:
    if (bind(sockd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0){
	if (errno == EINTR)
	    goto bind_sock_restart;
	return -1;
    }
    return (sockd);
}

/*
 * sockio_init_sock()
 *
 * initiate socket connection data
 */
int sockio_init_sock(struct sockaddr *saddrp, const char *hostname, int port)
{
    struct hostent *hep;
    
    memset((char *) saddrp, 0, sizeof(struct sockaddr_in));
    if (hostname == NULL){
	if ((hep = gethostbyname("localhost")) == NULL)
	    return -1;
    }
    else if ((hep = gethostbyname(hostname)) == NULL)
	return -1;
	
    ((struct sockaddr_in *) saddrp)->sin_family = AF_INET;
    ((struct sockaddr_in *) saddrp)->sin_port = htons(port);
    
    memcpy(&((struct sockaddr_in *) saddrp)->sin_addr, hep->h_addr,
	    hep->h_length);

    return 0;
}

/*
 * sockio_close_sock()
 *
 * Close a socket connection.
 */
int sockio_close_sock(int sock)
{
    return close(sock);
}

/*
 * sockio_connect_sock()
 *
 * initiate a connection on a socket
 */
int sockio_connect_sock(int sock, const char *hostname, int port)
{
    struct sockaddr saddr;

    if (sockio_init_sock(&saddr, hostname, port) != 0)
	return -1;

connect_sock_restart:
    if (connect(sock, (struct sockaddr *) &saddr, sizeof(saddr)) < 0){
	if (errno == EINTR)
	    goto connect_sock_restart;
	return -1;
    }
    return sock;
}

/* 
 * sockio_brecv()
 * 
 * blocking receive
 *  
 * Returns -1 if it cannot get all len bytes and the # of bytes received otherwise
 */
int sockio_brecv(int s, void *buf, int len)
{
    int oldfl, ret, comp = len;
    int olderrno;

    /* if previous socket is nonblocking, reset it to blocking */
    oldfl = fcntl(s, F_GETFL, 0);
    if (oldfl & O_NONBLOCK)
	fcntl(s, F_SETFL, oldfl & (~O_NONBLOCK));

    while (comp){
brecv_restart:
	if ((ret = recv(s, (char *) buf, comp, 0)) < 0){
	    if (errno == EINTR)
		goto brecv_restart;
	    
	    olderrno = errno;
	    fcntl(s, F_SETFL, oldfl|O_NONBLOCK);
	    errno = olderrno;
	    return -1;
	}
	if (!ret){
	    fcntl(s, F_SETFL, oldfl|O_NONBLOCK);
	    errno = EPIPE;
	    return -1;
	}
	comp -= ret;
	buf += ret;
    }

    fcntl(s, F_SETFL, oldfl | O_NONBLOCK);
    
    return (len - comp);
}

/* 
 * sockio_nbrecv()
 * 
 * nonblocking receive 
 */
int sockio_nbrecv(int s, void *buf, int len)
{
    int ret, comp = len;

    while (comp){
nbrecv_restart:
	ret = recv(s, buf, comp, 0);
	if (!ret){  /* socket closed */
	    errno = EPIPE;
	    return -1;
	}
	if (ret == -1 && errno == EWOULDBLOCK){
	    return (len - comp);    /* return amount completed */
	}
	if (ret == -1 && errno == EINTR){
	    goto nbrecv_restart;
	}
	else if (ret == -1){
	    return -1;
	}
	comp -= ret;
	buf += ret;
    }
    return (len - comp);
}

/* 
 * sockio_bsend()
 * 
 * blocking send 
 */
int sockio_bsend(int s, void *buf, int len)
{
    int oldfl, ret, comp = len;
    int olderrno;

    /* if previous socket is nonblocking, reset it to blocking */
    oldfl = fcntl(s, F_GETFL, 0);
    if (oldfl & O_NONBLOCK)
	fcntl(s, F_SETFL, oldfl & (~O_NONBLOCK));

    while (comp){
bsend_restart:
	if ((ret = send(s, (char *) buf, comp, 0)) < 0){
	    if (errno == EINTR)
		goto bsend_restart;

	    olderrno = errno;
	    fcntl(s, F_SETFL, oldfl | O_NONBLOCK);
	    errno = olderrno;
	    return -1;
	}
	comp -= ret;
	buf += ret;
    }

    fcntl(s, F_SETFL, oldfl | O_NONBLOCK);

    return (len - comp);
}

/* 
 * sockio_nbsend()
 * 
 * nonblocking send
 *  
 * should always return 0 when nothing gets done! 
 */
int sockio_nbsend(int s, void *buf, int len)
{
    int ret, comp = len;

    while (comp){
nbsend_restart:
	ret = send(s, (char *) buf, comp, 0);
	if (ret == 0 || (ret == -1 && errno == EWOULDBLOCK))
	    return (len - comp);    /* return amount completed */
	if (ret == -1 && errno == EINTR)
	    goto nbsend_restart;
	else if (ret == -1)
	    return -1;

	comp -= ret;
	buf += ret;
    }

    return (len - comp);
}

/* 
 * BMI_sockio_nbvector()
 * 
 * Nonblocking vector send
 */
int sockio_nbvector(int s, struct iovec* vector, int count, int send_recv)
{
    int ret = 0;

    do {
	if (send_recv == TCP_RECV){
	    ret = readv(s, vector, count);
	}
	else if (send_recv == TCP_SEND){
	    ret = writev(s, vector, count);
	}
	
    } while(ret == -1 && errno == EINTR);

    /* return zero if can't do any work at all */
    if(ret == -1 && errno == EWOULDBLOCK){
	return 0;
    }

    /* if data transferred or an error */
    return ret;
}

int sockio_nbpeek(int s, void *buf, int len)
{
    int ret, comp = len;
    while (comp){
nbpeek_restart:
	ret = recv(s, buf, comp, MSG_PEEK);
	if (!ret){ /* socket closed */
	    errno = EPIPE;
	    return -1;
	}
	if (ret == -1 && errno == EWOULDBLOCK){
	    return (len - comp);
	}
	if (ret == -1 && errno == EINTR){
	    goto nbpeek_restart;
	}
	else if (ret == -1){
	    return -1;
	}
	comp -= ret;
    }
    
    return (len - comp);
}

/*
 * sockio_set_tcpopt()
 *
 * set TCP options
 */
int sockio_set_tcpopt(int s, int optname, int val)
{
    struct protoent* p = getprotobyname("tcp");
    if(!p)
	return -1;
    if (setsockopt(s, p->p_proto, optname, &val, sizeof(val)) == -1)
	return -1;
    else
	return val;
}

/* 
 * sockio_get_sockopt()
 * 
 * get socket options 
 */
int sockio_get_sockopt(int s, int optname)
{
    int val, len = sizeof(val);

    if (getsockopt(s, SOL_SOCKET, optname, &val, &len) == -1)
	return -1;
    else
	return val;
}

/*
 * sockio_set_sockopt()
 *
 * set socket options
 */
int sockio_set_sockopt(int s, int optname, int val)
{
    if (setsockopt(s, SOL_SOCKET, optname, &val, sizeof(val)) == -1)
	return -1;
    else
	return val;
}

/*
 * vim: ts=8 sts=4 sw=4
 */
