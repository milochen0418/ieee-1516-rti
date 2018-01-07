/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_RangeBounds.h
 ***********************************************************************/

#ifndef RTI_RangeBounds_h
#define RTI_RangeBounds_h

#include "RTI_SpecificConfig.h"

class RTI_EXPORT RTI_RangeBounds
{
	public:
		RTI_RangeBounds(){};
		RTI_RangeBounds(unsigned long lowerBound, unsigned long upperBound)
		{
			_lowerBound = lowerBound;
			_upperBound = upperBound;
		};
		
		~RTI_RangeBounds() throw (){};
		
		RTI_RangeBounds(RTI_RangeBounds const & rhs)
		{
			_lowerBound = rhs._lowerBound;
			_upperBound = rhs._upperBound;
		};
		
		RTI_RangeBounds & operator=(RTI_RangeBounds const & rhs)
		{
			_lowerBound = rhs._lowerBound;
			_upperBound = rhs._upperBound;
			return *this;
		};
		
		unsigned long getLowerBound() const { return _lowerBound; };
		
		unsigned long getUpperBound() const { return _upperBound; };
		
		void setLowerBound(unsigned long lowerBound) { _lowerBound = lowerBound; };
		
		void setUpperBound(unsigned long upperBound) { _upperBound = upperBound; };

	private:
		unsigned long _lowerBound;
		unsigned long _upperBound;
};

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_RangeBounds.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_RangeBounds_h

/*
 * vim: ts=4 sts=4 sw=4
 */
