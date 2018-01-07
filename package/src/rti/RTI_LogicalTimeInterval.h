/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_LogicalTimeInterval.h
 ***********************************************************************/

#ifndef RTI_LogicalTimeInterval_h
#define RTI_LogicalTimeInterval_h

// The classes associated with logical time allow a federation to provide
// their own representation for logical time and logical time interval. The
// federation is responsible to inherit from the abstract classes declared
// below. The encoded time classes are used to hold the arbitrary bit
// representation of the logical time and logical time intervals.

#include <memory>

#include "RTI_EncodedLogicalTimeInterval.h"

#define RTI_auto_ptr std::auto_ptr

class RTI_EXPORT RTI_LogicalTimeInterval
{
	public:
		virtual ~RTI_LogicalTimeInterval() throw ();
		virtual
			void
			setZero() = 0;
		virtual
			RTI_bool
			isZero() = 0;
		virtual
			RTI_bool
			isEpsilon() = 0;
		virtual
			void
			setTo(RTI_LogicalTimeInterval const & value)
			throw (RTI_InvalidLogicalTimeInterval) = 0;
		virtual
			RTI_auto_ptr< RTI_LogicalTimeInterval >
			subtract(RTI_LogicalTimeInterval const & subtrahend) const
			throw (RTI_InvalidLogicalTimeInterval) = 0;
		virtual
			RTI_bool
			isGreaterThan(RTI_LogicalTimeInterval const & value) const
			throw (RTI_InvalidLogicalTimeInterval) = 0;
		virtual
			RTI_bool
			isLessThan(RTI_LogicalTimeInterval const & value) const
			throw (RTI_InvalidLogicalTimeInterval) = 0;
		virtual
			RTI_bool
			isEqualTo(RTI_LogicalTimeInterval const & value) const
			throw (RTI_InvalidLogicalTimeInterval) = 0;
		virtual
			RTI_bool
			isGreaterThanOrEqualTo(RTI_LogicalTimeInterval const & value) const
			throw (RTI_InvalidLogicalTimeInterval) = 0;
		virtual
			RTI_bool
			isLessThanOrEqualTo(RTI_LogicalTimeInterval const & value) const
			throw (RTI_InvalidLogicalTimeInterval) = 0;
		
		virtual RTI_auto_ptr< RTI_EncodedLogicalTimeInterval > encode() const = 0;
		virtual	RTI_wstring toString() const = 0;
};
#endif // RTI_LogicalTimeInterval_h
/*
 * vim: ts=4 sts=4 sw=4
 */
