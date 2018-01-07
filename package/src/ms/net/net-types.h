#ifndef __NETWORK_TYPES_H
#define __NETWORK_TYPES_H

#include <stdint.h>
#include <sys/types.h>
#include "list.h"

typedef uint32_t comm_size_t;
typedef int64_t comm_tag_t;

#define COMM_MAGIC_NR	0x4182

#define ANY_SOURCE	-1
#define ANY_TAG		-2

/* API return values */
#define SUCCESS		0
#define INPROGRESS	-1
#define E_REQUEST	-2

enum network_arch
{
	ARCH_TCP = 0x1,
	ATCH_IB  = 0x2,
};

enum
{
    INIT_VSERVER  = 0x1,
    INIT_FEDERATE = 0x2,
};

enum sendq_type
{
	SENDQ_INPROGRESS = 0,
	SENDQ_PENDING,
	SENDQ_MAX,
};

enum recvq_type
{
	RECVQ_INPROGRESS = 0,
	RECVQ_COMPLETE,
	RECVQ_PENDING,
	RECVQ_MAX,
};

/* server address struct */
struct host_addr_t
{
    char *hostname;		/* NOT used in TCP implementation */
	int port;
	int socket;
	int is_connected;
};

struct msg_header_t
{
	uint32_t magic_nr;	/* magic number */
	comm_size_t size;	/* payload size */
	comm_tag_t tag;		/* tag */
};

enum request_type
{
	MSG_SEND=0,
	MSG_RECV,
};

enum request_status
{
	STATUS_COMPLETE		= 0,
	STATUS_INPROGRESS	= 1,
	STATUS_ERROR		= 2,
};

/* Struct for nonblocking operation */
typedef uint64_t	Request;

typedef struct Status
{ 
    comm_size_t count;
	int source;
	comm_tag_t tag;
	int error;
} Status;

struct network_key_t
{
	int network_arch;	/* enum network_arch */
	int socket;
	comm_tag_t tag;
};

struct pending_key_t
{
	uint64_t id;
	int type;			/* enum request_type */
	Status status;		/* only useful for nonblocking send operations */
	struct network_key_t *network_key;
};

/*
 * server information
 */
struct server_addr_t
{
	/* TCP/IP */
	char *hostname;
	int port;
	int socket;
	int is_initialized;
};

#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
