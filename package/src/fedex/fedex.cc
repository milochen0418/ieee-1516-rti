#include <iostream>
#include "Endecode.h"
#include "OMT_Parser.h"
#include "PubSubTable.h"
extern "C"
{
#include "list.h"	
#include "msgdef.h"
#include "debug.h"
}

int LocalTransmit=1;

using namespace std;

extern char func_name[][50];
extern uint32_t fedex_number;

/* we assign the fid of fedex to 0, so the federate's id will start from 1 */
static uint32_t vid = 0;
static uint32_t fid = 1;

static OHT<int> * oht;
static IHT<int> * iht;
static char* FDDname=NULL;

template <class T>
class federateIDcmp
{
	    public:
		bool operator() (const T& t1,const T& t2) const
		{
		      return t1.vid==t2.vid ? t1.fid<t2.fid : t1.vid<t2.vid;
		}
};
typedef set<federateID,federateIDcmp<federateID>  > federateID_set;

typedef std::set<RTI_wstring> InstNameSet;
static InstNameSet globalObjectNames;
std::map<RTI_wstring, std::pair<int,int> > synpointmap;

std::map<RTI_wstring, std::pair<int,int> > fedex_synpointmap;
std::map<RTI_wstring, uint32_t > fedex_synpointreg;

/* OHT data list begin*/
struct DataList
{
	list_t list;
	federateID id;
};

static struct DataList *publist; 
static federateID_set *sendlist;
bool  *pubtable;

static struct DataList *Ipublist;
static federateID_set *Isendlist;
bool  *Ipubtable;

static int globalObject=1;
static bool Need_SendAllFedexs=false;

/*end*/

/* Declare all Fedex functions here */
typedef void (*func_t)(struct header_t *head, struct buffer_t *buffer);

static void Fedex_JoinFederationExecution(struct header_t *head, struct buffer_t *buffer);
static void Fedex_ResignFederationExecution(struct header_t *head, struct buffer_t *buffer);

static void Fedex_RegisterFederationSynchronizationPoint(struct header_t *head, struct buffer_t *buffer);

static void Fedex_SynchronizationPointAchieved(struct header_t *head, struct buffer_t *buffer);

static void Fedex_PublishObjectClassAttributes(struct header_t *head, struct buffer_t *buffer);
static void Fedex_UnpublishObjectClass(struct header_t *head, struct buffer_t *buffer);
static void Fedex_UnpublishObjectClassAttributes(struct header_t *head, struct buffer_t *buffer);
static void Fedex_SubscribeObjectClassAttributes(struct header_t *head, struct buffer_t *buffer);
static void Fedex_UnsubscribeObjectClass(struct header_t *head, struct buffer_t *buffer);
static void Fedex_UnsubscribeObjectClassAttributes(struct header_t *head, struct buffer_t *buffer);
static void Fedex_PublishInteractionClass(struct header_t *head, struct buffer_t *buffer);
static void Fedex_UnpublishInteractionClass(struct header_t *head, struct buffer_t *buffer);
static void Fedex_SubscribeInteractionClass(struct header_t *head, struct buffer_t *buffer);
static void Fedex_UnsubscribeInteractionClass(struct header_t *head, struct buffer_t *buffer);

/* Object Management Services */
static void Fedex_ReserveObjectInstanceName(struct header_t *head, struct buffer_t *buffer);
static void Fedex_RegisterObjectInstance(struct header_t *head, struct buffer_t *buffer);
static void Fedex_UpdateAttributeValues(struct header_t *head, struct buffer_t *buffer);
static void Fedex_SendInteraction(struct header_t *head, struct buffer_t *buffer);
static void Fedex_DeleteObjectInstance(struct header_t *head, struct buffer_t *buffer);
static void Fedex_ChangeAttributeTransportationType(struct header_t *head, struct buffer_t *buffer);
static void Fedex_ChangeInteractionTransportationType(struct header_t *head, struct buffer_t *buffer);
static void Fedex_RequestAttributeValueUpdate(struct header_t *head, struct buffer_t *buffer);

/*fedex to fedex for transmit*/
static void Fedex_TransmitReflectAttributeValues(struct header_t *head, struct buffer_t *buffer);
static void Fedex_TransmitDiscoverObjectInstance(struct header_t *head, struct buffer_t *buffer);
static void Fedex_TransmitReceiveInteraction(struct header_t *head, struct buffer_t *buffer);
static void Fedex_TransmitSynchronizationPointAchieved(struct header_t *head,
		        struct buffer_t *buffer);
static void Fedex_TransmitFederationSynchronized(struct header_t *head,
		                                struct buffer_t *buffer);
/* the function array is related to enum func_e in Endecode.h */
func_t Fedex_func[] = 
{
	NULL,
	
	/* Federation Management Services */
	NULL, // Fedex_CreateFederationExecution,
	NULL, // Fedex_DestroyFederationExecution,
	Fedex_JoinFederationExecution,
	Fedex_ResignFederationExecution,
	Fedex_RegisterFederationSynchronizationPoint,
	Fedex_SynchronizationPointAchieved,
	NULL, // Fedex_RequestFederationSave,
	NULL, // Fedex_FederateSaveBegun,
	NULL, // Fedex_FederateSaveComplete,
	NULL, // Fedex_QueryFederationSaveStatus,
	NULL, // Fedex_RequestFederationRestore,
	NULL, // Fedex_FederateRestoreComplete,
	NULL, // Fedex_QueryFederationRestoreStatus,

	/* Declaration Management Services */
	Fedex_PublishObjectClassAttributes,
	Fedex_UnpublishObjectClass,
	Fedex_UnpublishObjectClassAttributes,
	Fedex_PublishInteractionClass,
	Fedex_UnpublishInteractionClass,
	Fedex_SubscribeObjectClassAttributes,
	Fedex_UnsubscribeObjectClass,
	Fedex_UnsubscribeObjectClassAttributes,
	Fedex_SubscribeInteractionClass,
	Fedex_UnsubscribeInteractionClass,

	/* Object Management Services */
	Fedex_ReserveObjectInstanceName,
	Fedex_RegisterObjectInstance,
    Fedex_UpdateAttributeValues,
    Fedex_SendInteraction,
    Fedex_DeleteObjectInstance,
    NULL, // Fedex_LocalDeleteObjectInstance,
    Fedex_ChangeAttributeTransportationType,
	Fedex_ChangeInteractionTransportationType,
    Fedex_RequestAttributeValueUpdate,

	/* Ownership Management Services */
	NULL, // Fedex_UnconditionalAttributeOwnershipDivestiture,
	NULL, // Fedex_NegotiatedAttributeOwnershipDivestiture,
	NULL, // Fedex_ConfirmDivestiture,
	NULL, // Fedex_AttributeOwnershipAcquisition,
	NULL, // Fedex_AttributeOwnershipAcquisitionIfAvailable,
	NULL, // Fedex_AttributeOwnershipDivestitureIfWanted,
	NULL, // Fedex_CancelNegotiatedAttributeOwnershipDivestiture,
	NULL, // Fedex_CancelAttributeOwnershipAcquisition,
	NULL, // Fedex_QueryAttributeOwnership,
	NULL, // Fedex_IsAttributeOwnedByFederate,

	/* Time Management Service */
	NULL, // Fedex_EnableTimeRegulation,
	NULL, // Fedex_DisableTimeRegulation,
	NULL, // Fedex_EnableTimeConstrained,
	NULL, // Fedex_DisableTimeConstrained,
	NULL, // Fedex_TimeAdvanceRequest,
	NULL, // Fedex_TimeAdvanceRequestAvailable,
	NULL, // Fedex_NextMessageRequest,
	NULL, // Fedex_NextMessageRequestAvailable,
	NULL, // Fedex_FlushQueueRequest,
	NULL, // Fedex_EnableAsynchronousDelivery,
	NULL, // Fedex_DisableAsynchronousDelivery,
	NULL, // Fedex_QueryGALT,
	NULL, // Fedex_QueryLogicalTime,
	NULL, // Fedex_QueryLITS,
	NULL, // Fedex_ModifyLookahead,
	NULL, // Fedex_QueryLookahead,
	NULL, // Fedex_Retract,
	NULL, // Fedex_ChangeAttributeOrderType,
	NULL, // Fedex_ChangeInteractionOrderType,

	/* Data Distribution Management */
	NULL, // Fedex_CreateRegion,
	NULL, // Fedex_CommitRegionModifications,
	NULL, // Fedex_DeleteRegion,
	NULL, // Fedex_RegisterObjectInstanceWithRegions,
	NULL, // Fedex_AssociateRegionsForUpdates,
	NULL, // Fedex_UnassociateRegionsForUpdates,
	NULL, // Fedex_SubscribeObjectClassAttributesWithRegions,
	NULL, // Fedex_UnsubscribeObjectClassAttributesWithRegions,
	NULL, // Fedex_SubscribeInteractionClassWithRegions,
	NULL, // Fedex_UnsubscribeInteractionClassWithRegions,
	NULL, // Fedex_SendInteractionWithRegions,
	NULL, // Fedex_RequestAttributeValueUpdateWithRegions,

	/*Fedex to Fedex*/
	Fedex_TransmitReflectAttributeValues ,
	Fedex_TransmitDiscoverObjectInstance ,
	Fedex_TransmitReceiveInteraction,
	    Fedex_TransmitSynchronizationPointAchieved,
		    Fedex_TransmitFederationSynchronized
};		

int main(int argc,char *argv[])
{
	int flag;
	MSG_Status status;
	char *buffer;
	struct header_t *head;
	struct buffer_t *data;
	int i, nodeCounter;
	func_t fn = NULL;
	OHTTable ohttable;
	IHTTable ihttable;
	extern uint32_t fedexID;
	
	/* Start debug mode and timer in debug messages */
	set_debugmode(DEBUG_FEDEX | DEBUG_OMT);
	init_time();
	
	/* Initialize FDD parser */
	FDDname = new char[200];
	strcpy(FDDname, "sample.xml");
	
	oht = new OHT<int>();
	iht = new IHT<int>();

	if(oht->read_XMLfile(FDDname) == 0 || iht->read_XMLfile(FDDname) == 0)
	{
		printf("\n\n Fedexec Could Not Open The FDD file!!\n\n");
		return 0;
	}
	
	if(oht->parsing()==0 || iht->parsing()==0)
	{
		printf("\n\n Fedexec have a Error reading FDD file \n\n");
		return 0;
	}

	initialize_ohttable(oht, &ohttable);
	initilaize_ihttable(iht, &ihttable);
	 
	/* Initialize object handle tree and interaction handle tree data list */
	nodeCounter = (int)oht->getClassNodeCount();
	publist = new struct DataList[nodeCounter+1];
	sendlist = new federateID_set[nodeCounter+1];
	pubtable = new bool[nodeCounter+1];

	for(i=1; i<=nodeCounter; i++)
	{
		pubtable[i] = false;
		INIT_LIST_HEAD(&publist[i].list);
	}

	nodeCounter = (int)iht->getClassNodeCount();
	Ipublist = new struct DataList[nodeCounter+1];
	Isendlist = new federateID_set[nodeCounter+1];
	Ipubtable = new bool[nodeCounter+1];

	for(i=1; i<=nodeCounter; i++)
	{
	    Ipubtable[i] = false;
	    INIT_LIST_HEAD(&Ipublist[i].list);
	}
	
	/* Initialize network */
	VS_Init(atoi(argv[1]));
	
	while( VS_Alive() != 1 )
		sleep(1);
	
	vid=fedexID;
	debug(DEBUG_FEDEX, "Fedex initialized. %d \n",vid);
	
	/* main routine */
	while(true)
	{
		flag = 0;

		VS_Iprobe(&flag, &status);

		if(flag == 1)
		{
			/* recv command message from federate */
			buffer = new char[status.size];
			VS_Recv(buffer, &status);
			
			head=(struct header_t*)buffer;
			data = new struct buffer_t[head->count];


			DecodeBuffer(buffer, head, data);
			
			fn = Fedex_func[head->type];
			Need_SendAllFedexs=false;
			if (fn != NULL)
			{
				(*fn)(head, data);
			}
			if (Need_SendAllFedexs) {
			    VS_SendAllFedexs(buffer,status.size);
			}
			
			delete [] data;
			delete [] buffer;
		}
		usleep(10);
	}
	
	VS_Finalize();
	
	return 0;
}

static void Fedex_JoinFederationExecution(struct header_t *head, struct buffer_t *buffer)
{
	federateID id;
	wstring fedType,fedName;
	EndecodeFactory enc(JoinFederationExecution);

	decode_wstring(fedType, &buffer[0]);
	decode_wstring(fedName, &buffer[1]);
	
	/* send command back */
	id.vid = vid;
	id.fid = fid;
	
	enc.create(1);
	enc.add(&id, sizeof(id));
	VS_SendComm(head->src.fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
	debug(DEBUG_FEDEX, "%s: assign federate id to %d\n", __func__, fid);

	fid++;
}

static void Fedex_ResignFederationExecution(struct header_t *head, struct buffer_t *buffer)
{
}

static void Fedex_RegisterFederationSynchronizationPoint(struct header_t *head, struct buffer_t *buffer)
{
	unsigned int i;
	federateID origin;
	struct buffer_t buf;
	int synpointcount;
	RTI_wstring label;
	RTI_UserSuppliedTag theUserSuppliedTag;

    if(head->src.vid==vid)
            Need_SendAllFedexs=true;
	/* Decode Synchronization Point Label*/
	decode_wstring(label, &buffer[0]);
	origin = head->src;

	/* Check if the Synchronization Point is already exist*/
	if(Need_SendAllFedexs)
	if(synpointmap.count(label) != 0)
	{
		char reason[10] = "failed";
		/* FIXME: add proper failed reason */
		EndecodeFactory failed(SynchronizationPointRegistrationFailed);
		failed.create(2);
		encode_wstring(label, &buf);
		failed.add(&buf);
		failed.add(reason, 7);
		VS_SendMsg(head->src.fid, failed.get_buffer_list(), failed.get_len_list(), failed.get_size());
		
		return;
	}
	else
	{
		/* Send out SynchronizationPointRegistrationSucceeded */
		EndecodeFactory succeed(SynchronizationPointRegistrationSucceeded);
		succeed.create(1);
		encode_wstring(label, &buf);
		succeed.add(&buf);
		
		VS_SendMsg(head->src.fid, succeed.get_buffer_list(), succeed.get_len_list(), succeed.get_size());
	}

	/* Re-pack the header and payload */
	EndecodeFactory enc(AnnounceSynchronizationPoint);
	enc.create(head->count);
	enc.set_src(&origin);
	for (i=0; i<(unsigned int)head->count; i++)
	{
		enc.add(buffer[i].start, buffer[i].len);
	}
	enc.set_usertag(head->user_tag);

	/* If the RTI_FederateHandleSet's size is 0, synchronize all federate */
	if(head->user_tag == 0)
	{
		synpointcount = fid-1;
	}
	else /*Synchronize selected federate*/
	{
		synpointcount = head->user_tag;
	}

	/*Set the number of synchronized point to be zero, and save how much point shold be synchronized*/
	synpointmap[label] = make_pair(synpointcount, 0);

    if(Need_SendAllFedexs)
	{
	       fedex_synpointmap[label] = make_pair((int)fedex_number, 0);
	       fedex_synpointreg[label] = head->src.vid;
	}
	else
	{
	       fedex_synpointreg[label] = head->src.vid;
	}
		
	
	/* Send out AnnounceSynchronizationPoint to all federate*/
	for(i=1; i<fid; i++)
	{
		VS_SendMsg(i, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
	}
}

static void Fedex_SynchronizationPointAchieved(struct header_t *head, struct buffer_t *buffer)
{
	RTI_wstring label;
	std::pair<int,int> synfedcount;
	
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	decode_wstring(label, &buffer[0]);
	
	synfedcount = synpointmap[label];
	synfedcount.second++;
	synpointmap[label] = synfedcount;
#if 1
	if(synfedcount.first == synfedcount.second)
	{
		EndecodeFactory enc(TransmitSynchronizationPointAchieved);
		enc.create(1);
		enc.add(&buffer[0]);
		if(fedex_synpointreg[label]!=vid)
		{
			VS_SendFedex(fedex_synpointreg[label],enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}
		else
			Fedex_TransmitSynchronizationPointAchieved(head,buffer);
		synpointmap.erase(label);
	}
 	
	/* Check if all federates are Synchronized */
#else 
	if(synfedcount.first == synfedcount.second)
	{
		EndecodeFactory enc(FederationSynchronized);
		enc.create(1);
		enc.add(&buffer[0]);

		/* Send FederationSynchronized to all federates */
		for(i=1; i<(int)fid; i++)
		{
			VS_SendMsg(i, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}
		
		/* Erase Synchronizd label */
		synpointmap.erase(label);
	}
#endif
/* FIXME: should we need the following increment? */
#if 0
	else
	{
		synpointmap[label].first++;
	}
#endif

}

static void Fedex_PublishObjectClassAttributes(struct header_t *head, struct buffer_t *buffer)
{
	int i, base=1;
	struct buffer_list_t attr_list;
	RTI_ObjectClassHandle theClass;
	RTI_AttributeHandleSet attributeList;
	RTI_AttributeHandleSet::iterator iter;
	struct DataList *data;
	int root_node;
	int pnode;
	federateID_set::iterator p;
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);

    if(head->src.vid==vid)
		        Need_SendAllFedexs=true;
	
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);

	/* decode RTI_AttributeHandleSet */
	attr_list.num = head->count - 1;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	
	for(i=0; i<attr_list.num; i++)
	{
	    attr_list.start_list[i] = buffer[base+i].start;
	    attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleSet(attributeList, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;
		
	debug(DEBUG_FEDEX, "%s: fed (%d %d) publishes\n", __func__, head->src.vid, head->src.fid);
	debug(DEBUG_FEDEX, "    objectclass = %ld, attribute = ", theClass.getHandle());
	for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
	{
		debug_p(DEBUG_FEDEX, "%ld ", iter->getHandle());
	}
	debug_p(DEBUG_FEDEX, "\n");

	/* add object class handle to object class publish list */
	root_node = (int)theClass.getHandle();
	data = new struct DataList;
	data->id = head->src;
	
	INIT_LIST_HEAD(&data->list);
	list_add_tail(&data->list, &publist[root_node].list);
	pubtable[root_node] = true;

	   /*merage parent node sendlist */
	pnode=root_node;
	while(pnode!=1)
	{
		pnode=oht->getClassParentNode(pnode);
		sendlist[root_node].insert(sendlist[pnode].begin(),sendlist[pnode].end());
	}
}

static void Fedex_UnpublishObjectClass(struct header_t *head, struct buffer_t *buffer)
{
	RTI_ObjectClassHandle theClass;
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);
	debug(DEBUG_FEDEX, "%s: fed (%d %d) unpublishes objectclass %ld\n", 
			__func__, head->src.vid, head->src.fid, theClass.getHandle());
	/* TODO */
}

static void Fedex_UnpublishObjectClassAttributes(struct header_t *head, struct buffer_t *buffer)
{
	
	int i, base;
	struct buffer_list_t attr_list;
	RTI_ObjectClassHandle theClass;
	RTI_AttributeHandleSet attributeList;
	RTI_AttributeHandleSet::iterator iter;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);

	/* decode RTI_AttributeHandleSet */
	base = 1;
	attr_list.num = head->count-base;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	
	for(i=0; i<attr_list.num; i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleSet(attributeList, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;

	debug(DEBUG_FEDEX, "%s: fed (%d %d) unpublishes\n", head->src.vid, head->src.fid);
	debug(DEBUG_FEDEX, "    objectClass = %ld, attribute = ", theClass.getHandle());
	for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
	{
		debug_p(DEBUG_FEDEX, "%ld ", iter->getHandle());
	}
	debug_p(DEBUG_FEDEX, "\n");

	/* TODO */
}

static void Fedex_SubscribeObjectClassAttributes(struct header_t *head, struct buffer_t *buffer)
{
	int i, base;
	struct buffer_list_t attr_list;
	RTI_ObjectClassHandle theClass;
	RTI_AttributeHandleSet attributeList;
	RTI_AttributeHandleSet::iterator iter;
	int root_node, end_node;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);

	/* decode RTI_AttributeHandleSet */
	base = 1;
	attr_list.num = head->count-base;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	
	for(i=0; i<attr_list.num; i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleSet(attributeList, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;

	debug(DEBUG_FEDEX, "%s: fed (%d %d) subscribes\n", __func__, head->src.vid, head->src.fid);
	debug(DEBUG_FEDEX, "    objectclass = %ld, attribute = ", theClass.getHandle());
	for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
	{
		debug_p(DEBUG_FEDEX, "%ld ", iter->getHandle());
	}
	debug_p(DEBUG_FEDEX, "\n");

	/* 
	 * We need to add this subcribed class handle to the send list of 
	 * all published classes following by this subscribe object class.
	 */
	root_node = (int)theClass.getHandle();
	end_node = (int)oht->getClassTailValue(root_node);

	debug(DEBUG_FEDEX, "    update sendlist of subclass = ");
	for(i=root_node; i<=end_node; i++)
	{
		if(pubtable[i] == true)
		{
			sendlist[i].insert(head->src);	
			debug_p(DEBUG_FEDEX, "%d ", i);
		}
	}
	debug_p(DEBUG_FEDEX, "\n");
}

static void Fedex_UnsubscribeObjectClass(struct header_t *head, struct buffer_t *buffer)
{
	RTI_ObjectClassHandle theClass;
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);

	debug(DEBUG_FEDEX, "%s: fed (%d %d) unsubscribes objectclass %ld\n", 
			__func__, head->src.vid, head->src.fid, theClass.getHandle());

	/* TODO */
#if 0
	root_node = (int)theClass.getHandle();
	end_node = (int)oht->getClassTailValue(root_node);
#endif
}

static void Fedex_UnsubscribeObjectClassAttributes(struct header_t *head, struct buffer_t *buffer)
{
	int i, base;
	struct buffer_list_t attr_list;
	RTI_ObjectClassHandle theClass;
	RTI_AttributeHandleSet attributeList;
	RTI_AttributeHandleSet::iterator iter;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);

	/* decode RTI_AttributeHandleSet */
	base = 1;
	attr_list.num = head->count-base;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	
	for(i=0; i<attr_list.num; i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleSet(attributeList, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;

	debug(DEBUG_FEDEX, "%s: fed %d unsubscribes\n", __func__, head->src.fid, head->src.vid);
	debug(DEBUG_FEDEX, "    objectclass = %ld, attribute = ", theClass.getHandle());
	for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
	{
		debug_p(DEBUG_FEDEX, "%ld ", iter->getHandle());
	}
	debug_p(DEBUG_FEDEX, "\n");

	/* TODO */
}

static void Fedex_PublishInteractionClass(struct header_t *head, struct buffer_t *buffer)
{
	int root_node;
	RTI_InteractionClassHandle theClass;
	struct DataList *data;
	int pnode;
	
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);

	       if(head->src.vid==vid)
			           Need_SendAllFedexs=true;
	/* decode RTI_InteractionClassHandle */
	decode_RTI_InteractionClassHandle(theClass, &buffer[0]);
	
	debug(DEBUG_FEDEX, "%s: fed (%d %d) publishes interaction %ld\n", 
			__func__, head->src.vid, head->src.fid, theClass.getHandle());

	/* add interaction handle to interaction publish list */
	root_node = theClass.getHandle();
	data = new struct DataList;
	data->id = head->src;
	
	INIT_LIST_HEAD(&data->list);
	list_add_tail(&data->list, &Ipublist[root_node].list);
	Ipubtable[root_node] = true;
	
	/*merage parent node sendlist */
	pnode=root_node;
	federateID_set::iterator p;
	while(pnode!=1)
	{
		pnode=iht->getClassParentNode(pnode);
		Isendlist[root_node].insert(Isendlist[pnode].begin(),Isendlist[pnode].end());
	}
				
}

static void Fedex_UnpublishInteractionClass(struct header_t *head, struct buffer_t *buffer)
{
	RTI_InteractionClassHandle theClass;
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	/* decode RTI_InteractionClassHandle */
	decode_RTI_InteractionClassHandle(theClass,&buffer[0]);
	
	debug(DEBUG_FEDEX, "%s: fed (%d %d) unpublishes interaction %ld\n", 
			__func__, head->src.vid, head->src.fid, theClass.getHandle());

	
	/* TODO */
}

static void Fedex_SubscribeInteractionClass(struct header_t *head, struct buffer_t *buffer)
{
	int i;
	RTI_InteractionClassHandle theClass;
	int root_node, end_node;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	/* decode RTI_InteractionClassHandle */
	decode_RTI_InteractionClassHandle(theClass,&buffer[0]);

	debug(DEBUG_FEDEX, "%s: fed (%d %d) subscribes\n", __func__, head->src.vid, head->src.fid);
	debug(DEBUG_FEDEX, "    interaction = %ld\n", theClass.getHandle());

	root_node = (int)theClass.getHandle();
	end_node = (int)iht->getClassTailValue(root_node);

	debug(DEBUG_FEDEX, "    update sendlist of subclass = ");
	for(i=root_node; i<=end_node; i++)
	{
		if(Ipubtable[i] == true)
		{
			Isendlist[i].insert(head->src);
			debug_p(DEBUG_FEDEX, "%d ", i);
		}
	}
	debug_p(DEBUG_FEDEX, "\n");
}

static void Fedex_UnsubscribeInteractionClass(struct header_t *head, struct buffer_t *buffer)
{
	RTI_InteractionClassHandle theClass;
	
	if(head->src.vid==vid)
		Need_SendAllFedexs=true;
	decode_RTI_InteractionClassHandle(theClass,&buffer[0]);

	debug(DEBUG_FEDEX, "%s: fed (%d %d) unsubscribes interaction %ld\n", 
			__func__, head->src.vid, head->src.fid, theClass.getHandle());
	/* TODO */
}




/* 
 * Object Management Services 
 */



static void Fedex_ReserveObjectInstanceName(struct header_t *head, struct buffer_t *buffer)
{


	struct buffer_t buf;
	char name[300] = {'\0'};
	EndecodeFactory enc(ReserveObjectInstanceName);
	RTI_wstring theObjectInstanceName;


	
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	/* decode theObjectInstanceName*/
	decode_wstring(theObjectInstanceName, &buffer[0]);
	
	debug(DEBUG_FEDEX, "    federate %d reserveName with ", head->src.fid);
	WStrtoStr(name, theObjectInstanceName, 299);
	debug_p(DEBUG_FEDEX, "theObjectInstanceName = L\"%s\"\n", &name[0]);
	

	/* major code*/
	enc.create(1);
	encode_wstring(theObjectInstanceName, &buf);
	enc.add(&buf);
	if(globalObjectNames.count(theObjectInstanceName)<=0)
	{	
		debug(DEBUG_FEDEX, "    To Reserve Name\n");
		globalObjectNames.insert(theObjectInstanceName);
		debug(DEBUG_FEDEX, "    send NameReserveSucceed...");
		enc.set_usertag(1);
	}
	else
	{	
		debug(DEBUG_FEDEX, "    send NameReserveFailed...");
		enc.set_usertag(2);
	}
	VS_SendComm(head->src.fid, enc.get_buffer_list(),enc.get_len_list(), enc.get_size());	
	debug_p(DEBUG_FEDEX, "    Done.\n");

}



/*
 * Fedex_RegisterObjectInstance()
 *
 * There are two versions of RegisterObjectInstance. One is with user-specific object name
 * and the other is not.  With user-specific object name, the federate will set user_tag to 1
 * and set it to 0 to the other.
 * On success of object registeration, we will send the object handle back to the request 
 * federate and issuing the callback to subscribers.
 */
static void Fedex_RegisterObjectInstance(struct header_t *head, struct buffer_t *buffer)
{

	const int FEDERATE_PROVIDE_NAME=1;
	const int RTI_PROVIDE_NAME=2;
	int i, base=0;
	struct buffer_list_t attr_list;
	RTI_ObjectClassHandle theClass;
	RTI_ObjectInstanceHandle theObject;
	RTI_wstring theObjectInstanceName;
	RTI_AttributeHandleSet attributeList;
	RTI_AttributeHandleSet::iterator iter;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	int current_node;
	EndecodeFactory enc(RegisterObjectInstance);
	EndecodeFactory enc2(DiscoverObjectInstance);
	
	federateID_set::iterator pointer;
	int mark[10]={0};
	
	
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);

	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);
	
	if(head->user_tag == FEDERATE_PROVIDE_NAME)
	{
		decode_wstring(theObjectInstanceName, &buffer[1]);
		base = 2;
		enc.create(1);
		enc.set_usertag(FEDERATE_PROVIDE_NAME);
		enc2.set_usertag(FEDERATE_PROVIDE_NAME);
	}
	else if(head->user_tag == RTI_PROVIDE_NAME)
	{
		char HLAname[300];
		sprintf(HLAname, "HLAinstVID%dOID%d", vid, globalObject);
		theObjectInstanceName = RTI_wstring(StrtoWStr(&HLAname[0]));
		base =1;
		enc.create(2);
		enc.set_usertag(RTI_PROVIDE_NAME);
		enc2.set_usertag(RTI_PROVIDE_NAME);
	}
	else
	{
		printf("error\n");
		/*Error Appear*/
	}

	/* decode RTI_AttributeHandleSet */
	attr_list.num = head->count - base; 
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	for(i=0; i<attr_list.num; i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleSet(attributeList, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;

	debug(DEBUG_FEDEX, "    federate %d registers with\n", head->src.fid);
	debug(DEBUG_FEDEX, "    classhandle  = %ld, attribute = ", theClass.getHandle());
	
	for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
	{
		debug_p(DEBUG_FEDEX, "%ld ", iter->getHandle());
	}
	debug_p(DEBUG_FEDEX, "\n");
	debug(DEBUG_FEDEX, "    ");
	wcout<< "ObjectInstanceName = " << theObjectInstanceName << endl;
		
	/* 
	 * Send object handle back to federate's LRC
	 */
	
	theObject = RTI_ObjectInstanceHandle(globalObject);
	debug(DEBUG_FEDEX, "    object instance handle(oid)  = %ld\n",theObject.getHandle());
	
	encode_RTI_ObjectInstanceHandle(theObject, &buf);
	enc.add(&buf);
	if(head->user_tag == RTI_PROVIDE_NAME)
	{
		encode_wstring(theObjectInstanceName, &buf);
		enc.add(&buf);
	}
	
	VS_SendComm(head->src.fid, enc.get_buffer_list(),enc.get_len_list(), enc.get_size());
	globalObject++;
	
	/*
	 * Discovery callback:
	 *	Send the class handle, object handle of this class, object name ,and published attribute handle to 
	 *		all subscibers in the send list
	 */

	enc2.create(3 + attributeList.size());
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc2.add(&buf);
	encode_RTI_ObjectInstanceHandle(theObject, &buf);
	enc2.add(&buf);
	encode_wstring(theObjectInstanceName, &buf);
	enc2.add(&buf);
	encode_RTI_AttributeHandleSet(attributeList, &buf_list);
	enc2.add(&buf_list);
	enc2.set_src(&head->src);
	enc2.set_usertag(head->user_tag);
	
	

	
	debug(DEBUG_FEDEX, "    send discovery to fed:\n ");
	current_node = (int)theClass.getHandle();
	
	/* travsal sendlist of current_node, and decide to send to other fedex or LRC of same network area */	
	for(pointer=sendlist[current_node].begin();pointer!=sendlist[current_node].end();
			  pointer++)
	{
		if(pointer->vid==vid)
		{
			debug_p(DEBUG_FEDEX, "direct->(%d,%d ) \n ",pointer->vid,pointer->fid);
			enc2.set_type(DiscoverObjectInstance);
			VS_SendMsg(pointer->fid, enc2.get_buffer_list(), enc2.get_len_list(), enc2.get_size());
		}
		else
		{
			if(!mark[(int)pointer->vid])
			{
				debug_p(DEBUG_FEDEX, "indirect->(%d,%d ) \n ",pointer->vid,pointer->fid);
				enc2.set_type(TransmitDiscoverObjectInstance);
				VS_SendFedex(pointer->vid,enc2.get_buffer_list(), enc2.get_len_list(), enc2.get_size());
				mark[(int)pointer->vid]=1;
			}
		}
	}
	




}


static void Fedex_UpdateAttributeValues(struct header_t *head, struct buffer_t *buffer)
{
	int i;
	RTI_ObjectClassHandle theClass;
	int current_node;
	RTI_AttributeHandleSet local_set;
	struct buffer_list_t buf_list;
	EndecodeFactory enc(ReflectAttributeValues);
	EndecodeFactory enc2(UpdateAttributeValues);
	federateID_set::iterator pointer;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);

	/* Re-pack the header and payload */
	enc.create(head->count);
	for (i=0;i<head->count;i++)
	{
		enc.add(buffer[i].start, buffer[i].len);
	}
	enc.set_src(&head->src);
	

	

	debug(DEBUG_FEDEX, "%s: forward req from fed (%d %d) to fed: ", __func__, head->src.vid, head->src.fid);
	current_node = (int)theClass.getHandle();
	
	for(pointer=sendlist[current_node].begin();pointer!=sendlist[current_node].end();
			  pointer++)
	{
		   if(pointer->vid==vid)
		   {
				debug_p(DEBUG_FEDEX, "direct->(%d,%d )  ",pointer->vid,pointer->fid);
				if(LocalTransmit==0)
				{
					//enc.set_type(ReflectAttributeValues);
					enc.set_type(ReflectAttributeValues);
					VS_SendMsg(pointer->fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
				}
				else
				{
					if(pointer->fid!=head->src.fid)
						local_set.insert(RTI_AttributeHandle(pointer->fid));
					else
					{
						enc.set_type(ReflectAttributeValues);
	                    VS_SendMsg(pointer->fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
					}
				}
		   }
		   else
		   {
			   debug_p(DEBUG_FEDEX, "indirect->(%d,%d )  ",pointer->vid,pointer->fid);
			   enc.set_type(TransmitReflectAttributeValues);
			   VS_SendFedex(pointer->vid,enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		   }
	}
	debug_p(DEBUG_FEDEX, "\n");
	
	enc2.create(local_set.size());
	if(local_set.size()!=0)
	{
		encode_RTI_AttributeHandleSet(local_set, &buf_list);
		enc2.add(&buf_list);
	}
	else
		enc2.set_usertag(-1);
	enc2.set_src(&head->src);
	VS_SendComm(head->src.fid, enc2.get_buffer_list(), enc2.get_len_list(), enc2.get_size());
	
}

static void Fedex_SendInteraction(struct header_t *head, struct buffer_t *buffer)
{
	int i;
	RTI_InteractionClassHandle theInteraction;
	int current_node;
	struct buffer_list_t buf_list;
	 RTI_AttributeHandleSet local_set;
	EndecodeFactory enc(ReceiveInteraction);
	EndecodeFactory enc2(SendInteraction);
	federateID_set::iterator pointer;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	
	/* decode RTI_InteractionClassHandle */
	decode_RTI_InteractionClassHandle(theInteraction, &buffer[0]);

	/* Re-pack the header and payload */
	enc.create(head->count);
	for (i=0;i<head->count;i++)
	{
		enc.add(buffer[i].start, buffer[i].len);
	}
	enc.set_src(&head->src);
	
	debug(DEBUG_FEDEX, "%s: forward req from fed (%d %d) to fed: ", __func__, head->src.vid, head->src.fid);
	current_node = (int)theInteraction.getHandle();
	/* travers sendlist of current_node, 
	 * and decide to send to fedex in other area
	 * or send to LRC in same area.  
	 */
	for(pointer=Isendlist[current_node].begin();pointer!=Isendlist[current_node].end();
			  pointer++)
	{
		   if(pointer->vid==vid)
		   {
				debug_p(DEBUG_FEDEX, "direct->(%d,%d )  ",pointer->vid,pointer->fid);
				if(LocalTransmit==0)
				{
					enc.set_type(ReceiveInteraction);
					VS_SendMsg(pointer->fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
				}
				else
				{
					if(pointer->fid!=head->src.fid)
						local_set.insert(RTI_AttributeHandle(pointer->fid));
					else
					{
						enc.set_type(ReceiveInteraction);
						VS_SendMsg(pointer->fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
					}
				}
		   }
		   else
		   {
			   debug_p(DEBUG_FEDEX, "indirect->(%d,%d )  ",pointer->vid,pointer->fid);
			   enc.set_type(TransmitReceiveInteraction);
			   VS_SendFedex(pointer->vid,enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		   }
		   
	}
	debug_p(DEBUG_FEDEX, "\n");
	

	enc2.create(local_set.size());
	if(local_set.size()!=0)
	{
	     encode_RTI_AttributeHandleSet(local_set, &buf_list);
	     enc2.add(&buf_list);
	}
    else
	     enc2.set_usertag(-1);
	enc2.set_src(&head->src);
	VS_SendComm(head->src.fid, enc2.get_buffer_list(), enc2.get_len_list(), enc2.get_size());
}




static void Fedex_DeleteObjectInstance(struct header_t *head, struct buffer_t *buffer)
{
	RTI_ObjectInstanceHandle theObject;
	RTI_UserSuppliedTag theUserSuppliedTag;
	
	/* decode RTI_ObjectInstanceHandle */
	decode_RTI_ObjectInstanceHandle(theObject, &buffer[0]);
	/* decode RTI_UserSuppliedTag */
	decode_RTI_UserSuppliedTag(theUserSuppliedTag, &buffer[1]);

	
	debug(DEBUG_FEDEX, "%s: instance %ld usertag = %s\n", __func__, theObject.getHandle(), 
			(char *)theUserSuppliedTag.data());
}


static void Fedex_ChangeAttributeTransportationType(struct header_t *head, struct buffer_t *buffer)
{
	int i, base;
	struct buffer_list_t attr_list;
	RTI_ObjectInstanceHandle theClass;
	RTI_AttributeHandleSet attributeList;
	RTI_AttributeHandleSet::iterator iter;
	
	/* decode RTI_ObjectInstanceHandle */
	decode_RTI_ObjectInstanceHandle(theClass, &buffer[0]);

	/* decode RTI_AttributeHandleSet */
	base = 1;
	attr_list.num = head->count-base;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	for(i=0; i<attr_list.num; i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleSet(attributeList, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;
	
	for(iter=attributeList.begin(); iter!=attributeList.end();iter++)
	{
	}
	/* TODO */
}

static void Fedex_ChangeInteractionTransportationType(struct header_t *head, struct buffer_t *buffer)
{
	RTI_InteractionClassHandle theClass;

	decode_RTI_InteractionClassHandle(theClass,&buffer[0]);
}

/*
 * Fedex_RequestAttributeValueUpdate()
 *
 * There are two versions of RequestAttributeValueUpdate. One is for object instance handle
 * (user_tag = 0) and one for object class handle (user_tag = 1).
 */
static void Fedex_RequestAttributeValueUpdate(struct header_t *head, struct buffer_t *buffer)
{
	int i, base;
	struct buffer_list_t attr_list;
	RTI_ObjectInstanceHandle theObject;
	RTI_ObjectClassHandle theClass;
	RTI_AttributeHandleSet attributeList;
	RTI_UserSuppliedTag theUserSuppliedTag;
	RTI_AttributeHandleSet::iterator iter;

	debug(DEBUG_FEDEX, "%s: fed (%d %d) requset updates\n", __func__, head->src.vid, head->src.fid);
	if(head->user_tag == 0)
	{
		decode_RTI_ObjectInstanceHandle(theObject, &buffer[0]);
		debug(DEBUG_FEDEX, "    object instance = %ld\n", theObject.getHandle());
	}
	else if(head->user_tag == 1)
	{
		decode_RTI_ObjectClassHandle(theClass,&buffer[0]);
		debug(DEBUG_FEDEX, "    object class = %ld\n", theClass.getHandle());
	}
	else
		error("%s: user_tag is invalid\n", __func__);
	
	/* decode RTI_AttributeHandleSet */
	base = 1;
	attr_list.num = head->count-base -1;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	for(i=0;i<attr_list.num;i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleSet(attributeList,&attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;

	/* decode RTI_UserSuppliedTag */
	decode_RTI_UserSuppliedTag(theUserSuppliedTag, &buffer[base+i]);
	
	debug(DEBUG_FEDEX, "    attribute = ");
	for(iter=attributeList.begin(); iter!=attributeList.end();iter++)
	{
		debug_p(DEBUG_FEDEX, "%ld ", iter->getHandle());
	}
	debug_p(DEBUG_FEDEX, "\n");
}

static void Fedex_TransmitDiscoverObjectInstance(struct header_t *head, struct buffer_t *buffer)
{
	int i,current_node;
	struct buffer_list_t attr_list;
	int base;
	RTI_ObjectClassHandle theClass;
	RTI_ObjectInstanceHandle theObject;
	RTI_wstring theObjectInstanceName;
	RTI_AttributeHandleSet attributeList;
	EndecodeFactory enc(DiscoverObjectInstance);
	federateID_set::iterator pointer;


	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);
	
	/* decode RTI_ObjectInstanceHandle */
	decode_RTI_ObjectInstanceHandle(theObject, &buffer[1]);
	
	/* decode RTI_wstring */
	decode_wstring(theObjectInstanceName, &buffer[2]);
	
	debug(DEBUG_FEDEX, "theClass = %d\n",(int) theClass.getHandle());
	debug(DEBUG_FEDEX, "theObject(oid) = %d\n",(int) theObject.getHandle());
	debug(DEBUG_FEDEX, "");
	wcout << "theObjectInstanceName = "<< theObjectInstanceName<<endl;
	/* decode RTI_AttributeHandleSet */
	base = 3;
	attr_list.num = head->count-base;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	for(i=0; i<attr_list.num; i++)
	{
		
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
		
	}         
	decode_RTI_AttributeHandleSet(attributeList, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;








	
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);
	debug(DEBUG_FEDEX, "class = %d\n",(int) theClass.getHandle());
	enc.create(head->count);
	for (i=0;i<head->count;i++)
	{
	   enc.add(buffer[i].start, buffer[i].len);
	}
	enc.set_src(&head->src);
	
	current_node = (int)theClass.getHandle();
	for(pointer=sendlist[current_node].begin();pointer!=sendlist[current_node].end();
			  pointer++)
	{
		if(pointer->vid==vid)
		{
			debug_p(DEBUG_FEDEX, "direct->(%d,%d )  ",pointer->vid,pointer->fid);
			VS_SendMsg(pointer->fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}
	}
	debug_p(DEBUG_FEDEX, "\n");
}

static void Fedex_TransmitReflectAttributeValues(struct header_t *head, struct buffer_t *buffer)
{
	int i,current_node;
	RTI_ObjectClassHandle theClass;
	EndecodeFactory enc(ReflectAttributeValues);
	federateID_set::iterator pointer;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);
	
	enc.create(head->count);
	for (i=0;i<head->count;i++)
	{
		enc.add(buffer[i].start, buffer[i].len);
	}
	enc.set_src(&head->src);

	debug_p(DEBUG_FEDEX, "class = %d ",(int)theClass.getHandle());
	current_node = (int)theClass.getHandle();
	for(pointer=sendlist[current_node].begin();pointer!=sendlist[current_node].end();
			 pointer++)
	{
		if(pointer->vid==vid)
		{
			debug_p(DEBUG_FEDEX, "direct->(%d,%d )  ",pointer->vid,pointer->fid);
			VS_SendMsg(pointer->fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}
	}
	debug_p(DEBUG_FEDEX, "\n");
	
}

static void Fedex_TransmitReceiveInteraction(struct header_t *head, struct buffer_t *buffer)
{
	int i,current_node;
	RTI_InteractionClassHandle  theInteraction;
	EndecodeFactory enc(ReceiveInteraction);
	federateID_set::iterator pointer;
	
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	
	/* decode RTI_InteractionClassHandle */
	decode_RTI_InteractionClassHandle(theInteraction, &buffer[0]);
	enc.create(head->count);
	for (i=0;i<head->count;i++)
	{
		enc.add(buffer[i].start, buffer[i].len);
	}
	enc.set_src(&head->src);
	
	current_node = (int)theInteraction.getHandle();
	for(pointer=Isendlist[current_node].begin();pointer!=Isendlist[current_node].end();
			pointer++)
	{
		if(pointer->vid==vid)
		{
			debug_p(DEBUG_FEDEX, "direct->(%d,%d )  ",pointer->vid,pointer->fid);
			VS_SendMsg(pointer->fid, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}
	}
	debug_p(DEBUG_FEDEX, "\n");
}

static void Fedex_TransmitSynchronizationPointAchieved(struct header_t *head,
		struct buffer_t *buffer)
{
	int i;
	RTI_wstring label;
	std::pair<int,int> fedex_synfedcount;

	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	decode_wstring(label, &buffer[0]);

	fedex_synfedcount = fedex_synpointmap[label];
	fedex_synfedcount.second++;
	fedex_synpointmap[label] = fedex_synfedcount;
	if(fedex_synfedcount.first == fedex_synfedcount.second)
	{
		EndecodeFactory enc(TransmitFederationSynchronized);
		enc.create(1);
		enc.add(&buffer[0]);

		/* Send FederationSynchronized to all fedex */
		Fedex_TransmitFederationSynchronized(head,buffer);
		for(i=0; i<(int)fedex_number; i++)
		{
			VS_SendFedex(i, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}

		/* Erase fedex Synchronizd label */
		fedex_synpointmap.erase(label);
	}
}
static void Fedex_TransmitFederationSynchronized(struct header_t *head,
		struct buffer_t *buffer)
{
	int i;
	EndecodeFactory enc(FederationSynchronized);
	debug(DEBUG_FEDEX, "%s entered.\n", __func__);
	enc.create(1);
	enc.add(&buffer[0]);
	/* Send FederationSynchronized to all federates */
	for(i=1; i<(int)fid; i++)
	{
		VS_SendMsg(i, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
	}
	/* Erase Synchronizd label */
}
/*
 * vim: ts=4 sts=4 sw=4
 */
