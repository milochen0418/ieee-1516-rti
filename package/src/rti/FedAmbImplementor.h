#ifndef FedAmbImplementor_h
#define FedAmbImplementor_h

#include "RTI_FederateAmbassador.h"

class FedAmbImplementor : public RTI_FederateAmbassador
{
	public:
		FedAmbImplementor() throw (RTI_FederateInternalError){};
		virtual	~FedAmbImplementor() throw (){};
		
		// 4.7
		virtual	void synchronizationPointRegistrationSucceeded
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError){};
		
		virtual void synchronizationPointRegistrationFailed
			(RTI_wstring const & label,
			 RTI_SynchronizationFailureReason theReason)
			throw (RTI_FederateInternalError){};
		
		// 4.8
		virtual void announceSynchronizationPoint
			(RTI_wstring const & label,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_FederateInternalError){};
		
		// 4.10
		virtual	void federationSynchronized
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError){};
		
		// 4.12
		virtual	void initiateFederateSave
			(RTI_wstring const & label)
			throw (RTI_UnableToPerformSave,
					RTI_FederateInternalError){};
		
		virtual	void initiateFederateSave
			(RTI_wstring const & label,
			 RTI_LogicalTime const & theTime)
			throw (RTI_UnableToPerformSave,
					RTI_InvalidLogicalTime,
					RTI_FederateInternalError){};
		
		// 4.15
		virtual void federationSaved()
			throw (RTI_FederateInternalError){};
		
		virtual	void federationNotSaved
			(RTI_SaveFailureReason theSaveFailureReason)
			throw (RTI_FederateInternalError){};
		
		// 4.17
		virtual void federationSaveStatusResponse
			(RTI_auto_ptr<RTI_FederateHandleSaveStatusPairVector > theFederateStatusVector)
			throw (RTI_FederateInternalError){};
		
		// 4.19
		virtual void requestFederationRestoreSucceeded
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError){};
		
		virtual void requestFederationRestoreFailed()
			throw (RTI_FederateInternalError){};
		
		// 4.20
		virtual void federationRestoreBegun()
			throw (RTI_FederateInternalError){};
		
		// 4.21
		virtual void initiateFederateRestore
			(RTI_wstring const & label, 
			 RTI_FederateHandle const & handle)
			throw (RTI_SpecifiedSaveLabelDoesNotExist,
					RTI_CouldNotInitiateRestore,
					RTI_FederateInternalError){};
		
		// 4.23
		virtual void federationRestored()
			throw (RTI_FederateInternalError){};
		
		virtual void federationNotRestored
			(RTI_RestoreFailureReason theRestoreFailureReason)
			throw (RTI_FederateInternalError){};
		
		// 4.25
		virtual void federationRestoreStatusResponse
			(RTI_auto_ptr<RTI_FederateHandleRestoreStatusPairVector > theFederateStatusVector)
			throw (RTI_FederateInternalError){};
		
		/////////////////////////////////////
		// Declaration Management Services //
		/////////////////////////////////////
		
		// 5.10
		virtual void startRegistrationForObjectClass
			(RTI_ObjectClassHandle const & theClass)
			throw (RTI_ObjectClassNotPublished,
					RTI_FederateInternalError){};
		
		// 5.11
		virtual void stopRegistrationForObjectClass
			(RTI_ObjectClassHandle const & theClass)
			throw (RTI_ObjectClassNotPublished,
					RTI_FederateInternalError){};

		// 5.12
		virtual void turnInteractionsOn
			(RTI_InteractionClassHandle const & theHandle)
			throw (RTI_InteractionClassNotPublished,
					RTI_FederateInternalError){};
		
		// 5.13
		virtual void turnInteractionsOff
			(RTI_InteractionClassHandle const & theHandle)
			throw (RTI_InteractionClassNotPublished,
					RTI_FederateInternalError){};
		
		////////////////////////////////
		// Object Management Services //
		////////////////////////////////
		
		// 6.3
		virtual void objectInstanceNameReservationSucceeded
			(RTI_wstring const & theObjectInstanceName)
			throw (RTI_UnknownName,
					RTI_FederateInternalError){};
		
		virtual void objectInstanceNameReservationFailed
			(RTI_wstring const & theObjectInstanceName)
			throw (RTI_UnknownName,
					RTI_FederateInternalError){};
		
		// 6.5
		virtual void discoverObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_ObjectClassHandle const & theObjectClass,
			 RTI_wstring const & theObjectInstanceName)
			throw (RTI_CouldNotDiscover,
					RTI_ObjectClassNotKnown,
					RTI_FederateInternalError){};
		
		// 6.7
		virtual void reflectAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual void reflectAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_RegionHandleSet const & theSentRegionHandleSet)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual void reflectAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual void reflectAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder,
			 RTI_RegionHandleSet const & theSentRegionHandleSet)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual	void reflectAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder,
			 RTI_MessageRetractionHandle const & theHandle)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_InvalidLogicalTime,
					RTI_FederateInternalError){};
		
		virtual void reflectAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder,
			 RTI_MessageRetractionHandle const & theHandle,
			 RTI_RegionHandleSet const & theSentRegionHandleSet)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_InvalidLogicalTime,
					RTI_FederateInternalError){};
		// 6.9
		virtual void receiveInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType)
			throw (RTI_InteractionClassNotRecognized,
					RTI_InteractionParameterNotRecognized,
					RTI_InteractionClassNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual	void receiveInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_RegionHandleSet const & theSentRegionHandleSet)
			throw (RTI_InteractionClassNotRecognized,
					RTI_InteractionParameterNotRecognized,
					RTI_InteractionClassNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual void receiveInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder)
			throw (RTI_InteractionClassNotRecognized,
					RTI_InteractionParameterNotRecognized,
					RTI_InteractionClassNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual	void receiveInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder,
			 RTI_RegionHandleSet const & theSentRegionHandleSet)
			throw (RTI_InteractionClassNotRecognized,
					RTI_InteractionParameterNotRecognized,
					RTI_InteractionClassNotSubscribed,
					RTI_FederateInternalError){};
		
		virtual	void receiveInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder,
			 RTI_MessageRetractionHandle const & theHandle)
			throw (RTI_InteractionClassNotRecognized,
					RTI_InteractionParameterNotRecognized,
					RTI_InteractionClassNotSubscribed,
					RTI_InvalidLogicalTime,
					RTI_FederateInternalError){};
		
		virtual void receiveInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder,
			 RTI_MessageRetractionHandle const & theHandle,
			 RTI_RegionHandleSet const & theSentRegionHandleSet)
			throw (RTI_InteractionClassNotRecognized,
					RTI_InteractionParameterNotRecognized,
					RTI_InteractionClassNotSubscribed,
					RTI_InvalidLogicalTime,
					RTI_FederateInternalError){};
		
		// 6.11
		virtual void removeObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_FederateInternalError){};
		
		virtual void removeObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_FederateInternalError){};
		
		virtual void removeObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_LogicalTime const & theTime,
			 RTI_OrderType const & receivedOrder,
			 RTI_MessageRetractionHandle const & theHandle)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_InvalidLogicalTime,
					RTI_FederateInternalError){};
		
		// 6.15
		virtual void attributesInScope
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_FederateInternalError){};
		
		// 6.16
		virtual void attributesOutOfScope
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_FederateInternalError){};
		
		// 6.18
		virtual void provideAttributeValueUpdate
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > theAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotOwned,
					RTI_FederateInternalError){};
		
		// 6.19
		virtual void turnUpdatesOnForObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotOwned,
					RTI_FederateInternalError){};
		
		// 6.20
		virtual void turnUpdatesOffForObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotOwned,
					RTI_FederateInternalError){};

		///////////////////////////////////
		// Ownership Management Services //
		///////////////////////////////////
		
		// 7.4
		virtual void requestAttributeOwnershipAssumption
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > offeredAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeAlreadyOwned,
					RTI_AttributeNotPublished,
					RTI_FederateInternalError){};
		
		// 7.5
		virtual	void requestDivestitureConfirmation
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > releasedAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotOwned,
					RTI_AttributeDivestitureWasNotRequested,
					RTI_FederateInternalError){};
		
		// 7.7
		virtual void attributeOwnershipAcquisitionNotification
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > securedAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeAcquisitionWasNotRequested,
					RTI_AttributeAlreadyOwned,
					RTI_AttributeNotPublished,
					RTI_FederateInternalError){};

		// 7.10
		virtual	void attributeOwnershipUnavailable
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeAlreadyOwned,
					RTI_AttributeAcquisitionWasNotRequested,
					RTI_FederateInternalError){};
		
		// 7.11
		virtual void requestAttributeOwnershipRelease
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > candidateAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotOwned,
					RTI_FederateInternalError){};
		
		// 7.15
		virtual void confirmAttributeOwnershipAcquisitionCancellation
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet > theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeAlreadyOwned,
					RTI_AttributeAcquisitionWasNotCanceled,
					RTI_FederateInternalError){};
		
		// 7.17
		virtual void informAttributeOwnership
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandle const & theAttribute,
			 RTI_FederateHandle const & theOwner)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_FederateInternalError){};
		
		virtual	void attributeIsNotOwned
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandle const & theAttribute)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_FederateInternalError){};
		
		virtual	void attributeIsOwnedByRTI
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandle const & theAttribute)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_FederateInternalError){};
		
		//////////////////////////////
		// Time Management Services //
		//////////////////////////////
		
		// 8.3
		virtual	void timeRegulationEnabled
			(RTI_LogicalTime const & theFederateTime)
			throw (RTI_InvalidLogicalTime,
					RTI_NoRequestToEnableTimeRegulationWasPending,
					RTI_FederateInternalError){};
		
		// 8.6
		virtual	void timeConstrainedEnabled
			(RTI_LogicalTime const & theFederateTime)
			throw (RTI_InvalidLogicalTime,
					RTI_NoRequestToEnableTimeConstrainedWasPending,
					RTI_FederateInternalError){};
		
		// 8.13
		virtual	void timeAdvanceGrant
			(RTI_LogicalTime const & theTime)
			throw (RTI_InvalidLogicalTime,
					RTI_JoinedFederateIsNotInTimeAdvancingState,
					RTI_FederateInternalError){};
		
		// 8.22
		virtual void requestRetraction
			(RTI_MessageRetractionHandle const & theHandle)
			throw (RTI_FederateInternalError){};
};

#endif // RTI_FederateAmbassador_h
/*
 * vim: ts=4 sts=4 sw=4
 */
