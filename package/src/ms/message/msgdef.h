#ifndef MSGDEF_H
#define MSGDEF_H
#include "net.h"

#define EXITTAG 1

#define SENDTAG 4
#define RECVCONNTAG 5
#define PUBTAG 6
#define SCRTAG 7
#define UNPUBTAG 8
#define UNSCRTAG 9
#define GETPUB 10
#define GETSCR 11

#define USERBASE 1024
#define RECVSHIFT 10
#define RECVPOW 1024

#define MAXNODE 1000
#define __MSGDEBUG__

#define MSGSUCCEED 1
#define MSGFAIL 2

#define MSG_ANYSOURCE -1
#define MSG_ANYTAG -1

typedef struct
{
	int source;
	int tag;
	int size;
}MSG_Status;

typedef struct{
	int type;
	Request request;
}MSG_Request;

int VS_Init(int mode);
int VS_Recv( void *buffer,MSG_Status *stat);
int VS_Send( void *buffer,comm_size_t  size);

int VS_SendComm(int dest,void **buffer_list, uint32_t *size_list, int count);
int VS_SendMsg(int dest,void **buffer_list, uint32_t *size_list, int count);
int VS_SendMsg2(int dest,void *buffer,comm_size_t  size);

int VS_SendFedex(int dest,void **buffer_list, uint32_t *size_list, int count);
int VS_SendAllFedexs(void *buffer,comm_size_t  size);

int VS_Iprobe(int* flag,MSG_Status* stat);
int VS_Finalize();
int VS_Alive();
#endif
