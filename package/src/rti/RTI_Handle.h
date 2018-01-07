/***********************************************************************
 * IEEE 1516.1 High Level Architecture Interface Specification C++ API
 * File: RTI_Handle.h
 * ***********************************************************************/
#ifndef RTI_Handle_h
#define RTI_Handle_h

#include <string>

#if 0
#include "RTI_SpecificConfig.h"
#endif
#include "RTI_bool.h"

// The RTIhandle class is used to provide the common interface to the different
// RTI handle types used in the API. This interface includes a constructor,
// assignment, equality, inequality, and less than operators. The encode method
// returns a type safe EncodedHandleClass instance that can be used to exchange
// handles between federates as attributes and parameters. The constructor takes
// a EncodedHandleClass which enables the type safe creation of a RTIhandle from
// encoded data passed to a federate. The template parameter class
// ImplementationSpecificHandleClass provides RTI implementations the ability
// to customize a private class member for their particular implementation.The
// int template type parameter provides the ability to support strong typing.

template< class ImplementationSpecificHandleClass,
	class EncodedHandleClass,
	class ImplementationSpecificHandleClassFriend,
	int i>
class RTI_EXPORT RTI_Handle
{
	public:
		explicit RTI_Handle(EncodedHandleClass encodedHandle)
		{
			memcpy(&_impl, encodedHandle.data(), encodedHandle.size());
		};

		~RTI_Handle() throw() {};
		
		RTI_Handle(RTI_Handle const & rhs) { _impl = rhs._impl; };
		
		RTI_Handle & operator=(RTI_Handle const & rhs)
		{ 
			_impl = rhs._impl;
			return *this;
		};
		
		RTI_bool operator==(RTI_Handle const & rhs) const 
		{
			if (_impl == rhs._impl)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_Handle const & rhs) const
		{
			if (_impl != rhs._impl)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator< (RTI_Handle const & rhs) const
		{
			if (_impl < rhs._impl)
				return RTI_true;
			else
				return RTI_false;
		};
		
		EncodedHandleClass encode() const { return EncodedHandleClass(&_impl, sizeof(_impl)); };
		
		void encode(char **__data, uint32_t *__size) const
		{
			*__data = (char *)&_impl;
			*__size = sizeof(_impl);
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%ld", _impl);
			return StrtoWStr(str);
		};

		ImplementationSpecificHandleClass getHandle() const
		{	
			
			return _impl;
		}

#if 0
	private:
		ImplementationSpecificHandleClass _impl;
		//
		// This class is the only class which can construct an RTI_Handle
		//
		friend ImplementationSpecificHandleClassFriend;

		RTI_Handle(ImplementationSpecificHandleClass const & impl);
#endif
	/* 
	 * NOTE: The usage with template with friend function is not allowed in the C++ standard.
	 *		The above codes are modified by the suggestion from
	 *		  http://discussions.sisostds.org/default.asp?action=9&fid=26&read=40001
	 */
	private:
		ImplementationSpecificHandleClass _impl;
		//
		// This class is the only class which can construct an RTI_Handle
		//
		
	public:
	    RTI_Handle(ImplementationSpecificHandleClass const & impl){ _impl = impl; };

		RTI_Handle(){};

	public:
		RTI_Handle(void *data, size_t size) 
		{
			memcpy(&_impl, data, size);
		};
};

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_Handle.i"
#endif // RTI_USE_INLINE
#ifdef RTI_TEMPLATES_REQUIRE_SOURCE
#include "RTI_Handle.cpp"
#endif // RTI_TEMPLATES_REQUIRE_SOURCE
#endif

#endif // RTI_Handle_h

/*
 * vim: ts=4 sts=4 sw=4
 */
