#include <pthread.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "ms-funcs.h"
#include "msgtag.h"
extern "C"
{
#include "list.h"
#include "net.h"
#include "debug.h"
}

using namespace std;

#define Delay_RO 100
#define Delay_Msg 100
#define MAX_FEDERATE 100

int process[MAX_FEDERATE];

federateID handle;      /* federate handle */

pthread_mutex_t RO_mutex = PTHREAD_MUTEX_INITIALIZER;

struct network_fns *network = NULL;
struct host_addr_t vserver;
int MSG_RANK;

struct MsgData
{
	list_t list;
	char *msg;
};
static struct MsgData ROhead;

void *ROrun(void *ptr)
{
	struct MsgData *data;
	list_t *l, *n;
 
	MS_MsgHandler *handler;
	handler=(MS_MsgHandler *)ptr;
    while(true)
    {
		pthread_mutex_lock( &RO_mutex );
		list_for_each_safe(l, n, &ROhead.list)
		{
			data = (struct MsgData*)l;
			handler->On_RO_Receiving(data->msg);
			list_del(&data->list);
			
			delete [] data->msg;
			delete data;
		}
		pthread_mutex_unlock( &RO_mutex );
		
		usleep(Delay_RO);
    }
}

void *Msgrun(void *ptr)
{
    Status status;
    int flag;


	
    while(true)
    {
		pthread_testcancel();
		network->Iprobe(ANY_SOURCE, ANY_TAG, &flag, &status);
		
		if(flag == 1)
		{
			if(status.tag == USRMSG)
			{
				struct MsgData *temp;
				
				temp = new struct MsgData;
				temp->msg = new char[status.count];
				
				network->Recv(status.source,temp->msg, status.count,status.tag, &status);
				
				INIT_LIST_HEAD(&temp->list);
				list_add_tail(&temp->list,&ROhead.list);
			}
			else if(status.tag == CONNECTTAG)
			{
				uint32_t temp_id;
				network->Recv(status.source,&temp_id, status.count, status.tag, &status);
				process[temp_id]=status.source;
				//printf("fed %d connect\n",temp_id);
			}
				
		}
		usleep(Delay_Msg);
    }
}

int MS_Communicator::Init(MS_MsgHandler *MsgHandler)
{
	if (MsgHandler == NULL)
		error("%s: Null MsgHandler pointer.", __func__);
	
	this->MsgHandler = MsgHandler;
	
	INIT_LIST_HEAD(&ROhead.list);
	
	return 1;
} 	   

int MS_Communicator::Join(char *federationName, federateID *h)
{
	int ret;
	char filename[80]="server.config";
	char buf[80];
	char buf2[80];
	ifstream ff(filename);
	Status status;

	if (federationName == NULL)
		error("%s: federateionName is not provided.", __func__);
	if (h == NULL)
		error("%s: Null handle pointer.", __func__);
	
	if(!ff)
	    printf("not read file %s\n", filename);
	
	while(ff)
	{
	    ff.getline(buf,80,' ');
	    ff.getline(buf2,80,'\n');

	    if(strcasecmp(buf,"ip") == 0)
	        strcpy(ip,buf2);
	    else if(strcasecmp(buf,"port") == 0)
			port=atoi(buf2);
	}
	
	/*join network*/
	memset(&vserver, 0, sizeof(struct host_addr_t));
	vserver.hostname = strdup(ip);
	vserver.port = port;
	vserver.socket = -1;
	
	network = setup_network_systems();
	int sock=-1;
	uint32_t try_port=10000;
	
	while(sock==-1)
	{
		try_port++;
		sock=network->Initialize(NULL,try_port , INIT_VSERVER);
	}
	//printf("init port %d\n",try_port);
		
	while (1)
	{
		network->Connect(&vserver);
		if (vserver.is_connected == 1)
			break;
		usleep(100);
	}
	
	/* rewrite send handle not id */
	network->Send(vserver.socket, &try_port ,sizeof(uint32_t), JOINTAG);
	
	network->Recv(vserver.socket, &handle.vid, sizeof(uint32_t), JOINTAG, &status);
	network->Recv(vserver.socket, &handle.fid, sizeof(uint32_t), JOINTAG, &status);
	h->vid=handle.vid;
	h->fid=handle.fid;
	
	uint32_t node_count,i;
	connect_t temp_node;
	network->Recv(vserver.socket,&node_count,sizeof(uint32_t),UPDATETAG, &status);
	
	for(i=0;i<node_count;i++)
	{
		network->Recv(vserver.socket,&temp_node,sizeof(connect_t),UPDATETAG, &status);
		//printf("update id =%d ip=%s port=%d \n",temp_node.id,temp_node.address,temp_node.port);
		struct host_addr_t cserver;
		memset(&cserver, 0, sizeof(struct host_addr_t));
		cserver.hostname = strdup(temp_node.address);
		cserver.port = temp_node.port;
		cserver.socket = -1;
		while(1)
		{
		       network->Connect(&cserver);
		       if (cserver.is_connected == 1)
		             break;
			   usleep(100);
		}
		process[temp_node.id]=cserver.socket;
		network->Send(cserver.socket, &handle.fid ,sizeof(uint32_t),CONNECTTAG);
	}
	
	
	if ((ret = pthread_create( &ROthread, NULL, ROrun, (void *)MsgHandler)) != 0)
		error("%s: thread creation failed.", __func__);

	if ((ret = pthread_create( &Msgthread, NULL, Msgrun, NULL)) != 0)
		error("%s: thread creation failed.", __func__);

	return 1;
}

int MS_Communicator::Leave()
{
	int ret;
	
	ret = pthread_cancel(Msgthread);
   	network->Send(vserver.socket, &handle.fid, sizeof(uint32_t), LEAVETAG);

	return 1;
} 	   

int MS_Communicator::SendCommand(int op, void **buffer_list, uint32_t *size_list, int count)
{
	if (buffer_list == NULL)
		error("%s: Null buffer_list pointer.", __func__);
	if (size_list == NULL)
		error("%s: Null size_list pointer.", __func__);
	if (count == 0)
	{
		warning("%s: no data to process.", __func__);
		return 0;
	}
	
	network->Send_list(vserver.socket, buffer_list, size_list, count, FEDEX);
	
	return 1;
}
int MS_Communicator::SendToNode(int dest, void **buffer_list, uint32_t *size_list, int count)
{
	if (buffer_list == NULL)
		error("%s: Null buffer_list pointer.", __func__);
	if (size_list == NULL)
		error("%s: Null size_list pointer.", __func__);
	if (count == 0)
	{
		warning("%s: no data to process.", __func__);
		return 0;
	}

	network->Send_list(process[dest], buffer_list, size_list, count, USRMSG);

	return 1;
}
int MS_Communicator::RecvCommandSize(uint32_t *size)
{
	Status status;

	if (size == NULL)
		error("%s: Null size pointer.", __func__);
	
	network->Probe(vserver.socket, FEDEX, &status);
	*size=status.count;
	
	return 1;
}

int MS_Communicator::RecvCommand(void *buffer, uint32_t size)
{
	Status status;

	if (buffer == NULL)
		error("%s: Null buffer pointer.", __func__);
	if (size == 0)
	{
		warning("%s: no data to process.", __func__);
		return 0;
	}
	
	network->Recv(vserver.socket, buffer, size, FEDEX, &status);
	
	return 1;
}


/*
 * vim: ts=4 sts=4 sw=4
 */
