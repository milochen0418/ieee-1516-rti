#include <iostream>
#include <string>
#include "Endecode.h"

using namespace std;

char func_name[][50] = 
{
	"",
	/* Federation Management Services */
	"CreateFederationExecution",
	"DestroyFederationExecution",
	"JoinFederationExecution",
	"ResignFederationExecution",
	"RegisterFederationSynchronizationPoint",
	"SynchronizationPointAchieved",
	"RequestFederationSave",
	"FederateSaveBegun",
	"FederateSaveComplete",
	"QueryFederationSaveStatus",
	"RequestFederationRestore",
	"FederateRestoreComplete",
	"QueryFederationRestoreStatus",
	
	/* Declaration Management Services */
	"PublishObjectClassAttributes",
	"UnpublishObjectClass",
	"UnpublishObjectClassAttributes",
	"PublishInteractionClass",
	"UnpublishInteractionClass",
	"SubscribeObjectClassAttributes",
	"UnsubscribeObjectClass",
	"UnsubscribeObjectClassAttributes",
	"SubscribeInteractionClass",
	"UnsubscribeInteractionClass",
	
	/* Object Management Services */
	"ReserveObjectInstanceName",
	"RegisterObjectInstance",
	"UpdateAttributeValues",
	"SendInteraction",
	"DeleteObjectInstance",
	"LocalDeleteObjectInstance",
	"ChangeAttributeTransportationType",
	"ChangeInteractionTransportationType",
	"RequestAttributeValueUpdate",

	/* Ownership Management Services */
	"UnconditionalAttributeOwnershipDivestiture",
	"NegotiatedAttributeOwnershipDivestiture",
	"ConfirmDivestiture",
	"AttributeOwnershipAcquisition",
	"AttributeOwnershipAcquisitionIfAvailable",
	"AttributeOwnershipDivestitureIfWanted",
	"CancelNegotiatedAttributeOwnershipDivestiture",
	"CancelAttributeOwnershipAcquisition",
	"QueryAttributeOwnership",
	"IsAttributeOwnedByFederate",

	/* Time Management Service */
	"EnableTimeRegulation",
	"DisableTimeRegulation",
	"EnableTimeConstrained",
	"DisableTimeConstrained",
	"TimeAdvanceRequest",
	"TimeAdvanceRequestAvailable",
	"NextMessageRequest",
	"NextMessageRequestAvailable",
	"FlushQueueRequest",
	"EnableAsynchronousDelivery",
	"DisableAsynchronousDelivery",
	"QueryGALT",
	"QueryLogicalTime",
	"QueryLITS",
	"ModifyLookahead",
	"QueryLookahead",
	"Retract",
	"ChangeAttributeOrderType",
	"ChangeInteractionOrderType",

	/* Data Distribution Management */
	"CreateRegion",
	"CommitRegionModifications",
	"DeleteRegion",
	"RegisterObjectInstanceWithRegions",
	"AssociateRegionsForUpdates",
	"UnassociateRegionsForUpdates",
	"SubscribeObjectClassAttributesWithRegions",
	"UnsubscribeObjectClassAttributesWithRegions",
	"SubscribeInteractionClassWithRegions",
	"UnsubscribeInteractionClassWithRegions",
	"SendInteractionWithRegions",
	"RequestAttributeValueUpdateWithRegions",

	/*Fedex to Fedex*/
	"TransmitReflectAttributeValues",
	"TransmitDiscoverObjectInstance",
	"TransmitReceiveInteraction",
	
};

void DecodeBuffer(char *buffer, struct header_t *head, struct buffer_t *temp)
{
	int head_len=sizeof(struct header_t);
	int size_len=sizeof(uint32_t);
	int i, base;
	uint32_t *p;
	
	base = head_len + size_len*head->count;
	for(i=0;i< head->count ;i++)
	{
		temp[i].start = &buffer[base];
		p = (uint32_t*)&buffer[head_len+i*size_len];
		temp[i].len = *p;
		base = base + (*p);
	}
}

EndecodeFactory::~EndecodeFactory()
{
	if (header)
		delete header;
	if (buffer_list)
		delete [] buffer_list;
	if (len_list)
		delete [] len_list;
};

int EndecodeFactory::create(int size)
{
	struct header_t *header = NULL;
	
	/* allocate a region to record header and size_list */
	header_size = sizeof(struct header_t) + size * sizeof(uint32_t);
	this->header = new char[header_size];

	/* the reason to allocate size plus one is that the first element is used for the header */
	buffer_list = new void*[size+1];
	len_list = new uint32_t[size+1];
	
	if (this->header == NULL || buffer_list == NULL || len_list == NULL)
	{
		cout << "Not enough memory space" << endl;
		return 0;
	}
	
	header = (struct header_t *)this->header;
	memset(header, 0, header_size);
	header->type = type;
	header->count = size;

	buffer_list[0] = header;
	len_list[0] = header_size;
	
	this->size = size+1;
	current = 1;

	if (debug)
		cout << "Type " << type << " allocates " << this->size << " element(s)." << endl;

	return 1;
}

int EndecodeFactory::add(void *buffer, uint32_t len)
{
	uint32_t *len_p = NULL;
	
	if (buffer == NULL)
	{
		cout << "Null buffer pointer" << endl;
		return 0;
	}
	
	if (len == 0)
	{
		cout << "len must not be zero" << endl;
		return 0;
	}

	if (current == size)
	{
		cout << "Array out of bound" << endl;
		return 0;
	}

	buffer_list[current] = buffer;
	len_list[current] = len;

	len_p = (uint32_t *)((char *)header + sizeof(struct header_t));
	len_p[current-1] = len;

	current++;

	if (debug)
		cout << "Add one element with buffer " << buffer << " len " << len << endl;

	return 1;
}

int EndecodeFactory::add(struct buffer_t *buf)
{
	uint32_t *len_p = NULL;
	
	if (buf == NULL)
	{
		cout << "Null buf pointer" << endl;
		return 0;
	}

	if (buf->start == NULL)
	{
		cout << "Null buffer start pointer" << endl;
		return 0;
	}

	if (buf->len == 0)
	{
		cout << "len must not be zero" << endl;
		return 0;
	}

	if (current == size)
	{
		cout << "Array out of bound" << endl;
		return 0;
	}

	buffer_list[current] = buf->start;
	len_list[current] = buf->len;

	len_p = (uint32_t *)((char *)header + sizeof(struct header_t));
	len_p[current-1] = buf->len;

	current++;

	if (debug)
		cout << "Add one element with buffer " << buf->start << " len " << buf->len << endl;

	return 1;
}

int EndecodeFactory::add(struct buffer_list_t *buf_list)
{
	uint32_t *len_p = NULL;
	
	if (buf_list == NULL)
	{
		cout << "Null buf pointer" << endl;
		return 0;
	}

	if (buf_list->start_list == NULL || buf_list->len_list == NULL)
	{
		cout << "Null start_list or len_list pointer" << endl;
		return 0;
	}

	if (buf_list->num == 0)
	{
		cout << "No any encoded elemnet to process" << endl;
		return 0;
	}

	if (current + buf_list->num > size)
	{
		cout << "Array out of bound" << endl;
		return 0;
	}

	memcpy(&buffer_list[current], buf_list->start_list, buf_list->num * sizeof(void *));
	memcpy(&len_list[current], buf_list->len_list, buf_list->num * sizeof(uint32_t));

	len_p = (uint32_t *)((char *)header + sizeof(struct header_t));
	memcpy(&len_p[current-1], buf_list->len_list, buf_list->num * sizeof(uint32_t));

	current += buf_list->num;

	if (debug)
		cout << "Add " << buf_list->num << " elements" << endl;

	delete [] buf_list->start_list;
	delete [] buf_list->len_list;

	return 1;
}

int32_t EndecodeFactory::get_type()
{
	return type;
}

int32_t EndecodeFactory::get_size()
{
	return size;
}

int32_t EndecodeFactory::get_count()
{
	return current;
}

uint32_t EndecodeFactory::get_header_size()
{
	return header_size;
}

int EndecodeFactory::get_usertag()
{
	struct header_t *header = NULL;

	if (size == 0)
	{
		cout << "Encode structure not created" << endl;
		return -1;
	}
	
	header = (struct header_t *)this->header;
	return header->user_tag;
}

void **EndecodeFactory::get_buffer_list()
{
	return buffer_list;
}

uint32_t *EndecodeFactory::get_len_list()
{
	return len_list;
}

int EndecodeFactory::get_debug()
{
	return debug;
}

void EndecodeFactory::set_src(federateID *src)
{
	struct header_t *header = (struct header_t *)this->header;
	
	header->src = *src;
}

void EndecodeFactory::set_debug(int mode)
{
	debug = mode;
}

int EndecodeFactory::set_usertag(int32_t user_tag)
{
	struct header_t *header = NULL;

	if (size == 0)
	{
		cout << "Encode structure not created" << endl;
		return -1;
	}

	header = (struct header_t *)this->header;
	header->user_tag = user_tag;

	return 1;
}

int EndecodeFactory::set_type(int t)
{
	struct header_t *header = NULL;

	if (size == 0)
	{
		cout << "Encode structure not created" << endl;
		return -1;
	}

	header = (struct header_t *)this->header;
	header->type = t;

	return 1;
}


/*
 * Specific encode/decode rules
 */

/* string */
void encode_string(string const &str, struct buffer_t *buf)
{
	buf->start = (void *)str.c_str();
	buf->len = (str.size()+1) * sizeof(char);	/* plus one, because we want the terminal symbol */
}

void decode_string(string &str, struct buffer_t *buf)
{
	str = string((char *)buf->start);
}

/* wstring */
void encode_wstring(wstring const &wstr, struct buffer_t *buf)
{
	buf->start = (void *)wstr.c_str();
	buf->len = (wstr.size()+1) * sizeof(wchar_t);	/* plus one, because we want the terminal symbol */
}

void decode_wstring(wstring &wstr, struct buffer_t *buf)
{
	wstr = wstring((wchar_t *)buf->start);
}

/* 
 * RTI_Value: 
 *		RTI_AttributeValue, RTI_ParameterValue, RTI_UserSuppliedTag, RTI_EncodedData 
 */
#define endecode_RTI_Value(type) \
	void encode_##type(type const &__value, struct buffer_t *__buf) \
	{ \
		__value.encode((char **)&__buf->start, &__buf->len); \
	} \
	void decode_##type(type &__value, struct buffer_t *__buf) \
	{ \
		__value = type(__buf->start, __buf->len); \
	}

endecode_RTI_Value(RTI_AttributeValue);
endecode_RTI_Value(RTI_ParameterValue);
endecode_RTI_Value(RTI_UserSuppliedTag);
endecode_RTI_Value(RTI_EncodedData);

#undef endecod_RTI_Value

/*
 * RTI_Handle:
 *		RTI_FederateHandle, RTI_ObjectClassHandle, RTI_InteractionClassHandle, RTI_ObjectInstanceHandle,
 *		RTI_AttributeHandle, RTI_ParameterHandle, RTI_DimensionHandle, RTI_MessageRetractionHandle,
 *		RTI_RegionHandle
 */
#define endecode_RTI_Handle(type) \
	void encode_##type(type const &__handle, struct buffer_t *__buf) \
	{ \
		__handle.encode((char **)&__buf->start, &__buf->len); \
	} \
	void decode_##type(type &__handle, struct buffer_t *__buf) \
	{ \
		__handle = type(__buf->start, __buf->len); \
	}

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
 * RTI_set
 *		RTI_AttributeHandleSet, RTI_FederateHandleSet, RTI_DimensionHandleSet, RTI_RegionHandleSet
 *
 * NOTE: the release of memory(start_list,len_list) is left to EndecodeFactory:add() function
 */
#define endecode_RTI_set(settype, type) \
	void encode_##settype(settype const &__set, struct buffer_list_t *__buf) \
	{ \
		int num, index = 0; \
		struct buffer_t temp; \
		settype::iterator iter; \
		\
		num = __set.size(); \
		__buf->num = num; \
		__buf->start_list = new void*[num]; \
		__buf->len_list = new uint32_t[num]; \
		\
		for (iter = __set.begin(); iter != __set.end(); iter++) \
		{ \
			encode_##type((*iter), &temp); \
			__buf->start_list[index] = temp.start; \
			__buf->len_list[index] = temp.len; \
			index++; \
		} \
	} \
	void decode_##settype(settype &__set, struct buffer_list_t *__buf) \
	{ \
		int i; \
		struct buffer_t temp; \
		type handle; \
		\
		for (i = 0; i<__buf->num; i++) \
		{ \
			temp.start = __buf->start_list[i]; \
			temp.len = __buf->len_list[i]; \
			decode_##type(handle, &temp); \
			try { \
				__set.insert(handle); \
			} \
			catch (exception &e) { cerr << "STL::set exception: " << e.what() << endl; } \
		} \
	}

endecode_RTI_set(RTI_AttributeHandleSet, RTI_AttributeHandle);
endecode_RTI_set(RTI_FederateHandleSet, RTI_FederateHandle);
endecode_RTI_set(RTI_DimensionHandleSet, RTI_DimensionHandle);
endecode_RTI_set(RTI_RegionHandleSet, RTI_RegionHandle);

#undef endecode_RTI_set

/* 
 * RTI_map
 *		RTI_AttributeHandleValueMap, RTI_ParameterHandleValueMap
 *		
 * NOTE: the release of memory(start_list,len_list) is left to EndecodeFactory:add() function
 */
#define endecode_RTI_map(maptype, firsttype, secondtype) \
	void encode_##maptype(maptype const &__map, struct buffer_list_t *__buf) \
	{ \
		int num, index = 0; \
		struct buffer_t temp; \
		maptype::const_iterator iter; \
		\
		num = __map.size() * 2; \
		__buf->num = num ; \
		__buf->start_list = new void*[num]; \
		__buf->len_list = new uint32_t[num]; \
		for (iter = __map.begin(); iter != __map.end(); iter++) \
		{ \
			/* encode RTI_Handle */ \
			encode_##firsttype(iter->first, &temp); \
			__buf->start_list[index] = temp.start; \
			__buf->len_list[index] = temp.len; \
			index++; \
			\
			/* encode RTI_Value */ \
			encode_##secondtype(iter->second, &temp); \
			__buf->start_list[index] = temp.start; \
			__buf->len_list[index] = temp.len; \
			index++; \
		} \
	} \
	void decode_##maptype(maptype &__map, struct buffer_list_t *__buf) \
	{ \
		int i; \
		struct buffer_t temp; \
		firsttype handle; \
		secondtype value; \
		\
		for (i = 0; i < __buf->num; i+=2) \
		{ \
			/* decode RTI_Handle */ \
			temp.start = __buf->start_list[i]; \
			temp.len = __buf->len_list[i]; \
			decode_##firsttype(handle, &temp); \
			\
			/* decode RTI_Value */ \
			temp.start = __buf->start_list[i+1]; \
			temp.len = __buf->len_list[i+1]; \
			decode_##secondtype(value, &temp); \
			try { \
				__map.insert(maptype::value_type(handle, value)); \
			} \
			catch (exception &e) { cerr << "STL::map exception: " << e.what() << endl; } \
		} \
	} 

endecode_RTI_map(RTI_AttributeHandleValueMap, RTI_AttributeHandle, RTI_AttributeValue);
endecode_RTI_map(RTI_ParameterHandleValueMap, RTI_ParameterHandle, RTI_ParameterValue);

#undef endecode_RTI_map

/*
 * vim: ts=4 sts=4 sw=4
 */
