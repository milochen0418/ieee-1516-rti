/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_TransportationType.h
 ***********************************************************************/

#ifndef RTI_TransportationType_h
#define RTI_TransportationType_h

#include <string>

#include "RTI_SpecificConfig.h"
#include "RTI_SpecificTypedefs.h" // for RTI_EncodedData

#define RTI_wstring	std::wstring

// Type safe class used to represent type of data transportation.

class RTI_EXPORT RTI_TransportationType
{
	public:
		RTI_TransportationType(RTI_EncodedData const & rhs)
		{
			memcpy(&_transportationType, rhs.data(), rhs.size());
		};
		
		RTI_TransportationType(RTI_TransportationType const & rhs) throw ()
		{
			_transportationType = rhs._transportationType;
		};
		
		static RTI_TransportationType const reliable() throw() { return RTI_TransportationType(0); };
		static RTI_TransportationType const bestEffort() throw() { return RTI_TransportationType(1); };
		
		RTI_TransportationType & operator =(RTI_TransportationType const & rhs)	throw()
		{
			_transportationType = rhs._transportationType;
			return *this;
		};
		
		RTI_bool operator ==(RTI_TransportationType const & rhs) const throw()
		{
			if (_transportationType == rhs._transportationType)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator !=(RTI_TransportationType const & rhs) const throw()
		{
			if (_transportationType != rhs._transportationType)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _transportationType);
			return StrtoWStr(str);
		};
		
		RTI_EncodedData encode() const throw()
		{ 
			return RTI_EncodedData(&_transportationType, sizeof(_transportationType));
		};

		unsigned getType() const { return _transportationType; };
		
	private:
		RTI_TransportationType(unsigned transportationType) throw()
		{
			_transportationType = transportationType;
		};

		unsigned _transportationType;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_TransportationType const
RTI_RELIABLE = RTI_TransportationType::reliable();

RTI_TransportationType const
RTI_BEST_EFFORT = RTI_TransportationType::bestEffort();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_TransportationType.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_TransportationType_h

/*
 * vim: ts=4 sts=4 sw=4
 */
