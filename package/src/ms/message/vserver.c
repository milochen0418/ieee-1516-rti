#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "debug.h"
#include "msgdef.h"
#include "msgtag.h"
#include "list.h"

#define VSERVER_LISTEN_PORT	12345
#define MAXQUEUE 1000
#define MAXPUBLISH 1000

/*fedex connect fedex*/
#define ADD_SYS 200
#define UPDATE_SYS 201
#define ID_SYS 202
#define CONNECT_SYS 203
#define TEST_SYS 204
#define TOTAL_FEDEXS 10 

struct ConnectList
{
    list_t list;
    connect_t node;
};

connect_t fedex_process_info[TOTAL_FEDEXS];
int fedex_process[TOTAL_FEDEXS];
uint32_t fedex_number;
int control_mode;
uint32_t fedexID;


struct network_fns *network = NULL;

int process[MAXNODE];
int procqueue[MAXNODE];
struct ConnectList process_info;

uint32_t numproc;
uint32_t nr_proc;

typedef void* pbuffer;
typedef struct {
    int dest;
    int tag;
    comm_size_t size;
    pbuffer data;
}queuedata;

queuedata queue[MAXQUEUE];
int qhead,qtail;
int serveralive;
pthread_t  precv;


struct m_linkdata{
    int rank;
    struct m_linkdata* next;
};

typedef struct m_linkdata linkdata;

linkdata publist[MAXPUBLISH],sublist[MAXPUBLISH];

int addlist(linkdata * list,int rank,int classid);
int removelist(linkdata * list,int rank,int classid);
void freeservermem();
void printlist(linkdata * list,int classid);
int* getlist(linkdata* list,int classid,int * length);

void* VS_Init_fc(void* ipt); 

pthread_t vserver;

int VS_Finalize(){
    int pstat;
    if(serveralive==0){
	printf("VS_Init isn't called!.\n");
	return -1;
    }
    serveralive=0;
    pthread_join(vserver, (void **)&pstat);
    
    return 0;
}

int VS_Init(int mode){
    control_mode=mode;
    int i;
    for(i=0;i<TOTAL_FEDEXS;i++)
	fedex_process[i]=-1;
    pthread_create(&vserver, NULL,  VS_Init_fc, NULL);
    while(serveralive!=1)
	usleep(1000);
    return 0;
}


void init(){
    int i;
    qhead=0;
    qtail=0;
    INIT_LIST_HEAD(&process_info.list);
    for(i=0;i<MAXNODE;i++){
	process[i]=0;
	procqueue[i]=0;
    }
    for(i=0;i<MAXPUBLISH;i++){
	publist[i].next=NULL;
	publist[i].rank=-1;
	sublist[i].next=NULL;
	sublist[i].rank=-1;
    }
}

void* recvthread(void * input){

    /*
     * test multi fedex connect
    int commd;
    printf("enter command\n");
    scanf("%d",&commd);
    int i;
    for(i=0;i<TOTAL_FEDEXS;i++)
    {
	if(fedex_process[i]!=-1)
	network->Send(fedex_process[i],&fedexID,sizeof(uint32_t) , TEST_SYS);
    }
    */
    int fed,size,tag;
    Status status;
    int flag;
    int src;
    queuedata* p;
    while(serveralive==1){
	flag=0;
	network->Iprobe(ANY_SOURCE, RECVCONNTAG,&flag, &status);
	if(flag!=0){
	    fed = status.source;
	    size = status.count;
	    tag = status.tag;
	    network->Recv(fed, &src, size, RECVCONNTAG, &status);
	    procqueue[src]=fed;
	}
	if(qhead!=qtail){
	    p=&queue[qhead];
	    if(procqueue[p->dest]!=0){
		network->Send(procqueue[p->dest],p->data, p->size, p->tag);
		free(p->data);
		qhead++;
		qhead=qhead%MAXQUEUE;
	    }
	    if(procqueue[p->dest]==-2){
		free(p->data);
		qhead++;
		qhead=qhead%MAXQUEUE;
	    }
	}
	usleep(100);
    }
    while(qhead!=qtail){
	 p=&queue[qhead];
	 free(p->data);
	 qhead++;
	 qhead=qhead%MAXQUEUE;
    }
    return NULL;
}

int VS_Iprobe(int* flag,MSG_Status* stat){
    Status status;
    network->Iprobe(ANY_SOURCE, FEDEX,flag, &status);
    stat->size=status.count;
    stat->source=status.source;
    return MSGSUCCEED;
}

int VS_Recv(void *buffer,MSG_Status *stat){
    Status status;
    //int* from;
    network->Recv(stat->source, buffer, stat->size, FEDEX, &status);
    stat->size=status.count;
    stat->tag=FEDEX;
   // from=(int*)buffer;
   // stat->source=*from;
    return MSGSUCCEED;
}

int VS_Send( void *buffer,comm_size_t  size){
    int *d=(int*) buffer;
    int dest=*d;
    if(process[dest]>0){
	network->Send(process[dest], buffer, size, FEDEX);
    }
    return MSGSUCCEED;
}

int VS_SendComm(int dest,void **buffer_list, uint32_t *size_list, int count)
{
    network->Send_list(process[dest], buffer_list, size_list, count,FEDEX );
    return MSGSUCCEED;
}
int VS_SendMsg(int dest,void **buffer_list, uint32_t *size_list, int count)
{
    network->Send_list(process[dest], buffer_list, size_list, count,USRMSG );
    return MSGSUCCEED;
}
int VS_SendMsg2(int dest,void *buffer,comm_size_t  size)
{
    network->Send(process[dest],buffer,size,USRMSG);
    return MSGSUCCEED;
}

int VS_SendFedex(int dest,void **buffer_list, uint32_t *size_list, int count)
{
    if(fedex_process[dest]!=-1)
	network->Send_list(fedex_process[dest], buffer_list, size_list, count,FEDEX);
    return MSGSUCCEED;
}


int VS_SendAllFedexs(void *buffer,comm_size_t  size)
{
    int t;
    debug(DEBUG_FEDEX,"[vserver][VS_SendAllFedexs] total fedexs =%d\n",fedex_number);
    for(t=0;t<fedex_number;t++)
    {
	if(fedex_process[t]!=-1)
	{
	    debug(DEBUG_FEDEX,"[vserver][VS_SendAllFedexs] send fedex=%d \n",t);
	    network->Send(fedex_process[t],buffer,size,FEDEX);
	}
    }
    return MSGSUCCEED;
}



int VS_Alive(){
    return serveralive;
}

void* VS_Init_fc(void* ipt){
    int sock;
    Status status;
    int command;
    int fed,size,tag;
    int sendfrom;//,sendto;
    //comm_size_t sendsize;
    int pstat;
    int rank,classid;
    int* listarray;
    int listsize;
    int flag;
    queuedata* tail;
    init();   

    network = setup_network_systems();

    sock = network->Initialize(NULL, VSERVER_LISTEN_PORT, INIT_VSERVER);
    network->Initialize(NULL, VSERVER_LISTEN_PORT+1, INIT_VSERVER);
    
    /* we assign the fid of fedex to 0, so the federate's id will start from 1 */
    numproc=1;
    nr_proc=0;
    if(control_mode==1)
    {
	serveralive=1;
	fedexID=0;
	fedex_number=1;
	fedex_process_info[0].id=0;
	strcpy(fedex_process_info[0].address,network->GetAddress(sock));
    }
    else
    {
	struct host_addr_t mserver;
	memset(&mserver, 0, sizeof(struct host_addr_t));
	mserver.hostname = strdup("140.114.91.70");
	mserver.port = VSERVER_LISTEN_PORT;
        mserver.socket = -1;
	while(1)
	{
	    network->Connect(&mserver);
	    if (mserver.is_connected == 1)
	                break;
	    usleep(100);
	}
	uint32_t port=VSERVER_LISTEN_PORT;
	fedex_process[0]=mserver.socket;
	network->Send(mserver.socket,&port,sizeof(uint32_t),ADD_SYS);
	fedex_number=2;
    }
    
    pthread_create(&precv, NULL, recvthread, NULL);
    while(1){
       network->Iprobe(ANY_SOURCE, ANY_TAG, &flag,&status);
       if(flag==0){
	   usleep(100);
	   continue;
       }
       fed = status.source;
       size = status.count;
       tag = status.tag;
       if(tag==RECVCONNTAG||tag==FEDEX){
	   usleep(100);
	   continue;
       }
       //network->Recv(fed, &command, size, tag, &status);
       switch(tag){
	   case ADD_SYS:
	       if(control_mode==1)
	       {
		   network->Recv(fed, &fedex_process_info[fedex_number].port, size, tag, &status);
		   fedex_process_info[fedex_number].id=fedex_number;
		   strcpy(fedex_process_info[fedex_number].address,network->GetAddress(fed));
		   fedex_process[fedex_number]=fed; 
		   network->Send(fed,&fedex_number,sizeof(uint32_t),ID_SYS);
		   int i;
		   for(i=1;i<fedex_number;i++)
		   {
		       network->Send(fed,&fedex_process_info[i],sizeof(connect_t),UPDATE_SYS);
		   }
		   fedex_number++;  
	       }
	       break;
	   case ID_SYS:
		network->Recv(fed, &fedexID,size, tag, &status);
		serveralive=1;
	       break;
	   case UPDATE_SYS:
	       {
		   connect_t temp_node;
		   network->Recv(fed, &temp_node,size, tag, &status);
		   printf("update id =%ld ip=%s\n",(long)temp_node.id,temp_node.address);
		   
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
		   fedex_process[temp_node.id]=cserver.socket;
		   network->Send(cserver.socket,&fedexID,sizeof(uint32_t),CONNECT_SYS);
		   fedex_number++;
	       }
	       break;
	   case CONNECT_SYS:
	       {
		   int tempID;
		   network->Recv(fed, &tempID,size, tag, &status);
		   fedex_process[tempID]=fed;
		   fedex_number++;
	       }
	       break;
	   case TEST_SYS:
	       {
		   int tempID;
		   network->Recv(fed, &tempID,size, tag, &status);
		   printf("recv ID=%d\n",tempID);
	       }
	       break;
	   case EXITTAG:
	       serveralive=0;
	       break;
	   case JOINTAG:   
	       { 
	       struct ConnectList *data;
	       data=(struct ConnectList*)malloc(sizeof(struct ConnectList));
	       struct ConnectList *temp;
	       list_t *l;
	       uint32_t port;
	       INIT_LIST_HEAD(&data->list);
	       debug(DEBUG_FEDEX,"Received JOINTAG of node %d.\n",numproc);
	       network->Recv(fed, &port, size, tag, &status);
	       process[numproc]=fed;
	       
	       network->Send(fed, &fedexID, sizeof(uint32_t), JOINTAG);
	       network->Send(fed, &numproc, sizeof(uint32_t), JOINTAG);
	       network->Send(fed, &nr_proc, sizeof(uint32_t), UPDATETAG);
	       list_for_each(l, &process_info.list)
	       {
	           temp = (struct ConnectList *)l;
		   network->Send(fed,&temp->node,sizeof(connect_t),UPDATETAG);
		   debug(DEBUG_FEDEX,"fed nework id= %ld address=%s port=%ld\n",temp->node.id,temp->node.address,temp->node.port);
	       }
	       
	       /*add fedeerate network info. to process_info*/
	       data->node.id=numproc;
	       strcpy(data->node.address,network->GetAddress(fed));
	       data->node.port=port;
	       list_add_tail(&data->list, &process_info.list);
	       
	       debug(DEBUG_FEDEX,"Node %d is successfully joined.\n",numproc);
	       numproc++;
	       nr_proc++;
	       break;
	       }
	   case LEAVETAG:   
	       
#ifdef  __MSGDEBUG__
	       printf("Reveived LEAVEJAG of node %d.\n",command);
#endif
	       network->Recv(fed, &command, size, tag, &status);
	       process[command]=-1;
	       procqueue[command]=-2;
	       break;
	    case PUBTAG:
	       rank=command;
	       network->Recv(fed, &classid, sizeof(int), tag, &status);
	       addlist(publist,rank,classid);
#ifdef  __MSGDEBUG__
	       printf("Rank %d published classid %d:\n",rank,classid);
	       printf("Classid %d now:",classid);
	       printlist(publist,classid);
#endif
	       break;
	    case UNPUBTAG:
	       rank=command;
	       network->Recv(fed, &classid, sizeof(int), tag, &status);
	       removelist(publist,rank,classid);
#ifdef  __MSGDEBUG__
	       printf("Rank %d unpublished classid %d:\n",rank,classid);
	       printf("Classid %d now:",classid);
	       printlist(publist,classid);
#endif
	       break;
	    case SCRTAG:
	       rank=command;
	       network->Recv(fed, &classid, sizeof(int), tag, &status);
	       addlist(sublist,rank,classid);
#ifdef  __MSGDEBUG__
	       printf("Rank %d subscribed classid %d:\n",rank,classid);
	       printf("Classid %d now:",classid);
	       printlist(sublist,classid);
#endif
	       break;
	    case UNSCRTAG:
	       rank=command;
	       network->Recv(fed, &classid, sizeof(int), tag, &status);
	       removelist(sublist,rank,classid);
#ifdef  __MSGDEBUG__
	       printf("Rank %d unsubscribed classid %d:\n",rank,classid);
	       printf("Classid %d now:",classid);
	       printlist(sublist,classid);
#endif
	       break;
	    case GETPUB:
	       classid=command;
	       listarray=getlist(publist,classid,&listsize);
	       network->Send(fed, &listsize, sizeof(int), tag);
	       if(listsize!=0) network->Send(fed, listarray, sizeof(int)*listsize, tag);
	       break;
	    case GETSCR:
	       classid=command;
	       listarray=getlist(sublist,classid,&listsize);
	       network->Send(fed, &listsize, sizeof(int), tag);
	       if(listsize!=0) network->Send(fed, listarray, sizeof(int)*listsize, tag);
	       break;
	    default:
	       if(tag>=USERBASE){
		   sendfrom=command;
		   while(qhead==qtail+1||(qhead==0&&qtail==MAXQUEUE-1)) usleep(100);
		   tail=&queue[qtail];
		   network->Recv(fed, &tail->dest, sizeof(int), tag, &status);
		   network->Recv(fed, &tail->size, sizeof(tail->size), tag, &status);
		   tail->data=malloc(tail->size);
		   network->Recv(fed, tail->data, tail->size, tag, &status);
		   //network->Recv(fed, &sendto, sizeof(int), tag, &status);
		   //network->Recv(fed, &sendsize, sizeof(sendsize), tag, &status);
		   //network->Recv(fed, buffer, sendsize, tag, &status);
#ifdef  __MSGDEBUG__
		   printf("(TAG%d)Node %d want to send %d bytes to %d.\n",tag-USERBASE,sendfrom,tail->size,tail->dest);
#endif
		   tag-=USERBASE;
		   tail->tag=(tag<<RECVSHIFT)+sendfrom;
#ifdef  __MSGDEBUG__
		   printf("Shadow TAG=%d\n",tail->tag);
#endif
		   qtail++;
		   qtail=qtail % MAXQUEUE;
		   //network->Send(process[sendto], buffer,sendsize, (tag<<RECVSHIFT)+sendfrom); 
	       }
	       break;
	
       }
    }

#ifdef  __MSGDEBUG__
    printf("Exit tag received. Server will terminate.\n");
#endif
    freeservermem();
    pthread_join(precv, (void **)&pstat);
    return 0;
}


void printlist(linkdata * list,int classid){
    linkdata* p=&list[classid];
    p=p->next;
    while(p!=NULL){
	printf("%d ",p->rank);
	p=p->next;
    }
    printf("\n");
    return;
}

void freeservermem(){
    int i;
    linkdata* p;
    linkdata* tmp;
    for(i=0;i<MAXPUBLISH;i++){
	p=publist[i].next;
	while(p!=NULL){
	    tmp=p;
	    p=p->next;
	    free(tmp);
	}
	p=sublist[i].next;
	while(p!=NULL){
	    tmp=p;
	    p=p->next;
	    free(tmp);
	}
    }
}

int addlist(linkdata* list,int rank,int classid){
    linkdata* block;//=(linkdata*)malloc(sizeof(block));
    linkdata* p=&list[classid];
    while(p->next!=NULL){
	if(p->rank==rank) return -1;
	p=p->next;
    }
    if(p->rank==rank) return -1;
    block=(linkdata*)malloc(sizeof(block));
    block->next=NULL;
    block->rank=rank;
    p->next=block;
    return 0;
}
int removelist(linkdata* list,int rank,int classid){
    linkdata* p=&list[classid];
    linkdata* pre=p;
    p=p->next;
    while(p!=NULL){
	if(p->rank==rank){
	    pre->next=p->next;
	    free(p);
	    break;
	}else{
	    pre=p;
	    p=p->next;
	}
    }
    return 0;
}
int* getlist(linkdata* list,int classid,int * length){
    static int array[MAXNODE];
    *length=0;
    linkdata* p=list[classid].next;
    while(p!=NULL){
	array[*length]=p->rank;
	*length=*length+1;
	p=p->next;
    }
    return (int*)&array;
}

/*
 * vim: ts=8 sts=4 sw=4
 */
