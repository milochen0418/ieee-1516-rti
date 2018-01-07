/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_RestoreFailureReason.h
 ***********************************************************************/

#ifndef RTI_RestoreFailureReason_h
#define RTI_RestoreFailureReason_h

#include "RTI_SpecificConfig.h"

#define RTI_wstring	std::wstring

// Type safe class used to represent the reason a restore failed

class RTI_EXPORT RTI_RestoreFailureReason
{
	public:
		RTI_RestoreFailureReason(RTI_RestoreFailureReason const & rhs)
		{
			_RestoreFailureReason = rhs._RestoreFailureReason;
		};
		
		static RTI_RestoreFailureReason const rtiUnableToRestore(){ return RTI_RestoreFailureReason(0); };
		static RTI_RestoreFailureReason const federateReportedFailureDuringRestore(){ return RTI_RestoreFailureReason(1); };
		static RTI_RestoreFailureReason const federateResignedDuringRestore(){ return RTI_RestoreFailureReason(2); };
		static RTI_RestoreFailureReason const rtiDetectedFailureDuringRestore(){ return RTI_RestoreFailureReason(3); };
		
		RTI_RestoreFailureReason & operator=(RTI_RestoreFailureReason const & rhs)
		{
			_RestoreFailureReason = rhs._RestoreFailureReason;
			return *this;
		};
		
		RTI_bool operator==(RTI_RestoreFailureReason const & rhs) const
		{
			if (_RestoreFailureReason == rhs._RestoreFailureReason)
				return RTI_true;
			else
				return RTI_false;
		};

		RTI_bool operator!=(RTI_RestoreFailureReason const & rhs) const
		{
			if (_RestoreFailureReason != rhs._RestoreFailureReason)
				return RTI_true;
			else
				return RTI_false;
		};

		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _RestoreFailureReason);
			return StrtoWStr(str);
		};
		
	private:
		RTI_RestoreFailureReason(unsigned RestoreFailureReason)
		{ 
			_RestoreFailureReason = RestoreFailureReason;
		};
		unsigned _RestoreFailureReason;
};

// These constants Restore a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_RestoreFailureReason const
RTI_RTI_UNABLE_TO_RESTORE = RTI_RestoreFailureReason::rtiUnableToRestore();

RTI_RestoreFailureReason const
RTI_FEDERATE_REPORTED_FAILURE_DURING_RESTORE = RTI_RestoreFailureReason::federateReportedFailureDuringRestore();

RTI_RestoreFailureReason const
RTI_FEDERATE_RESIGNED_DURING_RESTORE = RTI_RestoreFailureReason::federateResignedDuringRestore();

RTI_RestoreFailureReason const
RTI_RTI_DETECTED_FAILURE_DURING_RESTORE = RTI_RestoreFailureReason::rtiDetectedFailureDuringRestore();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_RestoreFailureReason.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_RestoreFailureReason_h

/*
 * vim: ts=4 sts=4 sw=4
 */
