/***********************************************************************
 * IEEE 1516.1 High Level Architecture Interface Specification C++ API
 * File: RTI_Value.h
 * ***********************************************************************/
#ifndef RTI_Value_h
#define RTI_Value_h

#include "RTI_SpecificConfig.h"

class RTI_VariableLengthValueClass {
	public:
		RTI_VariableLengthValueClass() : data(NULL), size(0) {}
		RTI_VariableLengthValueClass(void const * __data, size_t __size)
		{
			data = new char[__size];
			memcpy(data, __data, __size);
			size = __size;
		};
		
		~RTI_VariableLengthValueClass() throw ()
		{ 
			if (data)
				delete [] data;
		};

		RTI_VariableLengthValueClass & operator=(RTI_VariableLengthValueClass const & rhs)
		{
			data = new char[rhs.size];
			memcpy(data, rhs.data, rhs.size);
			size = rhs.size;
			return *this;
		}
		
		char *data;
		size_t size;
};

// The RTI_Value class is used to as a generic value holder that contains a
// pointer to the memory location and the size of the value. This class is
// used for attributes, parameters, and user supplied tags.The constructor
// takes a pointer to the data value and the size of the data. The key methods
// on this class is the data method which returns a constant pointer to the
// value memory location, and the size method which returns the size in bytes
// of the value. The templatized class T provides RTI implementations the
// ability to customize their particular implementation. The int template type
// parameter provides the ability to support strong typing.

template < class T, int i >
RTI_EXPORT class RTI_Value
{
	public:
		RTI_Value(){};
		RTI_Value(void const * data, size_t size) : _impl(data, size) { };
		~RTI_Value() throw (){};
		
		RTI_Value(RTI_Value const & rhs): _impl(rhs.data(), rhs.size()){};
		
		RTI_Value &	operator=(RTI_Value const & rhs)
		{
			_impl = rhs._impl;
			return *this;
		};
		
		void const *data() const { return _impl.data; };

		size_t size() const { return _impl.size; };

		void encode(char **__data, uint32_t *__size) const
		{
			*__data = _impl.data;
			*__size = _impl.size;
		};
		
	private:
		T _impl;
};

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_Value.i"
#endif // RTI_USE_INLINE
#ifdef RTI_TEMPLATES_REQUIRE_SOURCE
#include "RTI_Value.cpp"
#endif // RTI_TEMPLATES_REQUIRE_SOURCE
#endif

#endif // RTI_Value_h

/*
 * vim: ts=4 sts=4 sw=4
 */
