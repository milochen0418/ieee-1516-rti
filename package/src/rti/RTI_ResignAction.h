/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_ResignAction.h
 ***********************************************************************/

#ifndef RTI_ResignAction_h
#define RTI_ResignAction_h

#include <string>

#include "RTI_SpecificConfig.h"

#define RTI_wstring	std::wstring

// Type safe class used to represent action taken on resign.

class RTI_EXPORT RTI_ResignAction
{
	public:
		RTI_ResignAction(RTI_ResignAction const & rhs) 
		{
			_resignAction = rhs._resignAction;
		};
		
		static RTI_ResignAction const unconditionallyDivestAttributes() { return RTI_ResignAction(0); };
		static RTI_ResignAction const deleteObjects() { return RTI_ResignAction(1); };
		static RTI_ResignAction const cancelPendingOwnershipAcquisitions() { return RTI_ResignAction(2); };
		static RTI_ResignAction const deleteObjectsThenDivest() { return RTI_ResignAction(3); };
		static RTI_ResignAction const cancelThenDeleteThenDivest() { return RTI_ResignAction(4); };
		static RTI_ResignAction const noAction() { return RTI_ResignAction(5); };
		
		RTI_ResignAction & operator=(RTI_ResignAction const & rhs)
		{
			_resignAction = rhs._resignAction;
			return *this;
		};
		
		RTI_bool operator==(RTI_ResignAction const & rhs) const
		{
			if (_resignAction == rhs._resignAction)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_bool operator!=(RTI_ResignAction const & rhs) const
		{
			if (_resignAction != rhs._resignAction)
				return RTI_true;
			else
				return RTI_false;
		};
		
		RTI_wstring const toString() const
		{
			char str[100];
			sprintf(str, "%d", _resignAction);
			return StrtoWStr(str);
		};

		unsigned getAction(){ return _resignAction; };

	private:
		RTI_ResignAction(unsigned _resignAction)
		{
			this->_resignAction = _resignAction;
		};
		unsigned _resignAction;
};

// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way

RTI_ResignAction const
RTI_UNCONDITIONALLY_DIVEST_ATTRIBUTES = RTI_ResignAction::unconditionallyDivestAttributes();

RTI_ResignAction const
RTI_DELETE_OBJECTS = RTI_ResignAction::deleteObjects();

RTI_ResignAction const
RTI_CANCEL_PENDING_OWNERSHIP_ACQUISITIONS = RTI_ResignAction::cancelPendingOwnershipAcquisitions();

RTI_ResignAction const
RTI_DELETE_OBJECTS_THEN_DIVEST = RTI_ResignAction::deleteObjectsThenDivest();

RTI_ResignAction const
RTI_CANCEL_THEN_DELETE_THEN_DIVEST = RTI_ResignAction::cancelThenDeleteThenDivest();

RTI_ResignAction const
RTI_NO_ACTION = RTI_ResignAction::noAction();

#if 0
#ifdef RTI_USE_INLINE
#include "RTI_ResignAction.i"
#endif // RTI_USE_INLINE
#endif

#endif // RTI_ResignAction_h
/*
 * vim: ts=4 sts=4 sw=4
 */
