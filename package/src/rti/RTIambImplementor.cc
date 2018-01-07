#include <stdint.h>
#include <iostream>
#include <map>
#include <set>
#include "RTIambImplementor.h"
#include "MsgHandler.h"
#include "PubSubTable.h"
#include "OMTDebug.h"
#include "macro.h"

extern "C"
{
#include "debug.h"
}

#define catchSTL()\
		char a[500];\
		const char *p2;\
		const char *ret;\
		char *p1;\
		p1=&a[0];\
		p2=__FILE__;\
		while(p1=='\0')*p1++=*p2++;\
		p2=":"; \
		while(p1=='\0')*p1++=*p2++;\
		p2=((*new string)+=__LINE__).c_str();\
		while(p1=='\0')*p1++=*p2++;\
		p2="->STL exception";\
		while(p1=='\0')*p1++=*p2++;\
		p2=e.what();\
		while(p1=='\0')*p1++=*p2++;	\
		ret=&a[0];\
		throw RTI_InternalError(ret)

#define catchFEDAMB()\
		char a[500];\
		const char *p2;\
		const char *ret;\
		char *p1;\
		p1=&a[0];\
		p2=__FILE__;\
		while(p1=='\0')*p1++=*p2++;\
		p2=":"; \
		while(p1=='\0')*p1++=*p2++;\
		p2=((*new string)+=__LINE__).c_str();\
		while(p1=='\0')*p1++=*p2++;\
		p2="->FedAmb exception";\
		while(p1=='\0')*p1++=*p2++;\
		p2=e.what();\
		while(p1=='\0')*p1++=*p2++;	\
		ret=&a[0];\
		throw RTI_InternalError(ret)


using namespace std;

RTI_auto_ptr<RTI_FederateAmbassador> FederateAmbassador;
MS_Communicator* RTI_comm;				/* for sending data from LRC to fedex					*/
federateID RTI_handle;					/* the Federate handle is for itself					*/
std::map<RTI_wstring, int> synpointset;	/* The set record all of Synchronization Point			*/

OHT<int> *oht;							/* parsing tree of object class tree					*/
IHT<int> *iht;							/* parsing tree of interaction class tree				*/

OHTTable ohttable;						/* P/S state of object class tree						*/
IHTTable ihttable;						/* P/S state of interaction class tree					*/
InstMap AllInst;						/* info of all of discovery&registed object instance	*/
InstSet OwnedInst;						/* all registed object instance							*/
InstNameSet ReserveNames;				/* The set of name that be reserveed but not used		*/
InstNameSet InUseNames;					/* The set of name that be reserved and used			*/
RTI_ObjectIDMap ObjectIDMap;			/* it afford you to use TopObject to find (vid,oid)		*/
int ObjectIDCount=0;					/* LRC has its owned counter to make unique TopObject	*/





////////////////////////////////////
// Federation Management Services //
////////////////////////////////////

// 4.2
void RTIambImplementor::createFederationExecution
	(RTI_wstring const & federationExecutionName,
	 RTI_wstring const & fullPathNameToTheFDDfile)
	throw (RTI_FederationExecutionAlreadyExists,
			RTI_CouldNotOpenFDD,
			RTI_ErrorReadingFDD,
			RTI_RTIinternalError)
{
	char path[100] = {'\0'};

	/* Start debug mode and timer in debug messages */
	set_debugmode(DEBUG_RTI | DEBUG_OMT | DEBUG_MSGHANDLER);
	init_time();
	debug(DEBUG_FM, "%s: timer initialized.\n", __func__);
	
	THROW_HAS_INITIALIZED();

	/* Initialize OHT and IHT parsing */
	oht = new OHT<int>();
	iht = new IHT<int>();

	if (WStrtoStr(path, fullPathNameToTheFDDfile, 99) == (size_t)-1)
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_RTIinternalError("Conversion of wstring error.");
	}
	if (oht->read_XMLfile(path) == 0 || iht->read_XMLfile(path) == 0)
	{
		printf("%s:%d ->RTI_CouldNotOpenFDD\n", __FILE__, __LINE__);
		throw RTI_CouldNotOpenFDD("Could not open FDD file.");
	}
	if (oht->parsing() == 0 || iht->parsing()==0)
	{	
		printf("%s:%d ->RTI_ErrorReadingFDD\n", __FILE__, __LINE__);
		throw RTI_ErrorReadingFDD("Error reading FDD file.");
	}
	initialize_ohttable(oht, &ohttable);
	initilaize_ihttable(iht, &ihttable);

	debug(DEBUG_FM, "%s: OMT initialized.\n", __func__);
	
	FederationName = federationExecutionName;
	is_initialized = RTI_true;
}

// 4.3
void RTIambImplementor::destroyFederationExecution
	(RTI_wstring const & federationExecutionName)
	throw (RTI_FederatesCurrentlyJoined,
			RTI_FederationExecutionDoesNotExist,
			RTI_RTIinternalError)
{
	debug(DEBUG_FM, "%s: entered\n", __func__);
	
	THROW_NOT_INITIALIZED();
	THROW_CURRENTLY_JOINED();

	if (oht)
		delete oht;
	if (iht)
		delete iht;
	
	FederationName = L"";
	is_initialized = RTI_false;
}

// 4.4
RTI_FederateHandle const & RTIambImplementor::joinFederationExecution
	(RTI_wstring const & federateType,
	 RTI_wstring const & federationExecutionName,
	 RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador,
	 RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory,
	 RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory)
	throw (RTI_FederateAlreadyExecutionMember,
			RTI_FederationExecutionDoesNotExist,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	MsgHandler *msghandler = NULL;
	struct buffer_t buf;
	char *buffer;
	char *data;
	uint32_t size;
	EndecodeFactory enc(JoinFederationExecution);
	
	THROW_HAS_JOINED();
	
	/* link up the federate ambassador */
	FederateAmbassador = federateAmbassador;

	msghandler = new MsgHandler();
	RTI_comm = new MS_Communicator();
	if (msghandler == NULL || RTI_comm == NULL)
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_RTIinternalError("Not enough memory space.");
	}
	
	RTI_comm->Init(msghandler);
	RTI_comm->Join("", &RTI_handle);

	/* TODO: change this fixed id method */
	enc.create(2);
	encode_wstring(federateType, &buf);
	enc.add(&buf);
	encode_wstring(federationExecutionName, &buf);
	enc.add(&buf);
	enc.set_src(&RTI_handle);
	RTI_comm->SendCommand(JoinFederationExecution, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
	
	/* wait for federate id from fedex */
	RTI_comm->RecvCommandSize(&size);
	buffer = new char[size];
	RTI_comm->RecvCommand(buffer, size);
	data = buffer + sizeof(struct header_t) + sizeof(uint32_t);
	memcpy(&RTI_handle, data, sizeof(RTI_handle));
	delete [] buffer;
	
	debug(DEBUG_FM, "%s: get myid = (%d %d).\n", __func__, RTI_handle.vid, RTI_handle.fid);
	
	handle = new RTI_FederateHandle(RTI_handle.fid);
	is_joined = RTI_true;
	
	return *handle;
}
	
// 4.5
void RTIambImplementor::resignFederationExecution
	(RTI_ResignAction resignAction)
	throw (RTI_OwnershipAcquisitionPending,
			RTI_FederateOwnsAttributes,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	uint32_t action;
	EndecodeFactory enc(ResignFederationExecution);

	debug(DEBUG_FM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();
	
	action = resignAction.getAction();
	
	/* the action is embedded in the usertag, so we just allocate 'zero' element */
	enc.create(0);
	enc.set_src(&RTI_handle);
	enc.set_usertag((int32_t)action);
	
	RTI_comm->SendCommand(ResignFederationExecution, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
	
	RTI_comm->Leave();
	
	if (RTI_comm)
		delete RTI_comm;
	is_joined = RTI_false;
}


// 4.6
void RTIambImplementor::registerFederationSynchronizationPoint
	(RTI_wstring const & label,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	RTI_FederateHandleSet nullSet;

	THROW_NOT_JOINED();	

	try{
		registerFederationSynchronizationPoint(label, theUserSuppliedTag, nullSet);
	}
	catch(RTI_Exception &e){
			printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
			throw RTI_RTIinternalError("RTI_RTIinternalError");
	}
}

void RTIambImplementor::registerFederationSynchronizationPoint
	(RTI_wstring const & label,
	 RTI_UserSuppliedTag const & theUserSuppliedTag,
	 RTI_FederateHandleSet const & syncSet)
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	int handlesize;
	EndecodeFactory enc(RegisterFederationSynchronizationPoint);
	RTI_FederateHandleSet::iterator pointer;

	debug(DEBUG_FM, "%s entered.\n", __func__);

	THROW_NOT_JOINED();	

	handlesize = syncSet.size();
	
	enc.create(2 + syncSet.size());
	enc.set_src(&RTI_handle);
	encode_wstring(label, &buf);
	enc.add(&buf);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	if (syncSet.size() != 0)
	{
		encode_RTI_FederateHandleSet(syncSet, &buf_list);
		enc.add(&buf_list);
	}

	/* The size of syncSet */
	enc.set_usertag(syncSet.size());

	RTI_comm->SendCommand(RegisterFederationSynchronizationPoint, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

// 4.9
void RTIambImplementor::synchronizationPointAchieved
	(RTI_wstring const & label)
	throw (RTI_SynchronizationPointLabelNotAnnounced,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	struct buffer_t buf;
	EndecodeFactory enc(SynchronizationPointAchieved);

	debug(DEBUG_FM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();	

	if(synpointset.count(label) == 0)
	{
		printf("%s:%d ->RTI_SynchronizationPointLabelNotAnnounced\n", __FILE__, __LINE__);
		throw RTI_SynchronizationPointLabelNotAnnounced("Synchronization point label is not announced");
	}
	if(synpointset[label] == 1)
	{
		printf("%s:%d ->RTI_SynchronizationPointLabelNotAnnounced\n", __FILE__, __LINE__);
		throw RTI_SynchronizationPointLabelNotAnnounced("Synchronization point label is Synchronized");
	}
	
	enc.create(1);
	enc.set_src(&RTI_handle);
	encode_wstring(label,&buf);
	enc.add(&buf);
	
	RTI_comm->SendCommand(SynchronizationPointAchieved, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
	 
	/* set synchronized point to be 1 */
	synpointset[label] = 1;
}


#if 0
// 4.11
void requestFederationSave
	(RTI_wstring const & label)
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

void requestFederationSave
	(RTI_wstring const & label,
	 RTI_LogicalTime const & theTime)
	throw (RTI_LogicalTimeAlreadyPassed,
			RTI_InvalidLogicalTime,
			RTI_FederateUnableToUseTime,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 4.13
void federateSaveBegun ()
	throw (RTI_SaveNotInitiated,
			RTI_FederateNotExecutionMember,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 4.14
void federateSaveComplete ()
	throw (RTI_FederateHasNotBegunSave,
			RTI_FederateNotExecutionMember,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

void federateSaveNotComplete()
	throw (RTI_FederateHasNotBegunSave,
			RTI_FederateNotExecutionMember,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 4.16
void queryFederationSaveStatus ()
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveNotInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 4.18
void requestFederationRestore
	(RTI_wstring const & label)
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 4.22
void federateRestoreComplete ()
	throw (RTI_RestoreNotRequested,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RTIinternalError);

void federateRestoreNotComplete ()
	throw (RTI_RestoreNotRequested,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RTIinternalError);

// 4.24
void queryFederationRestoreStatus ()
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreNotInProgress,
			RTI_RTIinternalError);
#endif		

/////////////////////////////////////
// Declaration Management Services //
/////////////////////////////////////

// 5.2
void RTIambImplementor::publishObjectClassAttributes
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSet const & attributeList)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle, attrhandle;
	RTI_AttributeHandleSet::iterator iter;
	int max;
	struct buffer_t buf;
	struct buffer_list_t buf_list;	

	debug(DEBUG_DM, "%s entered.\n", __func__);

	THROW_NOT_JOINED();
	
	classhandle = (int)theClass.getHandle();
	if(classhandle <= 0 || (int)classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}
	try{	
		Debug_AttributeList(classhandle, attributeList);
	}
	catch(exception &e){
		catchSTL();
	}

	/* check precondition and renew P/S state of LRC*/
	max = oht->getNumberOfAttribute(classhandle);
	try{
		for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
		{
			attrhandle = iter->getHandle();
			if(attrhandle <= 0 || attrhandle > max)
			{
				printf("%s:%d ->RTI_AttributeNotDefined\n", __FILE__, __LINE__);
				throw RTI_AttributeNotDefined("RTI_AttributeNotDefined");
			}
			ohttable.Apublish[classhandle][attrhandle] = true;
			ohttable.OCpublish[classhandle] = true;
		}
	}
	catch(exception &e){
		catchSTL();
	}
	try{
		Debug_PublishedAttributeList(&ohttable, classhandle);
	}
	catch(exception  &e){
		catchSTL();
	}

	/* send data to Fedexec */
	EndecodeFactory enc(PublishObjectClassAttributes);
	
	enc.create(1 + attributeList.size()); 
	enc.set_src(&RTI_handle);
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleSet(attributeList, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);
	
	RTI_comm->SendCommand(PublishObjectClassAttributes, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

// 5.3
void RTIambImplementor::unpublishObjectClass
	(RTI_ObjectClassHandle const & theClass)
	throw (RTI_ObjectClassNotDefined,
			RTI_OwnershipAcquisitionPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int i, max;
	int classhandle;
	struct buffer_t buf;

	debug(DEBUG_DM, "%s entered.\n", __func__);
	/* check precondition and renew P/S state of LRC */
	THROW_NOT_JOINED();

	classhandle = (int)theClass.getHandle();
	if(classhandle <= 0 || classhandle > (int)oht->getClassNodeCount() )
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}
	
	max = oht->getNumberOfAttribute(classhandle);
	
	for(i=1; i<=max; i++)
	{
		ohttable.Apublish[classhandle][i] = false;
	}
	ohttable.OCpublish[classhandle] = false;
	try{	
		Debug_PublishedAttributeList(&ohttable, classhandle);
	}
	catch(exception &e){
		catchSTL();
	}
	
	/* send data to fedexec */
	EndecodeFactory enc(UnpublishObjectClass);
	
	enc.create(1);
    encode_RTI_ObjectClassHandle(theClass, &buf);
    enc.add(&buf);
    enc.set_src(&RTI_handle);

    RTI_comm->SendCommand(UnpublishObjectClass, enc.get_buffer_list(),
            enc.get_len_list(), enc.get_size());
}


void RTIambImplementor::unpublishObjectClassAttributes
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSet const & attributeList)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_OwnershipAcquisitionPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle, attrhandle;
	RTI_AttributeHandleSet::iterator iter;
	int i, max;
	struct buffer_t buf;
	struct buffer_list_t buf_list;	

	debug(DEBUG_DM, "%s: entered.\n", __func__);

	THROW_NOT_JOINED();

	classhandle = theClass.getHandle();
	if(classhandle <= 0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}
	try{	
		Debug_AttributeList(classhandle, attributeList);
	}
	catch(exception &e){
		catchSTL();
	}

	/* check precondition and renew P/S state of LRC*/
	max = oht->getNumberOfAttribute(classhandle);
	try{
		for(iter=attributeList.begin();iter!=attributeList.end();iter++)
		{
			attrhandle = iter->getHandle();
			if(attrhandle <= 0 || attrhandle > max)
			{
				printf("%s:%d ->RTI_AttributeNotDefined\n", __FILE__, __LINE__);
				throw RTI_AttributeNotDefined("RTI_AttributeNotDefined");
			}
			ohttable.Apublish[classhandle][attrhandle] = false;
		}
	}
	catch(exception &e){
		catchSTL();
	}

	ohttable.OCpublish[classhandle] = false;
	for(i=1; i<=max; i++)
	{
		if(ohttable.Apublish[classhandle][i])
		{
			ohttable.OCpublish[classhandle] = true;
			break;
		}
	}
	try{
		Debug_PublishedAttributeList(&ohttable, classhandle);
	}
	catch(exception &e){
		catchSTL();
	}
	/* send data to Fedexec */
	EndecodeFactory enc(UnpublishObjectClassAttributes);

	enc.create(1 + attributeList.size()); 
	enc.set_src(&RTI_handle);			
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleSet(attributeList, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}

	enc.add(&buf_list);
	
	RTI_comm->SendCommand(UnpublishObjectClassAttributes, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

// 5.4
void RTIambImplementor::publishInteractionClass
	(RTI_InteractionClassHandle const & theInteraction)
	throw (RTI_InteractionClassNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle;
	struct buffer_t buf;

	debug(DEBUG_DM, "%s entered.\n", __func__);
	/* check precondition and renew P/S state of LRC*/ 
	THROW_NOT_JOINED();

	classhandle = theInteraction.getHandle();

	if(classhandle <=0 || classhandle > (int) iht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InteractionClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_InteractionClassNotDefined("publishInteractionClass: RTI_InteractionClassNotDefined");
	}
	ihttable.ICpublish[classhandle] = true;
	
	/* send data to fedexec */
	EndecodeFactory enc(PublishInteractionClass);
	enc.create(1);
    encode_RTI_InteractionClassHandle(theInteraction, &buf);
    enc.add(&buf);
    enc.set_src(&RTI_handle);
	
    RTI_comm->SendCommand(PublishInteractionClass, enc.get_buffer_list(),
            enc.get_len_list(), enc.get_size());
}

// 5.5
void RTIambImplementor::unpublishInteractionClass
	(RTI_InteractionClassHandle const & theInteraction)
	throw (RTI_InteractionClassNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle;
	struct buffer_t buf;

	debug(DEBUG_DM, "%s entered.\n", __func__);
	/* check precondition and renew P/S state of LRC */
	THROW_NOT_JOINED();
	
	classhandle = theInteraction.getHandle();

	if(classhandle <=0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InteractionClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_InteractionClassNotDefined("unpublishInteractionClass: RTI_InteractionClassNotDefined");
	}
	ihttable.ICpublish[classhandle] = false;
		
	/* send data to fedex */
	EndecodeFactory enc(UnpublishInteractionClass);
	enc.create(1);
    encode_RTI_InteractionClassHandle(theInteraction, &buf);
    enc.add(&buf);
    enc.set_src(&RTI_handle);
	
    RTI_comm->SendCommand(UnpublishInteractionClass, enc.get_buffer_list(),
            enc.get_len_list(), enc.get_size());
}

// 5.6
void RTIambImplementor::subscribeObjectClassAttributes
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSet const & attributeList,
	 RTI_bool active)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle, attrhandle;
	int max;
	bool old;
	RTI_AttributeHandleSet::iterator iter;
	int tail, next;
	struct buffer_t buf;
	struct buffer_list_t buf_list;	

	debug(DEBUG_DM, "%s entered.\n", __func__);
	/* check precondition and renew P/S state of LRC  */
	THROW_NOT_JOINED();

	classhandle = theClass.getHandle();
	if(classhandle <=0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}

	try{
		Debug_AttributeList(classhandle, attributeList);
	}
	catch(exception &e){
		catchSTL();
	}

	old = ohttable.OCsubscribe[classhandle];
	max = oht->getNumberOfAttribute(classhandle);
	try{
		for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
		{
			attrhandle = iter->getHandle();
			if(attrhandle <=0 || attrhandle > max)
			{
				printf("%s:%d ->RTI_AttributeNotDefined\n", __FILE__, __LINE__);
				throw RTI_AttributeNotDefined("RTI_AttributeNotDefined");
			}
			ohttable.Asubscribe[classhandle][attrhandle] = true;
			ohttable.OCsubscribe[classhandle] = true;
		}
	}
	catch(exception &e){
		catchSTL();
	}

	/*
	 *  When OCsubscribe changed, we need to do smart DFS-travsal.
	 *  How smart is that the node we don't need to care is nevever be visit 
	 */
	tail = (int)oht->getClassTailValue(classhandle);
	
	if(old != ohttable.OCsubscribe[classhandle]) /* if OCsubscribe[i]==false -> OCsubscribe'[i]==true */
	{
		ohttable.CDC[classhandle] = classhandle;
		
		/*
		 * Throughout the tree traversal, we follow two rules:
		 * 1. If the class selected has been subscribed, all its subclasses are skipped 
		 *		and the CDC will not be changed.
		 * 2. Otherwise, we set CDC of all subclasses to 'myself'.
		 */
		next = classhandle + 1;
		while (next <= tail)
		{
			if (ohttable.OCsubscribe[next] == true)
			{
				next = oht->getClassTailValue(next)+1;
			}
			else
			{
				ohttable.CDC[next] = classhandle;
				next++;
			}
		}
	}
	
	Display_AllSubscribe(&ohttable);
	Display_AllCDC(&ohttable);
	try{
		Debug_SubscribedAttributeList(&ohttable, classhandle);
	}
	catch(exception &e){
		catchSTL();
	}

	/* send data to Fedexec */
	EndecodeFactory enc(SubscribeObjectClassAttributes);

	enc.create(1 + attributeList.size()); 
	enc.set_src(&RTI_handle);			
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleSet(attributeList, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);

	RTI_comm->SendCommand(SubscribeObjectClassAttributes, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}


//5.7
void RTIambImplementor::unsubscribeObjectClass
	(RTI_ObjectClassHandle const & theClass)
	throw (RTI_ObjectClassNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int i;
	int classhandle, old, max, parent, next, tail;
	struct buffer_t buf;

	debug(DEBUG_DM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	classhandle = theClass.getHandle();
	if(classhandle <=0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}

	old = (int)ohttable.OCsubscribe[classhandle];
	max = (int)oht->getNumberOfAttribute(classhandle);

	/* Set CDC to parent's CDC */
	parent = oht->getClassParentNode(classhandle);
	ohttable.CDC[classhandle] = ohttable.CDC[parent];
	ohttable.OCsubscribe[classhandle] = false;
	
	for(i=1; i<=max; i++)
	{
		ohttable.Asubscribe[classhandle][i] = false;
	}
	
	/* maintain CDC property  */
	
	tail = (int)oht->getClassTailValue(classhandle);
	if(old != (int)ohttable.OCsubscribe[classhandle])
	{
		/*
		 * Throughout the tree traversal, we follow two rules:
		 * 1. If the class selected has been subscribed, all its subclasses are skipped 
		 *		and the CDC will not be changed.
		 * 2. Otherwise, we set CDC of all subclasses to parent's CDC.
		 */
		next = classhandle + 1;
		while (next <= tail)
		{
			if (ohttable.OCsubscribe[next])
			{
				next = oht->getClassTailValue(next)+1;
			}
			else
			{
				ohttable.CDC[next] = ohttable.CDC[parent];
				next++;
			}
		}
	}
	try{
		Debug_SubscribedAttributeList(&ohttable, classhandle);
	}
	catch(exception &e){
		catchSTL();
	}

	/* send data to Fedexec */
	EndecodeFactory enc(UnsubscribeObjectClass);

	enc.create(1); 
	enc.set_src(&RTI_handle);			
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);

	RTI_comm->SendCommand(UnsubscribeObjectClass, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

void RTIambImplementor::unsubscribeObjectClassAttributes
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSet const & attributeList)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int i;
	int classhandle, attrhandle;
	int max, old, parent, next, tail;
	RTI_AttributeHandleSet::iterator iter;
	struct buffer_t buf;
	struct buffer_list_t buf_list;	

	debug(DEBUG_DM, "%s entered.\n", __func__);

	THROW_NOT_JOINED();

	classhandle = theClass.getHandle();

	if(classhandle <=0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}
	try{	
		Debug_AttributeList(classhandle, attributeList);
	}
	catch(exception &e){
		catchSTL();
	}

	/* check precondition and renew P/S state of LRC */
	old = ohttable.OCsubscribe[classhandle];
	max = oht->getNumberOfAttribute(classhandle);
	try{	
		for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
		{
			attrhandle = iter->getHandle();
			if(attrhandle <=0 || attrhandle > max)
			{
				printf("%s:%d ->RTI_AttributeNotDefined\n", __FILE__, __LINE__);
				throw RTI_AttributeNotDefined("RTI_AttributeNotDefined");
			}
			ohttable.Asubscribe[classhandle][attrhandle] = false;
		}
	}
	catch(exception &e){
		catchSTL();
	}
	ohttable.OCsubscribe[classhandle] = false;
	
	for(i=1; i<=max; i++)
	{
		if(ohttable.Asubscribe[classhandle][i] == true)
		{
			ohttable.OCsubscribe[classhandle]=true;
			break;
		}
	}

	/* maintain CDC property */
	parent = oht->getClassParentNode(classhandle);
	tail = oht->getClassTailValue(classhandle);
	if(old != ohttable.OCsubscribe[classhandle]) /* OCsubscribe[i]==true -> OCsubscribe'[i]==false */
	{
		ohttable.CDC[classhandle] = ohttable.CDC[parent];
		next = classhandle + 1;
		while (next <= tail)
		{
			if (ohttable.OCsubscribe[next])
			{
				next = oht->getClassTailValue(next) + 1;
			}
			else
			{
				ohttable.CDC[next] = ohttable.CDC[parent];
				next++;
			}
		}
	}

	Display_AllSubscribe(&ohttable);
	
	/* send data to Fedexec */
	
	EndecodeFactory enc(UnsubscribeObjectClassAttributes);

	enc.create(1 + attributeList.size()); 
	enc.set_src(&RTI_handle);			
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleSet(attributeList, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);

	RTI_comm->SendCommand(UnsubscribeObjectClassAttributes, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

// 5.8
void RTIambImplementor::subscribeInteractionClass
	(RTI_InteractionClassHandle const & theClass,
	 RTI_bool active)
	throw (RTI_InteractionClassNotDefined,
			RTI_FederateServiceInvocationsAreBeingReportedViaMOM,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle;
	int old, tail, next;
	struct buffer_t buf;

	debug(DEBUG_DM, "%s entered.\n", __func__);
	/* check precondition and renew P/S state of LRC */
	THROW_NOT_JOINED();

	classhandle = (int)theClass.getHandle();

	if(classhandle <=0 || classhandle > (int)iht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InteractionClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_InteractionClassNotDefined("subscribeInteractionClass: RTI_InteractionClassNotDefined");
	}

	ihttable.ICsubscribe[classhandle] = true;
	ihttable.CDC[classhandle] = classhandle;

	/* maintain CDC property */
	old = ihttable.ICsubscribe[classhandle];
	tail = iht->getClassTailValue(classhandle);
	if(old != ihttable.ICsubscribe[classhandle]) /* ICsubscribe[i]==false -> ICsubscribe[i]==true */
	{
		next = classhandle + 1;
		while (next <= tail)
		{
			if (ihttable.ICsubscribe[next])
			{
				next = iht->getClassTailValue(next) + 1;
			}
			else
			{
				ihttable.CDC[next] = classhandle;
				next++;
			}
		}
	}

	Display_AllSubscribe(&ihttable);
	Display_AllCDC(&ihttable);
	
	/* send data to Fedexec */
	EndecodeFactory enc(SubscribeInteractionClass);

	enc.create(1); 
	enc.set_src(&RTI_handle);			
	encode_RTI_InteractionClassHandle(theClass, &buf);
	enc.add(&buf);

	RTI_comm->SendCommand(SubscribeInteractionClass, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

// 5.9
void RTIambImplementor::unsubscribeInteractionClass
	(RTI_InteractionClassHandle const & theClass)
	throw (RTI_InteractionClassNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle;
	int old, tail, next, parent;
	struct buffer_t buf;

	debug(DEBUG_DM, "%s entered.\n", __func__);
	/* check precondition and renew P/S state on LRC*/
	THROW_NOT_JOINED();

	classhandle = theClass.getHandle();
	if(classhandle <=0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("unsubscribeInteractionClass: RTI_InteractionClassNotDefined");
	}

	
	old = ihttable.ICsubscribe[classhandle];
	ihttable.ICsubscribe[classhandle] = false;
	
	/* maintain CDC property */

	tail = (int)iht->getClassTailValue(classhandle);
	parent = iht->getClassParentNode(classhandle);
	if(old != ihttable.ICsubscribe[classhandle])	/* ICsubscribe[i]==true -> ICsubscribe'[i]==false */
	{
		next = classhandle + 1;
		while (next <= tail)
		{
			if (ihttable.ICsubscribe[next])
			{
				next = iht->getClassTailValue(next) + 1;
			}
			else
			{
				ihttable.CDC[next] = ihttable.CDC[parent];
				next++;
			}
		}
	}
	
	/* send data to Fedexec */
	EndecodeFactory enc(UnsubscribeInteractionClass);

	enc.create(1); 
	enc.set_src(&RTI_handle);			
	encode_RTI_InteractionClassHandle(theClass, &buf);
	enc.add(&buf);

	RTI_comm->SendCommand(UnsubscribeInteractionClass, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

////////////////////////////////
// Object Management Services //
////////////////////////////////



// 6.2
void RTIambImplementor::reserveObjectInstanceName
	(RTI_wstring const & theObjectInstanceName)
	throw (RTI_IllegalName,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{

	char *buffer;
	struct buffer_t buf;
	uint32_t size;
	struct header_t *head;
	struct buffer_t *data;
	char name[300] = {'\0'};
	RTI_wstring returnObjectInstanceName;
	EndecodeFactory enc(ReserveObjectInstanceName);
	int ReserveNames_count;
	int InUseNames_count;

	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	WStrtoStr(name, theObjectInstanceName, 299);
	debug(DEBUG_OM, "    theObjectInstanceName = L\"%s\"\n", &name[0]);
	
	THROW_NOT_JOINED(); 
	try{	
		ReserveNames_count  = ReserveNames.count(theObjectInstanceName);
	}
	catch(exception &e){
		catchSTL();
	}
	try{
		InUseNames_count = InUseNames.count(theObjectInstanceName);
	}
	catch(exception &e){
		catchSTL();
	}
	
	if(ReserveNames_count>=1 || InUseNames_count>=1)
	{	
		try{
			FederateAmbassador->objectInstanceNameReservationFailed(theObjectInstanceName);
		}
		catch(RTI_Exception &e){
			catchFEDAMB();
		}
		printf("%s:%d ->RTI_IllegalName\n", __FILE__, __LINE__);
		throw RTI_IllegalName("RTI_IllegalName");	
	}
	
		
	if(theObjectInstanceName.length() >= 3 )
	{
		if((name[0]=='H'||name[0]=='h')&&(name[1]=='L'||name[1]=='l')&&(name[2]=='A'||name[2]=='a') )
		{	/* since OMT Def.3.3.1 */
			printf("%s:%d ->RTI_IllegalName ... User-defined name cannot be named like HLa* \n", __FILE__, __LINE__);
			throw RTI_IllegalName("RTI_IllegalName");
		
		}
	}


	enc.create(1);
	encode_wstring(theObjectInstanceName, &buf);
	enc.add(&buf);
	enc.set_usertag(0);	
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(ReserveObjectInstanceName, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());

	RTI_comm->RecvCommandSize(&size);
	buffer = new char[size];
	RTI_comm->RecvCommand(buffer, size);
	head = (struct header_t*)buffer;
	data = new struct buffer_t[head->count];
	DecodeBuffer(buffer, head, data);

	decode_wstring(returnObjectInstanceName, &data[0]);
	WStrtoStr(name, returnObjectInstanceName, 299);
	debug(DEBUG_OM, "    returnObjectInstanceName = L\"%s\"\n", &name[0]);
	
	if(returnObjectInstanceName != theObjectInstanceName)
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_IllegalName("RTI_RTIinternalError");
	}
	if(head->user_tag==1)
	{
		try{
			ReserveNames.insert(theObjectInstanceName);
		}
		catch(exception &e){
			catchSTL();
		}

		try{
			FederateAmbassador->objectInstanceNameReservationSucceeded(theObjectInstanceName);
		}
		catch(RTI_Exception &e){	
			catchFEDAMB();
		}
	}
	else if(head->user_tag==2)
	{
		try{
			FederateAmbassador->objectInstanceNameReservationFailed(theObjectInstanceName);
		}
		catch(RTI_Exception &e){
			catchFEDAMB();
		}
		printf("%s:%d ->RTI_IllegalName\n", __FILE__, __LINE__);
		throw RTI_IllegalName("RTI_IllegalName");
	}
	else 
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_IllegalName("RTI_RTIinternalError");
	}

}




// 6.4
RTI_ObjectInstanceHandle const & RTIambImplementor::registerObjectInstance
	(RTI_ObjectClassHandle const & theClass)
	throw (RTI_ObjectClassNotDefined,
			RTI_ObjectClassNotPublished,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{

	/* const int FEDERATE_PROVIDE_NAME=1; */
	const int RTI_PROVIDE_NAME=2; 
	int i;
	RTI_ObjectInstanceHandle *theObject;
	ObjectInstanceID ObjectID;
	RTI_ObjectInstanceHandle *TopObject;
	int classhandle, max;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	char *buffer;
	uint32_t size;
	struct header_t *head;
	struct buffer_t *data;
	RTI_wstring theObjectInstanceName;
	RTI_AttributeHandleSet attributeList;
	EndecodeFactory enc(RegisterObjectInstance);

	debug(DEBUG_OM, "%s entered.\n", __func__);
	debug(DEBUG_OM, "    theClass = %ld\n", theClass.getHandle());
	
	THROW_NOT_JOINED();

    /* check precondition */
	classhandle = (int)theClass.getHandle();
	
	if(classhandle<=0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}
	if(ohttable.OCpublish[classhandle] == false)
	{
		printf("%s:%d ->RTI_ObjectClassNotPublished\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotPublished("RTI_ObjectClassNotPublished");
	}
	
    /* send <<RegisterObjectInstance(OC_m,InstName,publishASet(OC_m))>> to Fedexec */
	max = (int)oht->getNumberOfAttribute(classhandle);
	for(i=1; i<=max; i++)
	{
		if(ohttable.Apublish[classhandle][i])
		{
			try{
				attributeList.insert(RTI_AttributeHandle((long)i));
			}
			catch(exception &e){
				catchSTL();
			}
		}
	}
	
	/* Send message to Fedex: attribute list, instance name */
	enc.create(1+attributeList.size());
	
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleSet(attributeList, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);
	enc.set_usertag(RTI_PROVIDE_NAME);
	enc.set_src(&RTI_handle);
	RTI_comm->SendCommand(RegisterObjectInstance, enc.get_buffer_list(),
			enc.get_len_list(),	enc.get_size());
		
	/* Wait for fedex sending instance id back */
	RTI_comm->RecvCommandSize(&size);
	buffer = new char[size];
	RTI_comm->RecvCommand(buffer, size);
	head = (struct header_t*)buffer;
	data = new struct buffer_t[head->count];
	DecodeBuffer(buffer, head, data);

	theObject = new RTI_ObjectInstanceHandle();
	decode_RTI_ObjectInstanceHandle(*theObject, &data[0]);
	debug(DEBUG_OM, "    (vid,oid) = %d\n", RTI_handle.vid,(int)theObject->getHandle());

	decode_wstring(theObjectInstanceName, &data[1]);
	debug(DEBUG_OM, "    return theObjectInstanceName = ");
	
	
	/*mapping federate source*/
	ObjectID.vid=RTI_handle.vid;
	ObjectID.oid=*theObject;
	ObjectIDCount++;
	TopObject=new RTI_ObjectInstanceHandle(ObjectIDCount);
	try{
		ObjectIDMap.insert(RTI_ObjectIDMap::value_type(ObjectIDCount,ObjectID));
	}
	catch(exception &e){
		catchSTL();
	}
	debug(DEBUG_OM, "TopObject = %d\n",(int)TopObject->getHandle());
	
	try{
		AllInst[*TopObject] = (InstNameOC){theObjectInstanceName, theClass};
	}
	catch(exception &e){
		catchSTL();
	}


	/* record owner of Inst_id */
	try{
		OwnedInst.insert(*TopObject);
	}
	catch(exception &e){
		catchSTL();
	}


	return *TopObject;	
}


RTI_ObjectInstanceHandle const & RTIambImplementor::registerObjectInstance
	(RTI_ObjectClassHandle const & theClass,
	 RTI_wstring const & theObjectInstanceName)
	throw (RTI_ObjectClassNotDefined,
			RTI_ObjectClassNotPublished,
			RTI_ObjectInstanceNameNotReserved,
			RTI_ObjectInstanceNameInUse,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	
	const int FEDERATE_PROVIDE_NAME=1;
	/* const int RTI_PROVIDE_NAME=2; */
	int i;
	RTI_ObjectInstanceHandle *theObject;
	ObjectInstanceID ObjectID;
	RTI_ObjectInstanceHandle *TopObject;
	int classhandle, max;
	char InstName[200] = {'\0'};
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	char *buffer;
	uint32_t size;
	struct header_t *head;
	struct buffer_t *data;
	char name[300];
	RTI_AttributeHandleSet attributeList;
	EndecodeFactory enc(RegisterObjectInstance);
	int ReserveNames_count;
	int InUseNames_count;

	debug(DEBUG_OM, "%s entered.\n", __func__);
	WStrtoStr(name, theObjectInstanceName, 299); 
	debug(DEBUG_OM, "    theObjectInstanceName = L\"%s\"\n", &name[0]);
	debug(DEBUG_OM, "    theClass = %ld\n", theClass.getHandle());
	
	THROW_NOT_JOINED();

	classhandle = (int)theClass.getHandle();
	
	WStrtoStr(InstName, theObjectInstanceName.c_str(), 199);
	if(classhandle<=0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_ObjectClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotDefined("RTI_ObjectClassNotDefined");
	}
	if(ohttable.OCpublish[classhandle] == false)
	{
		printf("%s:%d ->RTI_ObjectClassNotPublished\n", __FILE__, __LINE__);
		throw RTI_ObjectClassNotPublished("RTI_ObjectClassNotPublished");
	}
	try{
		InUseNames_count = InUseNames.count(theObjectInstanceName);
	}
	catch(exception &e){
		catchSTL();
	}
	
	if(InUseNames_count >= 1)
	{
		printf("%s:%d ->RTI_ObjectInstanceNameInUse\n", __FILE__, __LINE__);
		throw RTI_ObjectInstanceNameInUse("RTI_ObjectInstanceNameInUse");
	}
	try{
		ReserveNames_count = ReserveNames.count(theObjectInstanceName);
	}
	catch(exception &e){
		catchSTL();
	}

	if(ReserveNames_count<=0)
	{
		printf("%s:%d ->RTI_ObjectInstanceNameNotReserved\n", __FILE__, __LINE__);
		throw RTI_ObjectInstanceNameNotReserved("RTI_ObjectInstanceNameNotReserved");
	}
	try{	
		ReserveNames.erase(theObjectInstanceName);
	}
	catch(exception &e){
		catchSTL();
	}
	try{
		InUseNames.insert(theObjectInstanceName);
	}
	catch(exception &e){
		catchSTL();
	}
	
    /* send <<RegisterObjectInstance(OC_m,InstName,publishASet(OC_m))>>to Fedexec */
	max = (int)oht->getNumberOfAttribute(classhandle);
	for(i=1; i<=max; i++)
	{
		if(ohttable.Apublish[classhandle][i])
		{
			try{
				attributeList.insert(RTI_AttributeHandle((long)i));
			}
			catch(exception &e){
				catchSTL();
			}
		}
	}
	
	/* Send message to Fedex: attribute list, instance name */
	enc.create(2+attributeList.size());
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);
	encode_wstring(theObjectInstanceName, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleSet(attributeList, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);
	enc.set_usertag(FEDERATE_PROVIDE_NAME);	
	enc.set_src(&RTI_handle);
	RTI_comm->SendCommand(RegisterObjectInstance, enc.get_buffer_list(),
			enc.get_len_list(),	enc.get_size());
		
	/* Wait for fedex sending instance id back */
	RTI_comm->RecvCommandSize(&size);
	buffer = new char[size];
	RTI_comm->RecvCommand(buffer, size);
	head = (struct header_t*)buffer;
	data = new struct buffer_t[head->count];
	DecodeBuffer(buffer, head, data);

	theObject = new RTI_ObjectInstanceHandle();
	decode_RTI_ObjectInstanceHandle(*theObject, &data[0]);
	debug(DEBUG_OM, "    (vid,oid) = ( %d,%d)\n", RTI_handle.vid,(int)theObject->getHandle());
		
    /*mapping federate source*/
    ObjectID.vid=RTI_handle.vid;//head->src.vid;
    ObjectID.oid=*theObject;
    ObjectIDCount++;
    TopObject=new RTI_ObjectInstanceHandle(ObjectIDCount);
	try{
	    ObjectIDMap.insert(RTI_ObjectIDMap::value_type(ObjectIDCount,ObjectID));
	}
	catch(exception &e){
		catchSTL();
	}
	
    debug(DEBUG_OM, "TopObject = %d\n",(int)TopObject->getHandle());
	
	try{
		AllInst[*TopObject] = (InstNameOC){theObjectInstanceName, theClass};
	}
	catch(exception &e){
		catchSTL();
	}

	try{
		OwnedInst.insert(*TopObject);
	}
	catch(exception &e){
		catchSTL();
	}

	
	return *TopObject;
}

// 6.6
void RTIambImplementor::updateAttributeValues
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleValueMap const & theAttributeValues,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int i;
	int max;
	int num_map;
	Inst_id objInstID;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	EndecodeFactory enc(UpdateAttributeValues);
	char *buffer;
	uint32_t size;
	struct header_t *head;
	struct buffer_t *data;
	RTI_AttributeHandleSet local_set;
	struct buffer_list_t attr_list;
	int cc;
	int OwnedInst_count;
	debug(DEBUG_OM, "%s entered.\n", __func__);

	THROW_NOT_JOINED();

	max = (int)oht->getClassNodeCount();
	for(i=1; i<=max; i++)
	{
		try{
			Debug_PublishedAttributeList(&ohttable, i);
		}
		catch(exception &e){
			catchSTL();
		}
	}
	
	objInstID = (Inst_id)theObject.getHandle();
	try{
		OwnedInst_count = OwnedInst.count(theObject);
	}
	catch(exception &e){
		catchSTL();
	}
	
	if(!OwnedInst_count)
	{
		printf("%s:%d ->RTI_ObjectInstanceNotKnown\n", __FILE__, __LINE__);
		throw RTI_ObjectInstanceNotKnown("RTI_ObjectInstanceNotKnown");
	}
	try{
		Debug_UpdateAttributeList((int)theObject.getHandle(), theAttributeValues);
	}
	catch(exception &e){
		catchSTL();
	}
	
	debug(DEBUG_OM, "TopObject = %d\n",(int)theObject.getHandle());
	debug(DEBUG_OM, "(top,(vid,oid)) = (%d,(%d,%d))\n",(int)theObject.getHandle(),
			(int)ObjectIDMap[theObject].vid,(int)ObjectIDMap[theObject].oid.getHandle());
	/* Send AttributeValues to Fedex */
	num_map = theAttributeValues.size();
	enc.create(num_map*2+3);
	encode_RTI_ObjectClassHandle(AllInst[theObject].OC, &buf);
	enc.add(&buf);
	encode_RTI_ObjectInstanceHandle(ObjectIDMap[theObject].oid, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleValueMap(theAttributeValues, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	enc.set_usertag(0);	/* 0 for no time provided */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(UpdateAttributeValues, enc.get_buffer_list(),	enc.get_len_list(), enc.get_size());
	RTI_comm->RecvCommandSize(&size);
	buffer = new char[size];
	RTI_comm->RecvCommand(buffer, size);
	head = (struct header_t*)buffer;
	if(head->user_tag!=-1)
	{
		data = new struct buffer_t[head->count];
		DecodeBuffer(buffer, head, data);
		attr_list.num = head->count;
		attr_list.start_list = new void*[attr_list.num];
		attr_list.len_list = new uint32_t[attr_list.num];

		for(cc=0;cc<attr_list.num; cc++)
		{
			 attr_list.start_list[cc] = data[cc].start;
		     attr_list.len_list[cc] = data[cc].len;
		}
		decode_RTI_AttributeHandleSet(local_set, &attr_list);
		delete [] attr_list.start_list;
		delete [] attr_list.len_list;

		enc.set_type(ReflectAttributeValues);
	
		RTI_AttributeHandleSet::iterator niter;
		for(niter=local_set.begin(); niter!=local_set.end(); niter++)
		{
			debug(DEBUG_OM,"send fed %d\n",(int)niter->getHandle());
			RTI_comm->SendToNode(niter->getHandle(),enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}
	}
    
}

RTI_auto_ptr< RTI_MessageRetractionHandle > RTIambImplementor::updateAttributeValues
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleValueMap const & theAttributeValues,
	 RTI_UserSuppliedTag const & theUserSuppliedTag,
	 RTI_LogicalTime const & theTime)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_InvalidLogicalTime,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int num_map;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	EndecodeFactory enc(UpdateAttributeValues);
	RTI_auto_ptr< RTI_MessageRetractionHandle >	retractionHandle;
	char *buffer;
	uint32_t size;
	struct header_t *head;
	struct buffer_t *data;
	RTI_AttributeHandleSet local_set;
	struct buffer_list_t attr_list;
	int cc;
	RTI_AttributeHandleSet::iterator niter;



	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	num_map = theAttributeValues.size();

	enc.create(num_map*2+2);	/* for each map element, it contains two classes, so we muliply it by 2 */
	encode_RTI_ObjectInstanceHandle(theObject, &buf);
	enc.add(&buf);
	try{
		encode_RTI_AttributeHandleValueMap(theAttributeValues, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	
	enc.add(&buf_list);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	enc.set_usertag(1);	/* tag 1 for time provided */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(UpdateAttributeValues, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());

	RTI_comm->RecvCommandSize(&size);
	buffer = new char[size];
	RTI_comm->RecvCommand(buffer, size);
	head = (struct header_t*)buffer;
	data = new struct buffer_t[head->count];
	DecodeBuffer(buffer, head, data);
	attr_list.num = head->count;
	attr_list.start_list = new void*[attr_list.num];
	attr_list.len_list = new uint32_t[attr_list.num];

	for(cc=0;cc<attr_list.num; cc++)
	{
		attr_list.start_list[cc] = data[cc].start;
		attr_list.len_list[cc] = data[cc].len;
	}
	decode_RTI_AttributeHandleSet(local_set, &attr_list);
	delete [] attr_list.start_list;
	delete [] attr_list.len_list;
	enc.set_type(ReflectAttributeValues);
	for(niter=local_set.begin(); niter!=local_set.end(); niter++)
	{
		debug(DEBUG_OM,"send fed %d\n",(int)niter->getHandle());
		RTI_comm->SendToNode(niter->getHandle(),enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
	}
	return retractionHandle;
}

// 6.8
void RTIambImplementor::sendInteraction
	(RTI_InteractionClassHandle const & theInteraction,
	 RTI_ParameterHandleValueMap const & theParameterValues,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_InteractionClassNotPublished,
			RTI_InteractionClassNotDefined,
			RTI_InteractionParameterNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int classhandle;
	int max;
	int num_map;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	EndecodeFactory enc(SendInteraction);
	RTI_ParameterHandleValueMap::const_iterator iter;
	char *buffer;
	uint32_t size;
	struct header_t *head;
	struct buffer_t *data;
	RTI_AttributeHandleSet local_set;
	RTI_AttributeHandleSet::iterator niter;
	struct buffer_list_t attr_list;
	int cc;


	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	classhandle = (int)theInteraction.getHandle();
	
	if(classhandle <=0 || classhandle > (int)iht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InteractionClassNotDefined\n", __FILE__, __LINE__);
		throw RTI_InteractionClassNotDefined("sendInteraction: RTI_InteractionClassNotDefined");
	}
	
	if(ihttable.ICpublish[classhandle] == false)
	{
		printf("%s:%d ->RTI_InteractionClassNotPublished\n", __FILE__, __LINE__);
		throw RTI_InteractionClassNotPublished("RTI_InteractionClassNotPublished");
	}
	max = (int)iht->getNumberOfParameter(classhandle);
	try{
		for(iter=theParameterValues.begin(); iter!=theParameterValues.end(); iter++)
		{
			if(iter->first.getHandle()<=0 || iter->first.getHandle() > max)
			{
				printf("%s:%d ->RTI_InteractionParameterNotDefined\n", __FILE__, __LINE__);
				throw RTI_InteractionParameterNotDefined("RTI_InteractionParameterNotDefined");
			}
		}
	}
	catch(exception &e){
		catchSTL();
	}
	try{
		Debug_SendInteractionList(classhandle, theParameterValues);
	}
	catch(exception &e){
		catchSTL();
	}
	
	num_map = theParameterValues.size();
	enc.create(num_map*2+2);	/* for each map element, it contains two classes, so we muliply it by 2 */
	encode_RTI_InteractionClassHandle(theInteraction, &buf);
	enc.add(&buf);
	try{
		encode_RTI_ParameterHandleValueMap(theParameterValues, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	enc.set_usertag(0);	/* tag 0 for no time provided */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(SendInteraction, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
	RTI_comm->RecvCommandSize(&size);
	buffer = new char[size];
	RTI_comm->RecvCommand(buffer, size);
	head = (struct header_t*)buffer;
	if(head->user_tag!=-1)
	{
		data = new struct buffer_t[head->count];
		DecodeBuffer(buffer, head, data);
		attr_list.num = head->count;
		attr_list.start_list = new void*[attr_list.num];
		attr_list.len_list = new uint32_t[attr_list.num];

		for(cc=0;cc<attr_list.num; cc++)
		{
			attr_list.start_list[cc] = data[cc].start;
			attr_list.len_list[cc] = data[cc].len;
		}
		decode_RTI_AttributeHandleSet(local_set, &attr_list);
		delete [] attr_list.start_list;
		delete [] attr_list.len_list;

		enc.set_type(ReceiveInteraction);
		for(niter=local_set.begin(); niter!=local_set.end(); niter++)
		{
			debug(DEBUG_OM,"send fed %d\n",(int)niter->getHandle());
			RTI_comm->SendToNode(niter->getHandle(),enc.get_buffer_list(), enc.get_len_list(), enc.get_size());
		}
	}
}

RTI_auto_ptr< RTI_MessageRetractionHandle > RTIambImplementor::sendInteraction
	(RTI_InteractionClassHandle const & theInteraction,
	 RTI_ParameterHandleValueMap const & theParameterValues,
	 RTI_UserSuppliedTag const & theUserSuppliedTag,
	 RTI_LogicalTime const & theTime)
	throw (RTI_InteractionClassNotPublished,
			RTI_InteractionClassNotDefined,
			RTI_InteractionParameterNotDefined,
			RTI_InvalidLogicalTime,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int num_map;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	EndecodeFactory enc(SendInteraction);
	RTI_auto_ptr< RTI_MessageRetractionHandle > retractionHandle;

	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	num_map = theParameterValues.size();

	enc.create(num_map*2+2);	/* for each map element, it contains two classes, so we muliply it by 2 */
	encode_RTI_InteractionClassHandle(theInteraction, &buf);
	enc.add(&buf);
	try{
		encode_RTI_ParameterHandleValueMap(theParameterValues, &buf_list);
	}
	catch(exception &e){
		catchSTL();
	}
	enc.add(&buf_list);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	enc.set_usertag(1);	/* tag 1 for time provided */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(SendInteraction, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());

	return retractionHandle;
}

// 6.10
void RTIambImplementor::deleteObjectInstance
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_DeletePrivilegeNotHeld,
			RTI_ObjectInstanceNotKnown,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{

	Inst_id objInstID;
	struct buffer_t buf;
	EndecodeFactory enc(DeleteObjectInstance);
	int AllInst_count;
	int OwnedInst_count;
	debug(DEBUG_OM, "%s entered.\n", __func__);
	debug(DEBUG_OM, "    object instance = %ld, ", theObject.getHandle());


	THROW_NOT_JOINED();

	objInstID = (Inst_id)theObject.getHandle();
	try{
		AllInst_count = AllInst.count(objInstID);
	}
	catch(exception &e){
		catchSTL();
	}
	if(AllInst_count <= 0)
	{
		printf("%s:%d ->RTI_ObjectInstanceNotKnown\n", __FILE__, __LINE__);
		throw RTI_ObjectInstanceNotKnown("RTI_ObjectInstanceNotKnown");
	}
	try{
		OwnedInst_count = OwnedInst.count(objInstID);
	}
	catch(exception &e){
		catchSTL();
	}

	if(OwnedInst_count <= 0)
	{
		printf("%s:%d ->RTI_DeletePrivilegeNotHeld\n", __FILE__, __LINE__);
		throw RTI_DeletePrivilegeNotHeld("RTI_DeletePrivilegeNotHeld");
	}
	debug_p(DEBUG_OM, "object class = %ld\n", AllInst[theObject].OC.getHandle());

	/*mapping buttom object*/

	
	enc.create(3);
	encode_RTI_ObjectInstanceHandle(theObject, &buf);
	enc.add(&buf);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	encode_RTI_ObjectClassHandle(AllInst[theObject].OC, &buf);
	enc.add(&buf);
	
	enc.set_usertag(0); /* tag 0 for no time provided */
	enc.set_src(&RTI_handle);
	RTI_comm->SendCommand(DeleteObjectInstance, enc.get_buffer_list(), enc.get_len_list(), enc.get_size());


	try{
		OwnedInst.erase(objInstID);
	}
	catch(exception &e){
		catchSTL();
	}
	try{
		AllInst.erase(objInstID);
	}
	catch(exception &e){
		catchSTL();
	}
	


}

RTI_auto_ptr< RTI_MessageRetractionHandle > RTIambImplementor::deleteObjectInstance
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_UserSuppliedTag const & theUserSuppliedTag,
	 RTI_LogicalTime const & theTime)
	throw (RTI_DeletePrivilegeNotHeld,
			RTI_ObjectInstanceNotKnown,
			RTI_InvalidLogicalTime,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	struct buffer_t buf;
	EndecodeFactory enc(DeleteObjectInstance);
	RTI_auto_ptr< RTI_MessageRetractionHandle > retractionHandle;

	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	enc.create(2);	/* for each map element, it contains two classes, so we muliply it by 2 */
	encode_RTI_ObjectInstanceHandle(theObject, &buf);
	enc.add(&buf);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	enc.set_usertag(1);	/* tag 1 for time provided */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(DeleteObjectInstance, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());

	return retractionHandle;
}

#if 0
// 6.12
void localDeleteObjectInstance
	(RTI_ObjectInstanceHandle const & theObject)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_FederateOwnsAttributes,
			RTI_OwnershipAcquisitionPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);
#endif

// 6.13
void RTIambImplementor::changeAttributeTransportationType
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes,
	 RTI_TransportationType const & theType)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int num_set;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	uint32_t transType;
	EndecodeFactory enc(ChangeAttributeTransportationType);

	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	num_set = theAttributes.size();
	
	transType = theType.getType();
	
	enc.create(num_set+1);
	encode_RTI_ObjectInstanceHandle(theObject, &buf);
	enc.add(&buf);
	encode_RTI_AttributeHandleSet(theAttributes, &buf_list);
	enc.add(&buf_list);
	enc.set_usertag(transType);	/* embedd the transType in the usertag */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(ChangeAttributeTransportationType, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

// 6.14
void RTIambImplementor::changeInteractionTransportationType
	(RTI_InteractionClassHandle const & theClass,
	 RTI_TransportationType const & theType)
	throw (RTI_InteractionClassNotDefined,
			RTI_InteractionClassNotPublished,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	struct buffer_t buf;
	uint32_t transType;
	EndecodeFactory enc(ChangeInteractionTransportationType);
	
	debug(DEBUG_OM, "%s entered.\n", __func__);

	THROW_NOT_JOINED();

	transType = theType.getType();
	
	enc.create(1);
	encode_RTI_InteractionClassHandle(theClass, &buf);
	enc.add(&buf);
	enc.set_usertag(transType);	/* embedd the transType in the usertag */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(ChangeInteractionTransportationType, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

// 6.17
void RTIambImplementor::requestAttributeValueUpdate
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int num_set;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	EndecodeFactory enc(RequestAttributeValueUpdate);

	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	num_set = theAttributes.size();
	
	enc.create(num_set+2);
	encode_RTI_ObjectInstanceHandle(theObject, &buf);
	enc.add(&buf);
	encode_RTI_AttributeHandleSet(theAttributes, &buf_list);
	enc.add(&buf_list);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	enc.set_usertag(0);		/* 0 for ObjectInstanceHandle */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(RequestAttributeValueUpdate, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

void RTIambImplementor::requestAttributeValueUpdate
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSet const & theAttributes,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError)
{
	int num_set;
	struct buffer_t buf;
	struct buffer_list_t buf_list;
	EndecodeFactory enc(RequestAttributeValueUpdate);

	debug(DEBUG_OM, "%s entered.\n", __func__);
	
	THROW_NOT_JOINED();

	num_set = theAttributes.size();
	
	enc.create(num_set+2);
	encode_RTI_ObjectClassHandle(theClass, &buf);
	enc.add(&buf);
	encode_RTI_AttributeHandleSet(theAttributes, &buf_list);
	enc.add(&buf_list);
	encode_RTI_UserSuppliedTag(theUserSuppliedTag, &buf);
	enc.add(&buf);
	enc.set_usertag(1);		/* 1 for ObjectClassHandle */
	enc.set_src(&RTI_handle);
	
	RTI_comm->SendCommand(RequestAttributeValueUpdate, enc.get_buffer_list(),
			enc.get_len_list(), enc.get_size());
}

#if 0
///////////////////////////////////
// Ownership Management Services //
///////////////////////////////////

// 7.2
void unconditionalAttributeOwnershipDivestiture
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.3
void negotiatedAttributeOwnershipDivestiture
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_AttributeAlreadyBeingDivested,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.6
void confirmDivestiture
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_auto_ptr< RTI_AttributeHandleSet >securedAttributes,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_AttributeDivestitureWasNotRequested,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.8
void attributeOwnershipAcquisition
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & desiredAttributes,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_ObjectClassNotPublished,
			RTI_AttributeNotDefined,
			RTI_AttributeNotPublished,
			RTI_FederateOwnsAttributes,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.9
void attributeOwnershipAcquisitionIfAvailable
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & desiredAttributes)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_ObjectClassNotPublished,
			RTI_AttributeNotDefined,
			RTI_AttributeNotPublished,
			RTI_FederateOwnsAttributes,
			RTI_AttributeAlreadyBeingAcquired,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);
// 7.12
RTI_auto_ptr< RTI_AttributeHandleSet >
	attributeOwnershipDivestitureIfWanted
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.13
void cancelNegotiatedAttributeOwnershipDivestiture
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_AttributeDivestitureWasNotRequested,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.14
void cancelAttributeOwnershipAcquisition
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeAlreadyOwned,
			RTI_AttributeAcquisitionWasNotRequested,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.16
void queryAttributeOwnership
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandle const & theAttribute)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 7.18
RTI_bool isAttributeOwnedByFederate
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandle const & theAttribute)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

//////////////////////////////
// Time Management Services //
//////////////////////////////

// 8.2
void enableTimeRegulation
	(RTI_LogicalTimeInterval const & theLookahead)
	throw (RTI_TimeRegulationAlreadyEnabled,
			RTI_InvalidLookahead,
			RTI_InTimeAdvancingState,
			RTI_RequestForTimeRegulationPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.4
void disableTimeRegulation ()
	throw (RTI_TimeRegulationIsNotEnabled,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.5
void enableTimeConstrained ()
	throw (RTI_TimeConstrainedAlreadyEnabled,
			RTI_InTimeAdvancingState,
			RTI_RequestForTimeConstrainedPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.7
void disableTimeConstrained ()
	throw (RTI_TimeConstrainedIsNotEnabled,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.8
void timeAdvanceRequest
	(RTI_LogicalTime const & theTime)
	throw (RTI_InvalidLogicalTime,
			RTI_LogicalTimeAlreadyPassed,
			RTI_InTimeAdvancingState,
			RTI_RequestForTimeRegulationPending,
			RTI_RequestForTimeConstrainedPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.9
void timeAdvanceRequestAvailable
	(RTI_LogicalTime const & theTime)
	throw (RTI_InvalidLogicalTime,
			RTI_LogicalTimeAlreadyPassed,
			RTI_InTimeAdvancingState,
			RTI_RequestForTimeRegulationPending,
			RTI_RequestForTimeConstrainedPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.10
void nextMessageRequest
	(RTI_LogicalTime const & theTime)
	throw (RTI_InvalidLogicalTime,
			RTI_LogicalTimeAlreadyPassed,
			RTI_InTimeAdvancingState,
			RTI_RequestForTimeRegulationPending,
			RTI_RequestForTimeConstrainedPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.11
void nextMessageRequestAvailable
	(RTI_LogicalTime const & theTime)
	throw (RTI_InvalidLogicalTime,
			RTI_LogicalTimeAlreadyPassed,
			RTI_InTimeAdvancingState,
			RTI_RequestForTimeRegulationPending,
			RTI_RequestForTimeConstrainedPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.12
void flushQueueRequest
	(RTI_LogicalTime const & theTime)
	throw (RTI_InvalidLogicalTime,
			RTI_LogicalTimeAlreadyPassed,
			RTI_InTimeAdvancingState,
			RTI_RequestForTimeRegulationPending,
			RTI_RequestForTimeConstrainedPending,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.14
void enableAsynchronousDelivery ()
	throw (RTI_AsynchronousDeliveryAlreadyEnabled,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.15
void disableAsynchronousDelivery ()
	throw (RTI_AsynchronousDeliveryAlreadyDisabled,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.16
RTI_auto_ptr< RTI_LogicalTime > queryGALT ()
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.17
RTI_LogicalTime const & queryLogicalTime ()
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.18
RTI_auto_ptr< RTI_LogicalTime > const & queryLITS ()
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.19
void modifyLookahead
	(RTI_LogicalTimeInterval const & theLookahead)
	throw (RTI_TimeRegulationIsNotEnabled,
			RTI_InvalidLookahead,
			RTI_InTimeAdvancingState,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.20
RTI_LogicalTimeInterval const & queryLookahead ()
	throw (RTI_TimeRegulationIsNotEnabled,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.21
void retract
	(RTI_MessageRetractionHandle const & theHandle)
	throw (RTI_InvalidRetractionHandle,
			RTI_TimeRegulationIsNotEnabled,
			RTI_MessageCanNoLongerBeRetracted,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.23
void changeAttributeOrderType
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSet const & theAttributes,
	 RTI_OrderType const & theType)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_AttributeNotOwned,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 8.24
void changeInteractionOrderType
	(RTI_InteractionClassHandle const & theClass,
	 RTI_OrderType const & theType)
	throw (RTI_InteractionClassNotDefined,
			RTI_InteractionClassNotPublished,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

//////////////////////////////////
// Data Distribution Management //
//////////////////////////////////

// 9.2
RTI_auto_ptr< RTI_RegionHandle > createRegion
	(RTI_DimensionHandleSet const & theDimensions)
	throw (RTI_InvalidDimensionHandle,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.3
void commitRegionModifications
	(RTI_RegionHandleSet const & theRegionHandleSet)
	throw (RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.4
void deleteRegion
	(RTI_auto_ptr< RTI_RegionHandle > theRegion)
	throw (RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_RegionInUseForUpdateOrSubscription,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.5
RTI_ObjectInstanceHandle const & registerObjectInstanceWithRegions
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSetRegionHandleSetPairVector const &
	 theAttributeHandleSetRegionHandleSetPairVector)
	throw (RTI_ObjectClassNotDefined,
			RTI_ObjectClassNotPublished,
			RTI_AttributeNotDefined,
			RTI_AttributeNotPublished,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

RTI_ObjectInstanceHandle const & registerObjectInstanceWithRegions
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSetRegionHandleSetPairVector const &
	 theAttributeHandleSetRegionHandleSetPairVector,
	 RTI_wstring const & theObjectInstanceName)
	throw (RTI_ObjectClassNotDefined,
			RTI_ObjectClassNotPublished,
			RTI_AttributeNotDefined,
			RTI_AttributeNotPublished,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_ObjectInstanceNameNotReserved,
			RTI_ObjectInstanceNameInUse,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.6
void associateRegionsForUpdates
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSetRegionHandleSetPairVector const &
	 theAttributeHandleSetRegionHandleSetPairVector)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.7
void unassociateRegionsForUpdates
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_AttributeHandleSetRegionHandleSetPairVector const &
	 theAttributeHandleSetRegionHandleSetPairVector)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.8
void subscribeObjectClassAttributesWithRegions
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSetRegionHandleSetPairVector const &
	 theAttributeHandleSetRegionHandleSetPairVector,
	 RTI_bool active = RTI_true)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.9
void unsubscribeObjectClassAttributesWithRegions
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSetRegionHandleSetPairVector const
	 & theAttributeHandleSetRegionHandleSetPairVector)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.10
void subscribeInteractionClassWithRegions
	(RTI_InteractionClassHandle const & theClass,
	 RTI_RegionHandleSet const & theRegionHandleSet,
	 RTI_bool active = RTI_true)
	throw (RTI_InteractionClassNotDefined,RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_FederateServiceInvocationsAreBeingReportedViaMOM,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.11
void unsubscribeInteractionClassWithRegions
	(RTI_InteractionClassHandle const & theClass,
	 RTI_RegionHandleSet const & theRegionHandleSet)
	throw (RTI_InteractionClassNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.12
void sendInteractionWithRegions
	(RTI_InteractionClassHandle const & theInteraction,
	 RTI_ParameterHandleValueMap const & theParameterValues,
	 RTI_RegionHandleSet const & theRegionHandleSet,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_InteractionClassNotDefined,
			RTI_InteractionClassNotPublished,
			RTI_InteractionParameterNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

RTI_auto_ptr< RTI_MessageRetractionHandle > sendInteractionWithRegions
	(RTI_InteractionClassHandle const & theInteraction,
	 RTI_ParameterHandleValueMap const & theParameterValues,
	 RTI_RegionHandleSet const & theRegionHandleSet,
	 RTI_UserSuppliedTag const & theUserSuppliedTag,
	 RTI_LogicalTime const & theTime)
	throw (RTI_InteractionClassNotDefined,
			RTI_InteractionClassNotPublished,
			RTI_InteractionParameterNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_InvalidLogicalTime,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 9.13
void requestAttributeValueUpdateWithRegions
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandleSetRegionHandleSetPairVector const & theSet,
	 RTI_UserSuppliedTag const & theUserSuppliedTag)
	throw (RTI_ObjectClassNotDefined,
			RTI_AttributeNotDefined,
			RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_InvalidRegionContext,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

//////////////////////////
// RTI Support Services //
//////////////////////////

#endif
// 10.2
RTI_ObjectClassHandle const & RTIambImplementor::getObjectClassHandle
	(RTI_wstring const & theName)
	throw (RTI_NameNotFound,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
    char name[200] = {'\0'};
	RTI_ObjectClassHandle *handle;

	THROW_NOT_JOINED();

    WStrtoStr(name, theName, 199);
	
	handle = oht->getRTIObjectClassHandle(name);
    if(handle == NULL)
	{
		printf("%s:%d ->RTI_NameNotFound\n", __FILE__, __LINE__);
		throw RTI_NameNotFound("RTI_NameNotFound");
	}
	
	return *handle;
}

// 10.3
RTI_wstring const & RTIambImplementor::getObjectClassName
	(RTI_ObjectClassHandle const & theHandle)
	throw (RTI_InvalidObjectClassHandle,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{	
	int handle;
	RTI_wstring *ws;
	
	THROW_NOT_JOINED();
    
    handle = (int)theHandle.getHandle();
    if(handle <=0 || handle > (int)oht->getClassNodeCount())
	{	
		printf("%s:%d ->RTI_InvalidObjectClassHandle\n", __FILE__, __LINE__);
		throw RTI_InvalidObjectClassHandle("RTI_InvalidObjectClassHandle");
	}
	ws = oht->getRTIObjectClassName(handle);
	if (ws == NULL)
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_RTIinternalError("InternalError: no such object class handle.");
	}
	return *ws;
}

// 10.4
RTI_AttributeHandle const & RTIambImplementor::getAttributeHandle
	(RTI_ObjectClassHandle const & whichClass,
	 RTI_wstring const & theAttributeName)
	throw (RTI_InvalidObjectClassHandle,
			RTI_NameNotFound,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	char name[200] = {'\0'};
	int classhandle;
	RTI_AttributeHandle *attrhandle;
	
	THROW_NOT_JOINED();

	classhandle = (int)whichClass.getHandle();
    WStrtoStr(name, theAttributeName ,199);
	
	attrhandle = oht->getRTIAttributeHandle(classhandle, name);
    if(attrhandle == NULL)
	{
		printf("%s:%d ->RTI_NameNotFound\n", __FILE__, __LINE__);
		throw RTI_NameNotFound("RTI_NameNotFound");
	}
	return *attrhandle;
}

// 10.5
RTI_wstring const & RTIambImplementor::getAttributeName
	(RTI_ObjectClassHandle const & whichClass,
	 RTI_AttributeHandle const & theHandle)
	throw (RTI_InvalidObjectClassHandle,
			RTI_InvalidAttributeHandle,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	int classhandle, attrhandle;
	RTI_wstring *ws;
	
	THROW_NOT_JOINED();

    if(oht == NULL)
	{
		printf("%s:%d ->RTI_AttributeNotDefined\n", __FILE__, __LINE__);
		throw RTI_AttributeNotDefined("RTI_AttributeNotDefined");
	}
	
	classhandle = (int)whichClass.getHandle();
	attrhandle = (int)theHandle.getHandle();
	
    if(classhandle <= 0 || classhandle > (int)oht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InvalidObjectClassHandle\n", __FILE__, __LINE__);
		throw RTI_InvalidObjectClassHandle("RTI_InvalidObjectClassHandle");
	}
    if(attrhandle <=0 || attrhandle > (int)oht->getNumberOfAttribute(classhandle))
	{
		printf("%s:%d ->RTI_InvalidAttributeHandle\n", __FILE__, __LINE__);
		throw RTI_InvalidAttributeHandle("RTI_InvalidAttributeHandle");
	}
	
	ws = oht->getRTIAttributeName(classhandle, attrhandle);
	if (ws == NULL)
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_RTIinternalError("InternalError: no such attribute handle.");
	}
	return *ws;
}

// 10.6
RTI_InteractionClassHandle const & RTIambImplementor::getInteractionClassHandle
	(RTI_wstring const & theName)
	throw (RTI_NameNotFound,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	char name[200] = {'\0'};
	RTI_InteractionClassHandle *handle;

	THROW_NOT_JOINED();

    WStrtoStr(name, theName.c_str(), 199);

	handle = iht->getRTIInteractionClassHandle(name);
    if(handle == NULL)
	{
		printf("%s:%d ->RTI_NameNotFound\n", __FILE__, __LINE__);
		throw RTI_NameNotFound("RTI_NameNotFound");
	}

	return *handle;
}

// 10.7
RTI_wstring const & RTIambImplementor::getInteractionClassName
	(RTI_InteractionClassHandle const & theHandle)
	throw (RTI_InvalidInteractionClassHandle,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	int handle;
	RTI_wstring *ws;
	
	THROW_NOT_JOINED();

	handle = (int)theHandle.getHandle();
    if(handle <= 0 || handle > (int)iht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InvalidInteractionClassHandle\n", __FILE__, __LINE__);
		throw RTI_InvalidInteractionClassHandle("getInteractionClassName: RTI_InvalidInteractionClassHandle");
	}

	ws = iht->getRTIInteractionClassName(handle);
	if (ws == NULL)
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_RTIinternalError("InternalError: no such interaction class handle.");
	}
	return *ws;
}

// 10.8
RTI_ParameterHandle const & RTIambImplementor::getParameterHandle
	(RTI_InteractionClassHandle const & whichClass,
	 RTI_wstring const & theName)
	throw (RTI_InvalidInteractionClassHandle,
			RTI_NameNotFound,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
    char name[200] = {'\0'};
	int classhandle;
	RTI_ParameterHandle *paramhandle;
	
	THROW_NOT_JOINED();
	
	classhandle = (int)whichClass.getHandle();
    if(classhandle <=0 || classhandle > (int)iht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InvalidInteractionClassHandle\n", __FILE__, __LINE__);
		throw RTI_InvalidInteractionClassHandle("getParameterHandle: RTI_InvalidInteractionClassHandle");
	}

    WStrtoStr(name, theName.c_str(), 199);
	paramhandle = iht->getRTIParameterHandle(classhandle, name);

    if(paramhandle == NULL)
	{
		printf("%s:%d ->RTI_NameNotFound\n", __FILE__, __LINE__);
		throw RTI_NameNotFound("RTI_NameNotFound");
	}
	
	return *paramhandle;
}

// 10.9
RTI_wstring const & RTIambImplementor::getParameterName
	(RTI_InteractionClassHandle const & whichClass,
	 RTI_ParameterHandle const & theHandle)
	throw (RTI_InvalidInteractionClassHandle,
			RTI_InvalidParameterHandle,
			RTI_InteractionParameterNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	int classhandle, paramhandle;
	RTI_wstring *ws;

	THROW_NOT_JOINED();

    if(iht == NULL)
	{
		printf("%s:%d ->RTI_InteractionParameterNotDefined\n", __FILE__, __LINE__);
		throw RTI_InteractionParameterNotDefined("RTI_InteractionParameterNotDefined");
	}
	classhandle = (int)whichClass.getHandle();
	paramhandle = (int)theHandle.getHandle();

    if(classhandle <=0 || classhandle > (int)iht->getClassNodeCount())
	{
		printf("%s:%d ->RTI_InvalidInteractionClassHandle\n", __FILE__, __LINE__);
		throw RTI_InvalidInteractionClassHandle("getParameterName: RTI_InvalidInteractionClassHandle");
	}
    if(paramhandle <=0 || paramhandle > (int)iht->getNumberOfParameter(classhandle))
	{
		printf("%s:%d ->RTI_InvalidParameterHandle\n", __FILE__, __LINE__);
		throw RTI_InvalidParameterHandle("RTI_InvalidParameterHandle");
	}

	ws = iht->getRTIParameterName(classhandle, paramhandle);
	if (ws == NULL)
	{
		printf("%s:%d ->RTI_RTIinternalError\n", __FILE__, __LINE__);
		throw RTI_RTIinternalError("InternalError: no such parameter handle.");
	}	
	return *ws;
}

// 10.10
RTI_ObjectInstanceHandle const & RTIambImplementor::getObjectInstanceHandle
	(RTI_wstring const & theName)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	THROW_NOT_JOINED();

	InstMap::iterator iter;
	
	for(iter=AllInst.begin(); iter!= AllInst.end(); iter++)
	{
		if(iter->second.InstName == theName)
			return iter->first;
	}
	
	printf("%s:%d ->RTI_ObjectInstanceNotKnown\n", __FILE__, __LINE__);
	throw RTI_ObjectInstanceNotKnown("RTI_ObjectInstanceNotKnown");
}

// 10.11
RTI_wstring const & RTIambImplementor::getObjectInstanceName
	(RTI_ObjectInstanceHandle const & theHandle)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError)
{
	int AllInst_count;
	
	THROW_NOT_JOINED();

	try{
		AllInst_count = AllInst.count(theHandle);
	}
	catch(exception &e){
		catchSTL();
	}
	
	if(AllInst_count <= 0)
	{
		printf("%s:%d ->RTI_ObjectInstanceNotKnown\n", __FILE__, __LINE__);
		throw RTI_ObjectInstanceNotKnown("RTI_ObjectInstanceNotKnown");
	}
	else
	{
		return AllInst[theHandle].InstName;
	}
}

#if 0
// 10.12
RTI_DimensionHandle const & getDimensionHandle
	(RTI_wstring const & theName)
	throw (RTI_NameNotFound,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.13
RTI_wstring const & getDimensionName
	(RTI_DimensionHandle const & theHandle)
	throw (RTI_InvalidDimensionHandle,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.14
unsigned long getDimensionUpperBound
	(RTI_DimensionHandle const & theHandle)
	throw (RTI_InvalidDimensionHandle,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.15
RTI_DimensionHandleSet const & getAvailableDimensionsForClassAttribute
	(RTI_ObjectClassHandle const & theClass,
	 RTI_AttributeHandle const & theHandle)
	throw (RTI_InvalidObjectClassHandle,
			RTI_InvalidAttributeHandle,
			RTI_AttributeNotDefined,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.16
RTI_ObjectClassHandle const & getKnownObjectClassHandle
	(RTI_ObjectInstanceHandle const & theObject)
	throw (RTI_ObjectInstanceNotKnown,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.17
RTI_DimensionHandleSet const & getAvailableDimensionsForInteractionClass
	(RTI_InteractionClassHandle const & theClass)
	throw (RTI_InvalidInteractionClassHandle,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.18
RTI_TransportationType const & getTransportationType
	(RTI_wstring const & transportationName)
	throw (RTI_InvalidTransportationName,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.19
RTI_wstring const & getTransportationName
	(RTI_TransportationType const & transportationType)
	throw (RTI_InvalidTransportationType,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.20
RTI_OrderType const & getOrderType
	(RTI_wstring const & orderName)
	throw (RTI_InvalidOrderName,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.21
RTI_wstring const & getOrderName
	(RTI_OrderType const & orderType)
	throw (RTI_InvalidOrderType,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.22
void enableObjectClassRelevanceAdvisorySwitch ()
	throw (RTI_ObjectClassRelevanceAdvisorySwitchIsOn,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.23
void disableObjectClassRelevanceAdvisorySwitch ()
	throw (RTI_ObjectClassRelevanceAdvisorySwitchIsOff,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.24
void enableAttributeRelevanceAdvisorySwitch ()
	throw (RTI_AttributeRelevanceAdvisorySwitchIsOn,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.25
void disableAttributeRelevanceAdvisorySwitch ()
	throw (RTI_AttributeRelevanceAdvisorySwitchIsOff,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.26
void enableAttributeScopeAdvisorySwitch ()
	throw (RTI_AttributeScopeAdvisorySwitchIsOn,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.27
void disableAttributeScopeAdvisorySwitch ()
	throw (RTI_AttributeScopeAdvisorySwitchIsOff,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.28
void enableInteractionRelevanceAdvisorySwitch ()
	throw (RTI_InteractionRelevanceAdvisorySwitchIsOn,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.29
void disableInteractionRelevanceAdvisorySwitch ()
	throw (RTI_InteractionRelevanceAdvisorySwitchIsOff,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.30
RTI_DimensionHandleSet const & getDimensionHandleSet
	(RTI_RegionHandle const & theRegionHandle)
	throw (RTI_InvalidRegion,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.31
RTI_RangeBounds const & getRangeBounds
	(RTI_RegionHandle const & theRegionHandle,
	 RTI_DimensionHandle const & theDimensionHandle)
	throw (RTI_InvalidRegion,
			RTI_RegionDoesNotContainSpecifiedDimension,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.32
void setRangeBounds
	(RTI_RegionHandle const & theRegionHandle,
	 RTI_DimensionHandle const & theDimensionHandle,
	 RTI_RangeBounds const & theRangeBounds)
	throw (RTI_InvalidRegion,
			RTI_RegionNotCreatedByThisFederate,
			RTI_RegionDoesNotContainSpecifiedDimension,
			RTI_InvalidRangeBounds,
			RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.33
unsigned long normalizeFederateHandle
	(RTI_FederateHandle const & theFederateHandle)
	throw (RTI_FederateHandle,
			RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.34
unsigned long normalizeServiceGroup
	(RTI_ServiceGroupIndicator const & theServiceGroup)
	throw (RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.37
RTI_bool evokeCallback(double approximateMinimumTimeInSeconds)
	throw (RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.38
RTI_bool evokeMultipleCallbacks(double approximateMinimumTimeInSeconds,
		double approximateMaximumTimeInSeconds)
	throw (RTI_FederateNotExecutionMember,
			RTI_RTIinternalError);

// 10.39
void enableCallbacks ()
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

// 10.40
oid disableCallbacks ()
	throw (RTI_FederateNotExecutionMember,
			RTI_SaveInProgress,
			RTI_RestoreInProgress,
			RTI_RTIinternalError);

#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
