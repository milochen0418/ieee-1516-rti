#ifndef __MS_FUNCS_H
#define __MS_FUNCS_H

#include "ms-types.h"

class MS_MsgHandler
{
	public:
		virtual void On_RO_Receiving(char *buffer)=0;
		virtual void On_TSO_Receiving(char *buffer)=0;
		virtual void On_Event_Receiving(char *buffer)=0;
};

class MS_Communicator
{
	private:
		MS_MsgHandler *MsgHandler;
		pthread_t Msgthread;
		pthread_t ROthread;
		pthread_t Eventthread;
		
		char *federationName;	/* currently joined federation name */
		

		/* FIXME: if we try connecting to more ports? */
		char ip[80];
		int port;

	public:
		MS_Communicator(void){}
		~MS_Communicator(void){}
		
		/* Initialize basic data structure and functionalities */
		int Init(MS_MsgHandler *MsgHandler);
		
		/* Join to specific federation and build connections to other local federates */
		int Join(char *federationName, federateID *handle);
		
		/* Leave from current federation */
		int Leave(void);
		
		/* Get federate handle */
		int GetNodeID(federateID *handle);
		
		/* Query total amount of currently joined federation */
		int GetNodeCounts(int *count);
		
		/* Send command to local fedex */
		int SendCommand(int op, void **buffer_list, uint32_t *size_list, int count);

		/* Receive size of next message */
		int RecvCommandSize(uint32_t *size);
		/* Receive message */
		int RecvCommand(void *buffer,uint32_t size);
		
		int SendToNode(int dest, void **buffer_list, uint32_t *size_list, int count);

#if 0
		/* Synchronization */
		int Barrier();

		/* send message to specific federate with/without tag */
		int SendToNode(federateID dest, Msg_Info info, void *msg);
		int SendToNode(federateID dest, Msg_Info info, void *msg, int tag);

		/* send message to federates with the same channel with/without tag*/
		int SendToGroup(unsigned int channelID, Msg_Info info, void *msg);

		/* Broadcast */
		int Broadcast(Msg_Info info, void *msg);
#endif
};

#endif	/* __MS_FUNCS_H */

/*
 * vim: ts=4 sts=4 sw=4
 */
