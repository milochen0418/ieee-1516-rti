/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_SynchronizationFailureReason.h
 ***********************************************************************/

#ifndef RTI_SynchronizationFailureReason_h
#define RTI_SynchronizationFailureReason_h

#include <string>

#include "RTI_bool.h"
#include "RTI_SpecificConfig.h"

#define RTI_wstring std::wstring

// Type safe class used to represent type of data order.
class RTI_EXPORT RTI_SynchronizationFailureReason
{
	public:
		RTI_SynchronizationFailureReason(RTI_SynchronizationFailureReason const & rhs)
		{
			_SynchronizationFailureReason = rhs._SynchronizationFailureReason;
		};
		
		static RTI_SynchronizationFailureReason const synchronizationPointLabelNotUnique()
		{
			return RTI_SynchronizationFailureReason(0);
		};
		static RTI_SynchronizationFailureReason const synchronizationSetMemberNotJoined()
		{
			return RTI_SynchronizationFailureReason(1);
		};
		
		RTI_SynchronizationFailureReason & operator=(RTI_SynchronizationFailureReason const & rhs)
		{
			_SynchronizationFailureReason = rhs._SynchronizationFailureReason;
			return *this;
		};

		RTI_bool operator==(RTI_SynchronizationFailureReason const & rhs) const
		{
			if (_SynchronizationFailureReason == rhs._SynchronizationFailureReason)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_SynchronizationFailureReason const & rhs) const
		{
			if (_SynchronizationFailureReason != rhs._SynchronizationFailureReason)
				return RTI_true;
			else
				return RTI_false;
		};

		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _SynchronizationFailureReason);
			return StrtoWStr(str);
		};
		
	private:
		RTI_SynchronizationFailureReason(unsigned SynchronizationFailureReason)
		{
			_SynchronizationFailureReason = SynchronizationFailureReason;
		};
		unsigned _SynchronizationFailureReason;
};

// These constants Synchronization a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_SynchronizationFailureReason const
RTI_SYNCHRONIZATION_POINT_LABEL_NOT_UNIQUE = RTI_SynchronizationFailureReason::synchronizationPointLabelNotUnique();

RTI_SynchronizationFailureReason const
RTI_SYNCHRONIZATION_SET_MEMBER_NOT_JOINED = RTI_SynchronizationFailureReason::synchronizationSetMemberNotJoined();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_SynchronizationFailureReason.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_SynchronizationFailureReason_h
/*
 * vim: ts=4 sts=4 sw=4
 */
