#include <stdint.h>
#include <iostream>
#include <map>
#include <set>
#include "FedAmbImplementor.h"
#include "MsgHandler.h"
#include "PubSubTable.h"
#include "Endecode.h"
#include "OMTDebug.h"
extern "C"
{
#include "debug.h"
}

extern RTI_auto_ptr<RTI_FederateAmbassador> FederateAmbassador;
extern federateID RTI_handle;					/* the Federate handle is for itself			*/
extern std::map <RTI_wstring, int> synpointset;	/* The set record all of Synchronization Point	*/


extern OHTTable ohttable;			/* P/S state of object class tree						*/
extern IHTTable ihttable;			/* P/S state of interaction class tree					*/
extern InstMap AllInst;				/* info of all of discovery&registed object instance	*/
extern InstSet OwnedInst;			/* all registed object instance							*/
extern InstNameSet ReserveNames;	/* The set of name that be reserveed but not used		*/
extern InstNameSet InUseNames;		/* The set of name that be reserved and used			*/
extern RTI_ObjectIDMap ObjectIDMap;	/* it afford you to use TopObject to find (vid,oid)		*/
extern int ObjectIDCount;			/* LRC has its owned counter to make unique TopObject	*/



typedef void (*func_t)(struct header_t *head, struct buffer_t *buffer);

static void LRC_synchronizationPointRegistrationSucceeded(struct header_t *head, struct buffer_t *buffer);
static void LRC_synchronizationPointRegistrationFailed(struct header_t *head, struct buffer_t *buffer);
static void LRC_announceSynchronizationPoint(struct header_t *head, struct buffer_t *buffer);
static void LRC_federationSynchronized(struct header_t *head, struct buffer_t *buffer);

static void LRC_discoverObjectInstance(struct header_t *head, struct buffer_t *buffer);
static void LRC_removeObjectInstance(struct header_t *head, struct buffer_t *buffer);
static void LRC_reflectAttributeValues(struct header_t *head, struct buffer_t *buffer);
static void LRC_receiveInteraction(struct header_t *head, struct buffer_t *buffer);

char LRC_func_name[][50] = 
{

	/* Federation Management */
	"SynchronizationPointRegistrationSucceeded",
	"SynchronizationPointRegistrationFailed",
	"AnnounceSynchronizationPoint",
	"FederationSynchronized",
	"InitiateFederateSave",
	"FederationSaved",
	"FederationNotSaved",
	"FederationSaveStatusResponse",
	"RequestFederationRestoreSucceeded",
	"RequestFederationRestoreFailed",
	"FederationRestoreBegun",
	"InitiateFederateRestore",
	"FederationRestored",
	"FederationNotRestored",

	/* Declaration Management */
	"StartRegistrationForObjectClass",
	"StopRegistrationForObjectClass",
	"TurnInteractionsOn",
	"TurnInteractionsOff",

	/* Object Management  */
	"ObjectInstanceNameReservationSucceeded",
	"ObjectInstanceNameReservationFailed",
	"DiscoverObjectInstance",
	"ReflectAttributeValues",
	"ReceiveInteraction",
	"RemoveObjectInstance",
	"AttributesInScope",
	"AttributesOutOfScope",
	"ProvideAttributeValueUpdate",
	"TurnUpdatesOnForObjectInstance",
	"TurnUpdatesOffForObjectInstance",

	/* Ownership Management */
	"RequestAttributeOwnershipAssumption",
	"RequestDivestitureConfirmation",
	"AttributeOwnershipAcquisitionNotification",
	"AttributeOwnershipUnavailable",
	"RequestAttributeOwnershipRelease",
	"ConfirmAttributeOwnershipAcquisitionCancellation",
	"InformAttributeOwnership",
	"AttributeIsNotOwned",
	"AttributeIsOwnedByRTI",

	/* Time Management */
	"TimeRegulationEnabled",
	"TimeConstrainedEnabled",
	"TimeAdvanceGrant",
	"RequestRetraction"
};


func_t LRC_func[] =
{
	/* Federation Management */
	LRC_synchronizationPointRegistrationSucceeded,
	LRC_synchronizationPointRegistrationFailed,
	LRC_announceSynchronizationPoint,
	LRC_federationSynchronized,
	NULL, // LRC_initiateFederateSave,
	NULL, // LRC_federationSaved,
	NULL, // LRC_federationNotSaved,
	NULL, // LRC_federationSaveStatusResponse,
	NULL, // LRC_requestFederationRestoreSucceeded,
	NULL, // LRC_requestFederationRestoreFailed,
	NULL, // LRC_federationRestoreBegun,
	NULL, // LRC_initiateFederateRestore,
	NULL, // LRC_federationRestored,
	NULL, // LRC_federationNotRestored,

	/* Declaration Management */
	NULL, // LRC_startRegistrationForObjectClass,
	NULL, // LRC_stopRegistrationForObjectClass,
	NULL, // LRC_turnInteractionsOn,
	NULL, // LRC_turnInteractionsOff,

	/* Object Management */
	NULL, // LRC_objectInstanceNameReservationSucceeded,
	NULL, // LRC_objectInstanceNameReservationFailed,
	LRC_discoverObjectInstance,
	LRC_reflectAttributeValues,
	LRC_receiveInteraction,
	LRC_removeObjectInstance,
	NULL, // LRC_attributesInScope,
	NULL, // LRC_attributesOutOfScope,
	NULL, // LRC_provideAttributeValueUpdate,
	NULL, // LRC_turnUpdatesOnForObjectInstance,
	NULL, // LRC_turnUpdatesOffForObjectInstance,

	/* Ownership Management */
	NULL, // LRC_requestAttributeOwnershipAssumption,
	NULL, // LRC_requestDivestitureConfirmation,
	NULL, // LRC_attributeOwnershipAcquisitionNotification,
	NULL, // LRC_attributeOwnershipUnavailable,
	NULL, // LRC_requestAttributeOwnershipRelease,
	NULL, // LRC_confirmAttributeOwnershipAcquisitionCancellation,
	NULL, // LRC_informAttributeOwnership,
	NULL, // LRC_attributeIsNotOwned,
	NULL, // LRC_attributeIsOwnedByRTI,

	/* Time Managemenet */
	NULL, // LRC_timeRegulationEnabled,
	NULL, // LRC_timeConstrainedEnabled,
	NULL, // LRC_timeAdvanceGrant,
	NULL  // LRC_requestRetraction

};






void MsgHandler::On_RO_Receiving(char *buffer)
{
	struct header_t *head;
	struct buffer_t *data;
	func_t fn = NULL;
	
	head=(struct header_t*)buffer;

#if 0
	debug(DEBUG_MSGHANDLER, "Receive type %s from (%d,%d) count %d user_tag %d\n",
			LRC_func_name[head->type], head->src.vid, head->src.fid, head->count, head->user_tag);
#endif
	
    data = new struct buffer_t[head->count];
    DecodeBuffer(buffer, head, data);
	
	fn = LRC_func[head->type];
	if (fn != NULL)
	{
		try{
			(*fn)(head, data);
		}
		catch(exception &e){
			printf("%s:%d ->C++std Runtime-Error catched by MsgHandler.cc\n", __FILE__, __LINE__); 
			cout << e.what() << endl;
		}
		catch(RTI_Exception &e){
			printf("%s:%d ->RTI Runtime-Error catched by MsgHandler.cc\n", __FILE__, __LINE__); 
			cout << e.what() << endl;	
		}
		/* If the runntime-Error on MsgHandler.cc is not denoted by User's code, then the Runtime-Error
		 * is happen at  RTI-implemented code.
		 */
		
	}

	delete [] data;
}

void MsgHandler::On_TSO_Receiving(char *buffer)
{
	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
}

void MsgHandler::On_Event_Receiving(char *buffer)
{
	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
}

static void LRC_synchronizationPointRegistrationSucceeded(struct header_t *head, struct buffer_t *buffer)
{

	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
	wstring synpoint;
	decode_wstring(synpoint, &buffer[0]);
	

	try{
		FederateAmbassador->synchronizationPointRegistrationSucceeded(synpoint);
	}
	catch(std::exception &e){
		printf("%s:%d ->C++std Runtime-Error happened at User's code, which is\n", __FILE__, __LINE__);
		printf("FederateAmbassador->synchronizationPointRegistrationSucceeded(synpoint);\n");
		cout << e.what() << endl;
		
	}
	catch(RTI_Exception &e){
		printf("%s:%d ->RTI Runtime-Error happened at User's code,which is\n", __FILE__, __LINE__); 
		printf("FederateAmbassador->synchronizationPointRegistrationSucceeded(synpoint);\n");
		cout << e.what() << endl;	
	}


}

static void LRC_synchronizationPointRegistrationFailed(struct header_t *head, struct buffer_t *buffer)
{
	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);

}

static void LRC_announceSynchronizationPoint(struct header_t *head, struct buffer_t *buffer)
{
	
	int i;
	struct buffer_list_t fed_list;
	wstring synpoint;
	RTI_UserSuppliedTag theUserSuppliedTag;
	RTI_FederateHandleSet federateList;


	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
	/* If usertag != 0, Synchronization is called with RTI_FederateHandleSet */
	if(head->user_tag != 0)
	{
		fed_list.num = head->count - 2;
		fed_list.start_list = new void*[fed_list.num];
		fed_list.len_list = new uint32_t[fed_list.num];
		
		for(i=0; i<fed_list.num; i++)
		{
			fed_list.start_list[i] = buffer[2+i].start;
			fed_list.len_list[i] = buffer[2+i].len;
		}
					
		decode_RTI_FederateHandleSet(federateList, &fed_list);

		delete [] fed_list.start_list;
		delete [] fed_list.len_list;

		/* TODO:
		 * Check if I am in the RTI_FederateHandleSet
		 * If not, return ;
		 */
	}

	/* Add this Synchronization point into Synchronization point set */
	decode_wstring(synpoint, &buffer[0]);
	synpointset[synpoint] = 0;
	
	decode_RTI_UserSuppliedTag(theUserSuppliedTag, &buffer[1]);
	try{
		FederateAmbassador->announceSynchronizationPoint(synpoint, theUserSuppliedTag);
	}
	catch(std::exception &e){
		printf("%s:%d ->C++std Runtime-Error happened at User's code, which is\n", __FILE__, __LINE__);
		printf("FederateAmbassador->announceSynchronizationPoint(synpoint, theUserSuppliedTag)\n");
		cout << e.what() << endl;
		
	}
	catch(RTI_Exception &e){
		printf("%s:%d ->RTI Runtime-Error happened at User's code,which is\n", __FILE__, __LINE__); 
		printf("FederateAmbassador->announceSynchronizationPoint(synpoint, theUserSuppliedTag)\n");
		cout << e.what() << endl;	
	}

	

	
}

static void LRC_federationSynchronized(struct header_t *head, struct buffer_t *buffer)
{
	RTI_wstring synpoint;

	
	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
	decode_wstring(synpoint, &buffer[0]);
	
	/*Check if I should receive Synchronization message*/
	if(synpointset.count(synpoint) == 0)
		return;

	synpointset.erase(synpoint);

	try{
		FederateAmbassador->federationSynchronized(synpoint);
	}
	catch(std::exception &e){
		printf("%s:%d ->C++std Runtime-Error happened at User's code, which is\n", __FILE__, __LINE__);
		printf("FederateAmbassador->federationSynchronized(synpoint)\n");
		cout << e.what() << endl;
		
	}
	catch(RTI_Exception &e){
		printf("%s:%d ->RTI Runtime-Error happened at User's code,which is\n", __FILE__, __LINE__); 
		printf("FederateAmbassador->federationSynchronized(synpoint)\n");
		cout << e.what() << endl;	
	}

}




static void LRC_discoverObjectInstance(struct header_t *head, struct buffer_t *buffer)
{
	int i, base;
	struct buffer_list_t attr_list;
	RTI_wstring theObjectInstanceName;
	RTI_ObjectClassHandle theClass;
	RTI_ObjectInstanceHandle theObject;
	RTI_AttributeHandleSet attributeList;
	RTI_AttributeHandleSet::const_iterator iter;
	int classhandle, CDChandle, attrhandle;
	bool AddObjectIDMap;
	RTI_ObjectInstanceHandle *TopObject;
	ObjectInstanceID ObjectID;	
	bool isDiscover;
	
	
	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
	/* decode RTI_ObjectClassHandle */
	decode_RTI_ObjectClassHandle(theClass, &buffer[0]);
	
	/* decode RTI_ObjectInstanceHandle */
	decode_RTI_ObjectInstanceHandle(theObject, &buffer[1]);
	
	/* decode RTI_wstring */
	decode_wstring(theObjectInstanceName, &buffer[2]);	

	debug(DEBUG_MSGHANDLER, "theClass = %d\n",(int) theClass.getHandle());
	debug(DEBUG_MSGHANDLER, "(vid,oid)=( %d,%d)\n",head->src.vid,(int) theObject.getHandle());
	debug(DEBUG_MSGHANDLER, "");
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
	
	Debug_AttributeList((int)theClass.getHandle(), attributeList);


	/* check whether discover instance exist at ObjectIDMap */
	AddObjectIDMap=true;
	TopObject=new RTI_ObjectInstanceHandle(0);
	RTI_ObjectIDMap::iterator pointer;
	for(pointer=ObjectIDMap.begin();pointer!=ObjectIDMap.end();pointer++)
	{
		if(pointer->second.oid.getHandle()==theObject.getHandle() && pointer->second.vid == head->src.vid)
		{
			AddObjectIDMap=false;
			*TopObject=pointer->first;
			break;
		}
	}
	
	/* if discover instance is not exist at ObjectIDMap,
	 * then allocate a TopObject by ObjectIDCount's value 
	 */
	if(AddObjectIDMap)
	{
		ObjectID.vid=head->src.vid;
		ObjectID.oid=theObject;
		ObjectIDCount++;
		TopObject=new RTI_ObjectInstanceHandle(ObjectIDCount);
		ObjectIDMap.insert(RTI_ObjectIDMap::value_type(*TopObject,ObjectID));
	}
	debug(DEBUG_MSGHANDLER, "check object instance (top,(vid,oid)))=(%d , (%d,%d))\n", (int)TopObject->getHandle(),head->src.vid,(int)theObject.getHandle());
	

	
	/* check whether we need to generate object instance */
	isDiscover=false;
	classhandle = (int)theClass.getHandle();
	CDChandle = (int)ohttable.CDC[classhandle];
	for (iter = attributeList.begin(); iter != attributeList.end(); iter++)
	{
		attrhandle = (int)iter->getHandle();
		if(ohttable.Asubscribe[CDChandle][attrhandle])
		{
			isDiscover = true;
			break;
		}
	}
	
	if(isDiscover)
	{
		AllInst[*TopObject] = (InstNameOC){theObjectInstanceName, theClass};



	try{
		FederateAmbassador->discoverObjectInstance(*TopObject, RTI_ObjectClassHandle(CDChandle), theObjectInstanceName);
	}
	catch(std::exception &e){
		printf("%s:%d ->C++std Runtime-Error happened at User's code, which is\n", __FILE__, __LINE__);
		printf("FederateAmbassador->discoverObjectInstance(*TopObject, RTI_ObjectClassHandle(CDChandle), theObjectInstanceName)\n");
		cout << e.what() << endl;
		
	}
	catch(RTI_Exception &e){
		printf("%s:%d ->RTI Runtime-Error happened at User's code,which is\n", __FILE__, __LINE__); 
		printf("FederateAmbassador->discoverObjectInstance(*TopObject, RTI_ObjectClassHandle(CDChandle), theObjectInstanceName)\n");
		cout << e.what() << endl;	
	}

		
	}
	
}

static void LRC_removeObjectInstance(struct header_t *head, struct buffer_t *buffer){
	RTI_ObjectInstanceHandle theObject;
	RTI_UserSuppliedTag theUserSuppliedTag;
	RTI_ObjectClassHandle theClass; 
	RTI_OrderType sentOrder = RTI_RECEIVE;
	
	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
	/* decoded*/
	decode_RTI_ObjectInstanceHandle(theObject, &buffer[0]);
	decode_RTI_UserSuppliedTag(theUserSuppliedTag, &buffer[1]);
	decode_RTI_ObjectClassHandle(theClass, &buffer[2]);
	debug(DEBUG_MSGHANDLER, "    object instance = %ld, ", theObject.getHandle());
	debug_p(DEBUG_MSGHANDLER, "object class = %ld\n", theClass.getHandle());
	
	/* major code */
	if(AllInst.count(theObject)<=0 || OwnedInst.count(theObject)>=1)
	{
		if( OwnedInst.count(theObject)>=1)
			warning("theObject=%ld that will be remove is in OwnedInst",theObject.getHandle());			
	}
	else
	{
		AllInst.erase(theObject);
		OwnedInst.erase(theObject);	
	}
	
	/* TODO: RTI_OrderType need to send, so need to plus Endecode for RTI_OrderType  */

	try{
		FederateAmbassador->removeObjectInstance(theObject, theUserSuppliedTag, sentOrder);
	}
	catch(std::exception &e){
		printf("%s:%d ->C++std Runtime-Error happened at User's code, which is\n", __FILE__, __LINE__);
		printf("FederateAmbassador->removeObjectInstance(theObject, theUserSuppliedTag, sentOrder);\n");
		cout << e.what() << endl;
		
	}
	catch(RTI_Exception &e){
		printf("%s:%d ->RTI Runtime-Error happened at User's code,which is\n", __FILE__, __LINE__); 
		printf("FederateAmbassador->removeObjectInstance(theObject, theUserSuppliedTag, sentOrder);\n");
		cout << e.what() << endl;	
	}

}



static void LRC_reflectAttributeValues(struct header_t *head, struct buffer_t *buffer)
{
	int i, base;
	struct buffer_list_t attr_list;
	Inst_id objInstID;
	int classhandle, CDChandle, attrhandle;
	RTI_ObjectInstanceHandle theObject;
	RTI_AttributeHandleValueMap theAttributeValues;
	RTI_UserSuppliedTag theUserSuppliedTag;
	RTI_AttributeHandleValueMap::const_iterator iter;
	bool FindObjectIDMap;
	RTI_ObjectInstanceHandle TopObject;
	RTI_ObjectIDMap::iterator pointer;

	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
	
	/* decode RTI_ObjectInstanceHandle */
	decode_RTI_ObjectInstanceHandle(theObject, &buffer[1]);
	debug(DEBUG_MSGHANDLER, "    theObject =  %ld\n", theObject.getHandle() );	
	/* decode RTI_AttributeHandleValueMap */
	base = 2;
	attr_list.num = head->count-base -1;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	
	for(i=0; i<attr_list.num; i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_AttributeHandleValueMap(theAttributeValues, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;

	/* decode RTI_UserSuppliedTag */
	decode_RTI_UserSuppliedTag(theUserSuppliedTag, &buffer[base+i]);

	FindObjectIDMap=false;
	for(pointer=ObjectIDMap.begin();pointer!=ObjectIDMap.end();pointer++)
	{
		    if((pointer->second.vid==head->src.vid)&&(pointer->second.oid.getHandle()==theObject.getHandle()))
			{
				FindObjectIDMap=true;
				TopObject=pointer->first;
				break;
				
			}
	}
	
	objInstID = (Inst_id)TopObject.getHandle();
	if((FindObjectIDMap)&&AllInst.count(objInstID))
	{
		
		classhandle = (int)AllInst[objInstID].OC.getHandle();
		CDChandle = ohttable.CDC[classhandle];

		//debug(DEBUG_OMT, "  (Before)\n");
		//Debug_UpdateAttributeList((int)theObject.getHandle(), theAttributeValues);
	    /* compute reflect_set, which is assigned by interaction of subscribeASet(OC_m) and aSet */
		for(iter=theAttributeValues.begin(); iter!= theAttributeValues.end(); iter++)
		{
			attrhandle = (int)iter->first.getHandle();
			if(ohttable.Asubscribe[CDChandle][attrhandle] == false)
			{
				theAttributeValues.erase(iter->first);
			}
		}
		
		//debug(DEBUG_OMT, "  (After)\n"); 
		//Debug_UpdateAttributeList((int)theObject.getHandle(), theAttributeValues); 
		
		std::auto_ptr<RTI_AttributeHandleValueMap> 
			theAttributeValuesPointer(new RTI_AttributeHandleValueMap(theAttributeValues));
		
		try{
			FederateAmbassador->reflectAttributeValues(TopObject, theAttributeValuesPointer,theUserSuppliedTag, RTI_RECEIVE, RTI_RELIABLE);
		}
		catch(std::exception &e){
			printf("%s:%d ->C++std Runtime-Error happened at User's code, which is\n", __FILE__, __LINE__);
			printf("FederateAmbassador->reflectAttributeValues(TopObject, theAttributeValuesPointer,theUserSuppliedTag, RTI_RECEIVE, RTI_RELIABLE);\n");
			cout << e.what() << endl;
			
		}
		catch(RTI_Exception &e){
			printf("%s:%d ->RTI Runtime-Error happened at User's code,which is\n", __FILE__, __LINE__); 
			printf("FederateAmbassador->reflectAttributeValues(TopObject, theAttributeValuesPointer,theUserSuppliedTag, RTI_RECEIVE, RTI_RELIABLE);\n");
			cout << e.what() << endl;	
		}
	
		delete theAttributeValuesPointer.release();
	}
	else
	{
		warning("the object instance is not registered.\n");
		/* FIXME: do we need to handle this?  Ans: we need although the probability is low.*/
		/* if(LRC never record the such Inst_id) exit & do nothing   ...The probability is low */
	}
}




static void LRC_receiveInteraction(struct header_t *head, struct buffer_t *buffer)
{
	int i, base;
	struct buffer_list_t attr_list;
	RTI_InteractionClassHandle  theInteraction;
	RTI_ParameterHandleValueMap  theParameterValues;
	RTI_UserSuppliedTag  theUserSuppliedTag;
	int classhandle, CDChandle, paramhandle;
	RTI_ParameterHandleValueMap::const_iterator iter;

	debug(DEBUG_MSGHANDLER, "%s entered.\n", __func__);
	
	/* decode RTI_InteractionClassHandle */
	decode_RTI_InteractionClassHandle(theInteraction, &buffer[0]);
	
	/* decode RTI_ParameterHandleValueMap */
	base = 1;
	attr_list.num = head->count-base -1;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];
	for(i=0;i<attr_list.num;i++)
	{
		attr_list.start_list[i] = buffer[base+i].start;
		attr_list.len_list[i] = buffer[base+i].len;
	}
	decode_RTI_ParameterHandleValueMap(theParameterValues, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;

	/* decode RTI_UserSuppliedTag */
	decode_RTI_UserSuppliedTag(theUserSuppliedTag, &buffer[base+i]);
	
	classhandle = (int)theInteraction.getHandle();
	CDChandle = ihttable.CDC[classhandle];
	if(CDChandle != 0)
	{
		Debug_SendInteractionList(classhandle, theParameterValues);
		for(iter=theParameterValues.begin(); iter!= theParameterValues.end(); iter++)
		{
			paramhandle = (int)iter->first.getHandle();
			if(paramhandle <= 0 || paramhandle > (int)ihttable.M[CDChandle])
			{
				theParameterValues.erase(iter->first);
			}
		}
		Debug_SendInteractionList(classhandle, theParameterValues);
		
		std::auto_ptr<RTI_ParameterHandleValueMap>
			theParameterValuesPointer(new RTI_ParameterHandleValueMap(theParameterValues));
		

		try{
			FederateAmbassador->receiveInteraction(theInteraction, theParameterValuesPointer,theUserSuppliedTag, RTI_RECEIVE, RTI_RELIABLE);
		}
		catch(std::exception &e){
			printf("%s:%d ->C++std Runtime-Error happened at User's code, which is\n", __FILE__, __LINE__);
			printf("FederateAmbassador->receiveInteraction(theInteraction, theParameterValuesPointer,theUserSuppliedTag, RTI_RECEIVE, RTI_RELIABLE);\n");
			cout << e.what() << endl;
			
		}
		catch(RTI_Exception &e){
			printf("%s:%d ->RTI Runtime-Error happened at User's code,which is\n", __FILE__, __LINE__); 
			printf("FederateAmbassador->receiveInteraction(theInteraction, theParameterValuesPointer,theUserSuppliedTag, RTI_RECEIVE, RTI_RELIABLE);\n");
			cout << e.what() << endl;	
		}

		delete theParameterValuesPointer.release();
	}
}

/*
 * vim: ts=4 sts=4 sw=4
 */
