/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_bool.h
 ***********************************************************************/

#ifndef RTI_bool_h
#define RTI_bool_h

#include <iostream>
#include <string>
#include "RTI_SpecificConfig.h"

#define RTI_wstring std::wstring

// This is a type-safe boolean class. Not all platforms support the bool
// type. Consequently, one must be provided.
//

class RTI_EXPORT RTI_bool
{
	public:
		RTI_bool(){};
		RTI_bool(unsigned boolean) { _boolean = boolean; };
		RTI_bool(RTI_bool const & b) { _boolean = b._boolean; } ;
		~RTI_bool()	throw() {};
		
		static RTI_bool const True(){ return RTI_bool(1); };
		
		static RTI_bool const False(){ return RTI_bool(0); };
		
		operator unsigned() const{ return _boolean;	};
		
		RTI_bool const operator==(RTI_bool const & rhs) const
		{
			if (_boolean == rhs._boolean)
				return RTI_bool(1);
			else
				return RTI_bool(0);
		};
		
		RTI_bool const operator!=(RTI_bool const & rhs) const
		{
			if (_boolean != rhs._boolean)
				return RTI_bool(0);
			else
				return RTI_bool(1);
		};
		
		RTI_wstring toString() const
		{
			char str[100];
			sprintf(str, "%d", _boolean);
			return StrtoWStr(str);
		};
		
	private:
		unsigned _boolean;
};

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_bool.i"
#endif // RTI_USE_INLINE
#endif

RTI_bool const RTI_true = RTI_bool::True();
RTI_bool const RTI_false = RTI_bool::False();

#endif // RTI_bool_h

/*
 * vim: ts=4 sts=4 sw=4
 */
