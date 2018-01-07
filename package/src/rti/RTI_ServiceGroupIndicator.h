/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_ServiceGroupIndicator.h
 ***********************************************************************/

#ifndef RTI_ServiceGroupIndicator_h
#define RTI_ServiceGroupIndicator_h

#include <string>

#include "RTI_SpecificConfig.h"

#define RTI_wstring	std::wstring

// Type safe class used to represent the service group

class RTI_EXPORT RTI_ServiceGroupIndicator
{
	public:
		RTI_ServiceGroupIndicator(RTI_ServiceGroupIndicator const & rhs)
		{
			_ServiceGroupIndicator = rhs._ServiceGroupIndicator;
		};
		
		static RTI_ServiceGroupIndicator const federationManagement() { return RTI_ServiceGroupIndicator(0); };
		static RTI_ServiceGroupIndicator const declarationManagement() { return RTI_ServiceGroupIndicator(1); };
		static RTI_ServiceGroupIndicator const objectManagement() { return RTI_ServiceGroupIndicator(2); };
		static RTI_ServiceGroupIndicator const ownershipManagement() { return RTI_ServiceGroupIndicator(3); };
		static RTI_ServiceGroupIndicator const timeManagement() { return RTI_ServiceGroupIndicator(4); };
		static RTI_ServiceGroupIndicator const dataDistributionManagement() { return RTI_ServiceGroupIndicator(5); };
		static RTI_ServiceGroupIndicator const supportServices() { return RTI_ServiceGroupIndicator(6); };
		
		RTI_ServiceGroupIndicator & operator=(RTI_ServiceGroupIndicator const & rhs)
		{
			_ServiceGroupIndicator = rhs._ServiceGroupIndicator;
			return *this;
		};
		
		RTI_bool operator==(RTI_ServiceGroupIndicator const & rhs) const
		{
			if (_ServiceGroupIndicator == rhs._ServiceGroupIndicator)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_ServiceGroupIndicator const & rhs) const
		{
			if (_ServiceGroupIndicator != rhs._ServiceGroupIndicator)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _ServiceGroupIndicator);
			return StrtoWStr(str);
		};
		
	private:
		RTI_ServiceGroupIndicator(unsigned _ServiceGroupIndicator)
		{
			this->_ServiceGroupIndicator = _ServiceGroupIndicator;
		};
		unsigned _ServiceGroupIndicator;
};

// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_ServiceGroupIndicator const
RTI_FEDERATION_MANAGEMENT = RTI_ServiceGroupIndicator::federationManagement();

RTI_ServiceGroupIndicator const
RTI_DECLARATION_MANAGEMENT = RTI_ServiceGroupIndicator::declarationManagement();

RTI_ServiceGroupIndicator const
RTI_OBJECT_MANAGEMENT = RTI_ServiceGroupIndicator::objectManagement();

RTI_ServiceGroupIndicator const
RTI_OWNERSHIP_MANAGEMENT = RTI_ServiceGroupIndicator::ownershipManagement();

RTI_ServiceGroupIndicator const
RTI_TIME_MANAGEMENT = RTI_ServiceGroupIndicator::timeManagement();

RTI_ServiceGroupIndicator const
RTI_DATA_DISTRIBUTION_MANAGEMENT = RTI_ServiceGroupIndicator::dataDistributionManagement();

RTI_ServiceGroupIndicator const
RTI_SUPPORT_SERVICES = RTI_ServiceGroupIndicator::supportServices();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_ServiceGroupIndicator.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_ServiceGroupIndicator_h
/*
 * vim: ts=4 sts=4 sw=4
 */
