/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_EncodedLogicalTimeInterval.h
 ***********************************************************************/

#ifndef RTI_EncodedLogicalTimeInterval_h
#define RTI_EncodedLogicalTimeInterval_h

#include "RTI_SpecificConfig.h"

class RTI_EncodedLogicalTimeInterval
{
	public:
		virtual ~RTI_EncodedLogicalTimeInterval() throw () {}
		virtual void const * data() const = 0;
		virtual size_t size() const = 0;
};

#endif // RTI_EncodedLogicalTimeInterval_h


/*
 * vim: ts=4 sts=4 sw=4
 */
