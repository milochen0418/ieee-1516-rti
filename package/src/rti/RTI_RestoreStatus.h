/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_RestoreStatus.h
 ***********************************************************************/

#ifndef RTI_RestoreStatus_h
#define RTI_RestoreStatus_h

class RTI_bool;

#include "RTI_SpecificConfig.h"
#if 0
#include <RTI_string>
#endif
#include <string>
#define RTI_wstring	std::wstring

// Type safe class used to represent save status of an individual federate.

class RTI_EXPORT RTI_RestoreStatus
{
	public:
		RTI_RestoreStatus(RTI_RestoreStatus const & rhs)
		{
			_RestoreStatus = rhs._RestoreStatus;
		};
		
		static RTI_RestoreStatus const noRestoreInProgress(){ return RTI_RestoreStatus(0); };
		static RTI_RestoreStatus const federateRestoreRequestPending(){ return RTI_RestoreStatus(1); };
		static RTI_RestoreStatus const federateWaitingForRestoreToBegin(){ return RTI_RestoreStatus(2); };
		static RTI_RestoreStatus const federatePreparedToRestore(){ return RTI_RestoreStatus(3); };
		static RTI_RestoreStatus const federateRestoring(){ return RTI_RestoreStatus(4); };
		static RTI_RestoreStatus const federateWaitingForFederationToRestore(){ return RTI_RestoreStatus(5); };
		
		RTI_RestoreStatus & operator=(RTI_RestoreStatus const & rhs)
		{
			_RestoreStatus = rhs._RestoreStatus;
			return *this;
		};
		
		RTI_bool operator==(RTI_RestoreStatus const & rhs) const
		{
			if (_RestoreStatus == rhs._RestoreStatus)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_RestoreStatus const & rhs) const
		{
			if (_RestoreStatus != rhs._RestoreStatus)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _RestoreStatus);
			return StrtoWStr(str);
		};
		
	private:
		RTI_RestoreStatus(unsigned _RestoreStatus)
		{
			this->_RestoreStatus = _RestoreStatus;
		};
		unsigned _RestoreStatus;
};

// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_RestoreStatus const
RTI_NO_RESTORE_IN_PROGRESS = RTI_RestoreStatus::noRestoreInProgress();

RTI_RestoreStatus const
RTI_FEDERATE_RESTORE_REQUEST_PENDING = RTI_RestoreStatus::federateRestoreRequestPending();

RTI_RestoreStatus const
RTI_FEDERATE_WAITING_FOR_RESTORE_TO_BEGIN = RTI_RestoreStatus::federateWaitingForRestoreToBegin();

RTI_RestoreStatus const
RTI_FEDERATE_PREPARED_TO_RESTORE = RTI_RestoreStatus::federatePreparedToRestore();

RTI_RestoreStatus const
RTI_FEDERATE_RESTORING = RTI_RestoreStatus::federateRestoring();

RTI_RestoreStatus const
RTI_FEDERATE_WAITING_FOR_FEDERATION_TO_RESTORE = RTI_RestoreStatus::federateWaitingForFederationToRestore();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_RestoreStatus.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_RestoreStatus_h

/*
 * vim: ts=4 sts=4 sw=4
 */
