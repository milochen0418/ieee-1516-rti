/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_LogicalTimeFactory.h
 ***********************************************************************/

#ifndef RTI_LogicalTimeFactory_h
#define RTI_LogicalTimeFactory_h

// The classes associated with logical time allow a federation to provide
// their own representation for logical time and logical time interval. The
// federation is responsible to inherit from the abstract classes declared
// below. The encoded time classes are used to hold the arbitrary bit
// representation of the logical time and logical time intervals.

#include "RTI_Exceptions.h"

#define RTI_auto_ptr	std::auto_ptr

class RTI_EXPORT RTI_LogicalTimeFactory
{
	public:
		virtual ~RTI_LogicalTimeFactory() throw (){};
		virtual RTI_auto_ptr< RTI_LogicalTime >
			makeInitial() throw (RTI_InternalError) = 0;
		
		virtual RTI_auto_ptr< RTI_LogicalTime > 
			decode(RTI_EncodedLogicalTime const & encodedLogicalTime)
			throw (RTI_InternalError,
					RTI_CouldNotDecode) = 0;
};

#endif // RTI_LogicalTimeFactory_h

/*
 * vim: ts=4 sts=4 sw=4
 */
