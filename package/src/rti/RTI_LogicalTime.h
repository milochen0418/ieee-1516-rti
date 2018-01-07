/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_LogicalTime.h
 ***********************************************************************/

#ifndef RTI_LogicalTime_h
#define RTI_LogicalTime_h

// The classes associated with logical time allow a federation to provide their
// own representation for logical time and logical time interval. The federation
// is responsible to inherit from the abstract classes declared below. The
// encoded time classes are used to hold the arbitrary bit representation of the
// logical time and logical time intervals.

#include <memory>

#include "RTI_LogicalTimeInterval.h"
#include "RTI_EncodedLogicalTime.h"

#define RTI_auto_ptr	std::auto_ptr

class RTI_EXPORT RTI_LogicalTime
{
	public:
		virtual	~RTI_LogicalTime()throw ();
		virtual	void setInitial() = 0;
		virtual RTI_bool isInitial() = 0;
		virtual void setFinal() = 0;
		virtual	RTI_bool isFinal() = 0;
		virtual	void setTo(RTI_LogicalTime const & value)
			throw (RTI_InvalidLogicalTime) = 0;
		virtual	void increaseBy(RTI_LogicalTimeInterval const & addend)
			throw (RTI_IllegalTimeArithmetic, RTI_InvalidLogicalTimeInterval) = 0;
		virtual	void decreaseBy(RTI_LogicalTimeInterval const & subtrahend)
			throw (RTI_IllegalTimeArithmetic, RTI_InvalidLogicalTimeInterval) = 0;
		virtual RTI_auto_ptr< RTI_LogicalTimeInterval >
			subtract(RTI_LogicalTime const & subtrahend) const
			throw (RTI_InvalidLogicalTime) = 0;
		virtual RTI_bool isGreaterThan(RTI_LogicalTime const & value) const
			throw (RTI_InvalidLogicalTime) = 0;
		virtual RTI_bool isLessThan(RTI_LogicalTime const & value) const
			throw (RTI_InvalidLogicalTime) = 0;
		virtual RTI_bool isEqualTo(RTI_LogicalTime const & value) const
			throw (RTI_InvalidLogicalTime) = 0;
		virtual RTI_bool isGreaterThanOrEqualTo(RTI_LogicalTime const & value) const
			throw (RTI_InvalidLogicalTime) = 0;
		virtual RTI_bool isLessThanOrEqualTo(RTI_LogicalTime const & value) const
			throw (RTI_InvalidLogicalTime) = 0;
		virtual RTI_auto_ptr< RTI_EncodedLogicalTime > encode() const = 0;
		virtual RTI_wstring toString() const = 0;
		
};
#endif // RTI_LogicalTime_h

/*
 * vim: ts=4 sts=4 sw=4
 */
