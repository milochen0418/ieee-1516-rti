/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_OrderType.h
 ***********************************************************************/

#ifndef RTI_OrderType_h
#define RTI_OrderType_h

#include <string>

#include "RTI_SpecificConfig.h"
#include "RTI_SpecificTypedefs.h" // for RTI_EncodedData

#define RTI_wstring	std::wstring

// Type safe class used to represent type of data order.
class RTI_EXPORT RTI_OrderType
{
	public:
		RTI_OrderType(RTI_EncodedData const & theEncodedOrderType)
		{
			memcpy(&_orderType, theEncodedOrderType.data(), theEncodedOrderType.size());
		};

		RTI_OrderType(RTI_OrderType const & rhs) throw()
		{
			_orderType = rhs._orderType;
		};
		
		static RTI_OrderType const receive() throw(){ return RTI_OrderType(0); };
		static RTI_OrderType const timestamp() throw() { return RTI_OrderType(1); };
		
		RTI_OrderType & operator=(RTI_OrderType const & rhs) throw ()
		{
			_orderType = rhs._orderType;
			return *this;
		};
		
		RTI_bool operator==(RTI_OrderType const & rhs) const throw()
		{
			if (_orderType == rhs._orderType)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_OrderType const & rhs) const throw()
		{
			if (_orderType != rhs._orderType)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _orderType);
			return StrtoWStr(str);
		};
		
		RTI_EncodedData encode() const throw()
		{
			return RTI_EncodedData(&_orderType, sizeof(_orderType)); 
		};
		
	private:
		RTI_OrderType(unsigned orderType) throw() { _orderType = orderType;};
		unsigned _orderType;
};

// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_OrderType const
RTI_RECEIVE = RTI_OrderType::receive();

RTI_OrderType const
RTI_TIMESTAMP = RTI_OrderType::timestamp();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_OrderType.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_OrderType_h

/*
 * vim: ts=4 sts=4 sw=4
 */
