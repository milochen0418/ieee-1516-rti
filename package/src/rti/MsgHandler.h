#ifndef _MY_MSG_HANDLER
#define _MY_MSG_HANDLER

#include "ms-funcs.h"

class MsgHandler : public MS_MsgHandler
{
	public:
		virtual void On_RO_Receiving(char *buffer);
		virtual void On_TSO_Receiving(char *buffer);
		virtual void On_Event_Receiving(char *buffer);
};

#endif 

/*
 * vim: ts=4 sts=4 sw=4
 */
