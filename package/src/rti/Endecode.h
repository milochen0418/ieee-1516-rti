#ifndef ENDECODE_H
#define ENDECODE_H

#include <stdint.h>

#include "RTI_Typedefs.h"
#include "ms-types.h"

/* Not all enumerations will be used, but we still list it here */
enum 
{
	/* Federation Management Services */
	CreateFederationExecution = 1,
	DestroyFederationExecution,
	JoinFederationExecution,
	ResignFederationExecution,
	RegisterFederationSynchronizationPoint,
	SynchronizationPointAchieved,
	RequestFederationSave,
	FederateSaveBegun,
	FederateSaveComplete,
	QueryFederationSaveStatus,
	RequestFederationRestore,
	FederateRestoreComplete,
	QueryFederationRestoreStatus,
	
	/* Declaration Management Services */
	PublishObjectClassAttributes,
	UnpublishObjectClass,
	UnpublishObjectClassAttributes,
	PublishInteractionClass,
	UnpublishInteractionClass,
	SubscribeObjectClassAttributes,
	UnsubscribeObjectClass,
	UnsubscribeObjectClassAttributes,
	SubscribeInteractionClass,
	UnsubscribeInteractionClass,
	
	/* Object Management Services */
	ReserveObjectInstanceName,
	RegisterObjectInstance,
	UpdateAttributeValues,
	SendInteraction,
	DeleteObjectInstance,
	LocalDeleteObjectInstance,
	ChangeAttributeTransportationType,
	ChangeInteractionTransportationType,
	RequestAttributeValueUpdate,

	
	/* Ownership Management Services */
	UnconditionalAttributeOwnershipDivestiture,
	NegotiatedAttributeOwnershipDivestiture,
	ConfirmDivestiture,
	AttributeOwnershipAcquisition,
	AttributeOwnershipAcquisitionIfAvailable,
	AttributeOwnershipDivestitureIfWanted,
	CancelNegotiatedAttributeOwnershipDivestiture,
	CancelAttributeOwnershipAcquisition,
	QueryAttributeOwnership,
	IsAttributeOwnedByFederate,

	/* Time Management Service */
	EnableTimeRegulation,
	DisableTimeRegulation,
	EnableTimeConstrained,
	DisableTimeConstrained,
	TimeAdvanceRequest,
	TimeAdvanceRequestAvailable,
	NextMessageRequest,
	NextMessageRequestAvailable,
	FlushQueueRequest,
	EnableAsynchronousDelivery,
	DisableAsynchronousDelivery,
	QueryGALT,
	QueryLogicalTime,
	QueryLITS,
	ModifyLookahead,
	QueryLookahead,
	Retract,
	ChangeAttributeOrderType,
	ChangeInteractionOrderType,

	/* Data Distribution Management */
	CreateRegion,
	CommitRegionModifications,
	DeleteRegion,
	RegisterObjectInstanceWithRegions,
	AssociateRegionsForUpdates,
	UnassociateRegionsForUpdates,
	SubscribeObjectClassAttributesWithRegions,
	UnsubscribeObjectClassAttributesWithRegions,
	SubscribeInteractionClassWithRegions,
	UnsubscribeInteractionClassWithRegions,
	SendInteractionWithRegions,
	RequestAttributeValueUpdateWithRegions,
	
	/*Fedex to Fedex*/
	TransmitReflectAttributeValues ,
	TransmitDiscoverObjectInstance ,
	TransmitReceiveInteraction,
	TransmitSynchronizationPointAchieved,
	TransmitFederationSynchronized
};


enum
{
	/* Federation Management */
	SynchronizationPointRegistrationSucceeded=0,
	SynchronizationPointRegistrationFailed,
	AnnounceSynchronizationPoint,
	FederationSynchronized,
	InitiateFederateSave,
	FederationSaved,
	FederationNotSaved,
	FederationSaveStatusResponse,
	RequestFederationRestoreSucceeded,
	RequestFederationRestoreFailed,
	FederationRestoreBegun,
	InitiateFederateRestore,
	FederationRestored,
	FederationNotRestored,

	/* Declaration Management */
	StartRegistrationForObjectClass,
	StopRegistrationForObjectClass,
	TurnInteractionsOn,
	TurnInteractionsOff,

	/* Object Management  */
	ObjectInstanceNameReservationSucceeded,
	ObjectInstanceNameReservationFailed,
	DiscoverObjectInstance,
	ReflectAttributeValues,
	ReceiveInteraction,
	RemoveObjectInstance,
	AttributesInScope,
	AttributesOutOfScope,
	ProvideAttributeValueUpdate,
	TurnUpdatesOnForObjectInstance,
	TurnUpdatesOffForObjectInstance,

	/* Ownership Management */
	RequestAttributeOwnershipAssumption,
	RequestDivestitureConfirmation,
	AttributeOwnershipAcquisitionNotification,
	AttributeOwnershipUnavailable,
	RequestAttributeOwnershipRelease,
	ConfirmAttributeOwnershipAcquisitionCancellation,
	InformAttributeOwnership,
	AttributeIsNotOwned,
	AttributeIsOwnedByRTI,

	/* Time Management */
	TimeRegulationEnabled,
	TimeConstrainedEnabled,
	TimeAdvanceGrant,
	RequestRetraction
};



struct header_t
{
	federateID src;
	int32_t type;
	int32_t count;
	int32_t user_tag;	/* used by developer */
};

struct buffer_t
{
	void *start;
	uint32_t len;
};

struct buffer_list_t
{
	int num;
	void **start_list;
	uint32_t *len_list;
};

void DecodeBuffer(char *buffer, struct header_t *head, struct buffer_t *temp);

class EndecodeFactory
{
	private:
		int32_t type;
		int32_t size;		/* size of the buffer/size list */
		int32_t current;

		/* header portion */
		char *header;
		uint32_t header_size;

		/* payload portion */
		void **buffer_list;
		uint32_t *len_list;

		int debug;
		
	public:

		EndecodeFactory(int type) : size(0), current(0), header(NULL), header_size(0),  
									buffer_list(NULL), len_list(NULL), debug(0)
		{
			this->type = type;
		};
		
		~EndecodeFactory();

		int create(int size);
		int add(void * buffer, uint32_t len);
		int add(struct buffer_t *buf);
		int add(struct buffer_list_t *buf_list);

		int32_t get_type();
		int32_t get_size();
		int32_t get_count();
		int32_t get_usertag();	/* on error, return -1 */
		uint32_t get_header_size();

		void **get_buffer_list();
		uint32_t *get_len_list();
		
		int get_debug();

		void set_src(federateID *src);	/* set source federate ID */
		void set_debug(int mode);
		int set_usertag(int32_t user_tag);	/* on error, return -1 */
		int set_type(int type);
};

/* endecode string/wstring */
void encode_string(string const &str, struct buffer_t *buf);
void decode_string(string &str, struct buffer_t *buf);
void encode_wstring(wstring const &wstr, struct buffer_t *buf);
void decode_wstring(wstring &wstr, struct buffer_t *buf);

/* 
 * endecode RTI_Value:
 *		RTI_AttributeValue, RTI_ParameterValue, RTI_UserSuppliedTag, RTI_EncodedData
 */
#define endecode_RTI_Value(type) \
	void encode_##type(type const &__value, struct buffer_t *__buf); \
	void decode_##type(type &__value, struct buffer_t *__buf)

endecode_RTI_Value(RTI_AttributeValue);
endecode_RTI_Value(RTI_ParameterValue);
endecode_RTI_Value(RTI_UserSuppliedTag);
endecode_RTI_Value(RTI_EncodedData);

#undef endecode_RTI_Value

/* 
 * endecode RTI_Handle 
 *		RTI_FederateHandle, RTI_ObjectClassHandle, RTI_InteractionClassHandle, RTI_ObjectInstanceHandle
 *		RTI_AttributeHandle, RTI_ParameterHandle, RTI_DimensionHandle, RTI_MessageRetractionHandle,
 *		RTI_RegionHandle
 */
#define endecode_RTI_Handle(type) \
	void encode_##type(type const &__handle, struct buffer_t *__buf); \
	void decode_##type(type &__handle, struct buffer_t *__buf)

endecode_RTI_Handle(RTI_FederateHandle);
endecode_RTI_Handle(RTI_ObjectClassHandle);
endecode_RTI_Handle(RTI_InteractionClassHandle);
endecode_RTI_Handle(RTI_ObjectInstanceHandle);
endecode_RTI_Handle(RTI_AttributeHandle);
endecode_RTI_Handle(RTI_ParameterHandle);
endecode_RTI_Handle(RTI_DimensionHandle);
endecode_RTI_Handle(RTI_MessageRetractionHandle);
endecode_RTI_Handle(RTI_RegionHandle);

#undef endecode_RTI_Handle

/* 
 * endecode RTI_set 
 *		RTI_AttributeHandleSet, RTI_FederateHandleSet, RTI_DimensionHandleSet, RTI_RegionHandleSet
 */
#define endecode_RTI_set(type) \
	void encode_##type(type const &__set, struct buffer_list_t *__buf); \
	void decode_##type(type &__set, struct buffer_list_t *__buf)

endecode_RTI_set(RTI_AttributeHandleSet);
endecode_RTI_set(RTI_FederateHandleSet);
endecode_RTI_set(RTI_DimensionHandleSet);
endecode_RTI_set(RTI_RegionHandleSet);

#undef endecode_RTI_set

/* 
 * RTI_map 
 *		RTI_AttributeHandleValueMap, RTI_ParameterHandleValueMap
 */
#define endecode_RTI_map(type) \
	void encode_##type(type const &__map, struct buffer_list_t *__buf); \
	void decode_##type(type &__map, struct buffer_list_t *__buf)

endecode_RTI_map(RTI_AttributeHandleValueMap);
endecode_RTI_map(RTI_ParameterHandleValueMap);

#undef endecode_RTI_map

#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
