/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_SaveStatus.h
 ***********************************************************************/

#ifndef RTI_SaveStatus_h
#define RTI_SaveStatus_h

#include <string>

#include "RTI_SpecificConfig.h"

#define RTI_wstring	std::wstring

// Type safe class used to represent save status of an individual federate.

class RTI_EXPORT RTI_SaveStatus
{
	public:
		RTI_SaveStatus(RTI_SaveStatus const & rhs)
		{
			_SaveStatus = rhs._SaveStatus;
		};
		
		static RTI_SaveStatus const noSaveInProgress(){ return RTI_SaveStatus(0); };
		static RTI_SaveStatus const federateInstructedToSave(){ return RTI_SaveStatus(1); };
		static RTI_SaveStatus const federateSaving(){ return RTI_SaveStatus(2); };
		static RTI_SaveStatus const federateWaitingForFederationToSave(){ return RTI_SaveStatus(3); } ;
		
		RTI_SaveStatus & operator=(RTI_SaveStatus const & rhs)
		{
			_SaveStatus = rhs._SaveStatus;
			return *this;
		};
		
		RTI_bool operator==(RTI_SaveStatus const & rhs) const
		{
			if (_SaveStatus == rhs._SaveStatus)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_SaveStatus const & rhs) const
		{
			if (_SaveStatus != rhs._SaveStatus)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _SaveStatus);
			return StrtoWStr(str);
		};

	private:
		RTI_SaveStatus(unsigned _SaveStatus)
		{
			this->_SaveStatus = _SaveStatus;
		};
		unsigned _SaveStatus;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_SaveStatus const
RTI_NO_SAVE_IN_PROGRESS = RTI_SaveStatus::noSaveInProgress();

RTI_SaveStatus const
RTI_FEDERATE_INSTRUCTED_TO_SAVE = RTI_SaveStatus::federateInstructedToSave();

RTI_SaveStatus const
RTI_FEDERATE_SAVING = RTI_SaveStatus::federateSaving();

RTI_SaveStatus const
RTI_FEDERATE_WAITING_FOR_FEDERATION_TO_SAVE = RTI_SaveStatus::federateWaitingForFederationToSave();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_SaveStatus.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_SaveStatus_h

/*
 * vim: ts=4 sts=4 sw=4
 */
