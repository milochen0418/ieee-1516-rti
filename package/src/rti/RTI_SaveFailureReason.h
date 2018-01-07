/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_SaveFailureReason.h
 ***********************************************************************/

#ifndef RTI_SaveFailureReason_h
#define RTI_SaveFailureReason_h

#include <string>

#include "RTI_SpecificConfig.h"

#define RTI_wstring	std::wstring

// Type safe class used to represent type of data order.

class RTI_EXPORT RTI_SaveFailureReason
{
	public:
		RTI_SaveFailureReason(RTI_SaveFailureReason const & rhs)
		{
			_saveFailureReason = rhs._saveFailureReason;
		};
		
		static RTI_SaveFailureReason const rtiUnableToSave(){ return RTI_SaveFailureReason(0); };
		static RTI_SaveFailureReason const federateReportedFailureDuringSave(){ return RTI_SaveFailureReason(1); };
		static RTI_SaveFailureReason const federateResignedDuringSave(){ return RTI_SaveFailureReason(2); };
		static RTI_SaveFailureReason const rtiDetectedFailureDuringSave(){ return RTI_SaveFailureReason(3); };
		static RTI_SaveFailureReason const saveTimeCannotBeHonored(){ return RTI_SaveFailureReason(4); };
		
		RTI_SaveFailureReason & operator=(RTI_SaveFailureReason const & rhs)
		{
			_saveFailureReason = rhs._saveFailureReason;
			return *this;
		};
		
		RTI_bool operator==(RTI_SaveFailureReason const & rhs) const
		{
			if (_saveFailureReason == rhs._saveFailureReason)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_SaveFailureReason const & rhs) const
		{
			if (_saveFailureReason != rhs._saveFailureReason)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _saveFailureReason);
			return StrtoWStr(str);
		};
		
	private:
		RTI_SaveFailureReason(unsigned saveFailureReason){
			_saveFailureReason = saveFailureReason; 
		};
		unsigned _saveFailureReason;
};

// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_SaveFailureReason const
RTI_RTI_UNABLE_TO_SAVE = RTI_SaveFailureReason::rtiUnableToSave();

RTI_SaveFailureReason const
RTI_FEDERATE_REPORTED_FAILURE_DURING_SAVE = RTI_SaveFailureReason::federateReportedFailureDuringSave();

RTI_SaveFailureReason const
RTI_FEDERATE_RESIGNED_DURING_SAVE = RTI_SaveFailureReason::federateResignedDuringSave();

RTI_SaveFailureReason const
RTI_RTI_DETECTED_FAILURE_DURING_SAVE = RTI_SaveFailureReason::rtiDetectedFailureDuringSave();

RTI_SaveFailureReason const
RTI_SAVE_TIME_CANNOT_BE_HONORED = RTI_SaveFailureReason::saveTimeCannotBeHonored();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_SaveFailureReason.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_SaveFailureReason_h

/*
 * vim: ts=4 sts=4 sw=4
 */
