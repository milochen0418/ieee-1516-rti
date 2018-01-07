#ifndef RTIambImplementor_h
#define RTIambImplementor_h

#include <stdint.h>
#include "RTI_RTIambassador.h"
#include "Endecode.h"
#include "ms-funcs.h"
#include "OMT_Parser.h"

#define RTI_auto_ptr	std::auto_ptr

class RTIambImplementor : public  RTI_RTIambassador
{
	private:
		RTI_bool is_initialized;
		RTI_bool is_joined;

		RTI_wstring FederationName;
		RTI_FederateHandle* handle;
		uint32_t globalID;				//the globalID is given by fedexec
	public:
		RTI_FederateAmbassador *fedamb;
		
		// 10.37
	
		RTIambImplementor(): is_initialized(RTI_false), is_joined(RTI_false), FederationName(L""), 
							 handle(NULL), fedamb(NULL){};
		
		~RTIambImplementor() 
			throw (){};

		// 4.2
		void createFederationExecution
			(RTI_wstring const & federationExecutionName,
			 RTI_wstring const & fullPathNameToTheFDDfile)
			throw (RTI_FederationExecutionAlreadyExists,
					RTI_CouldNotOpenFDD,
					RTI_ErrorReadingFDD,
					RTI_RTIinternalError);
		
		// 4.3
		void destroyFederationExecution
			(RTI_wstring const & federationExecutionName)
			throw (RTI_FederatesCurrentlyJoined,
					RTI_FederationExecutionDoesNotExist,
					RTI_RTIinternalError) ;
		
		// 4.4
		RTI_FederateHandle const & joinFederationExecution
			(RTI_wstring const & federateType,
			 RTI_wstring const & federationExecutionName,
			 RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador,
			 RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory,
			 RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory)
			throw (RTI_FederateAlreadyExecutionMember,
					RTI_FederationExecutionDoesNotExist,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError) ;
		
		// 4.5
		void resignFederationExecution
			(RTI_ResignAction resignAction)
			throw (RTI_OwnershipAcquisitionPending,
					RTI_FederateOwnsAttributes,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError) ;


		// 4.6
		virtual void registerFederationSynchronizationPoint
			(RTI_wstring const & label,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		virtual void registerFederationSynchronizationPoint
			(RTI_wstring const & label,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_FederateHandleSet const & syncSet)
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 4.9
		virtual void synchronizationPointAchieved
			(RTI_wstring const & label)
			throw (RTI_SynchronizationPointLabelNotAnnounced,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
#if 0		
		// 4.11
		virtual void requestFederationSave
			(RTI_wstring const & label)
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		virtual void requestFederationSave
			(RTI_wstring const & label,
			 RTI_LogicalTime const & theTime)
			throw (RTI_LogicalTimeAlreadyPassed,
					RTI_InvalidLogicalTime,
					RTI_FederateUnableToUseTime,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 4.13
		virtual void federateSaveBegun ()
			throw (RTI_SaveNotInitiated,
					RTI_FederateNotExecutionMember,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 4.14
		virtual void federateSaveComplete ()
			throw (RTI_FederateHasNotBegunSave,
					RTI_FederateNotExecutionMember,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		virtual void federateSaveNotComplete()
			throw (RTI_FederateHasNotBegunSave,
					RTI_FederateNotExecutionMember,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 4.16
		virtual void queryFederationSaveStatus ()
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveNotInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 4.18
		virtual void requestFederationRestore
			(RTI_wstring const & label)
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 4.22
		virtual void federateRestoreComplete ()
			throw (RTI_RestoreNotRequested,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RTIinternalError);
		
		virtual void federateRestoreNotComplete ()
			throw (RTI_RestoreNotRequested,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RTIinternalError);
		
		// 4.24
		virtual void queryFederationRestoreStatus ()
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreNotInProgress,
					RTI_RTIinternalError);
#endif		
		
        /////////////////////////////////////
        // Declaration Management Services //
        /////////////////////////////////////

		// 5.2
		void publishObjectClassAttributes
			(RTI_ObjectClassHandle const & theClass,
			  RTI_AttributeHandleSet const & attributeList)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 5.3
		void unpublishObjectClass
			(RTI_ObjectClassHandle const & theClass)
			throw (RTI_ObjectClassNotDefined,
					RTI_OwnershipAcquisitionPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		void unpublishObjectClassAttributes
			(RTI_ObjectClassHandle const & theClass,
			  RTI_AttributeHandleSet const & attributeList)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_OwnershipAcquisitionPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 5.4
		void publishInteractionClass
			(RTI_InteractionClassHandle const & theInteraction)
			throw (RTI_InteractionClassNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 5.5
		void unpublishInteractionClass
			(RTI_InteractionClassHandle const & theInteraction)
			throw (RTI_InteractionClassNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 5.6
		void subscribeObjectClassAttributes
			(RTI_ObjectClassHandle const & theClass,
			  RTI_AttributeHandleSet const & attributeList,
			   RTI_bool active = RTI_true)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 5.7
		void unsubscribeObjectClass
			(RTI_ObjectClassHandle const & theClass)
			throw (RTI_ObjectClassNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		void unsubscribeObjectClassAttributes
			(RTI_ObjectClassHandle const & theClass,
			  RTI_AttributeHandleSet const & attributeList)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 5.8
		void subscribeInteractionClass
			(RTI_InteractionClassHandle const & theClass,
			  RTI_bool active = RTI_true)
			throw (RTI_InteractionClassNotDefined,
					RTI_FederateServiceInvocationsAreBeingReportedViaMOM,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 5.9
		void unsubscribeInteractionClass
			(RTI_InteractionClassHandle const & theClass)
			throw (RTI_InteractionClassNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		////////////////////////////////
		// Object Management Services //
		////////////////////////////////
		
		// 6.2
		virtual void reserveObjectInstanceName
			(RTI_wstring const & theObjectInstanceName)
			throw (RTI_IllegalName,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

	
		// 6.4
		virtual RTI_ObjectInstanceHandle const & registerObjectInstance
			(RTI_ObjectClassHandle const & theClass)
			throw (RTI_ObjectClassNotDefined,
					RTI_ObjectClassNotPublished,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		virtual RTI_ObjectInstanceHandle const & registerObjectInstance
			(RTI_ObjectClassHandle const & theClass,
			 RTI_wstring const & theObjectInstanceName)
			throw (RTI_ObjectClassNotDefined,
					RTI_ObjectClassNotPublished,
					RTI_ObjectInstanceNameNotReserved,
					RTI_ObjectInstanceNameInUse,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 6.6
		virtual void updateAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleValueMap const & theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		virtual RTI_auto_ptr< RTI_MessageRetractionHandle > updateAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleValueMap const & theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_LogicalTime const & theTime)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_InvalidLogicalTime,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 6.8
		virtual void sendInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_ParameterHandleValueMap const & theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_InteractionClassNotPublished,
					RTI_InteractionClassNotDefined,
					RTI_InteractionParameterNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		virtual RTI_auto_ptr< RTI_MessageRetractionHandle > sendInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_ParameterHandleValueMap const & theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_LogicalTime const & theTime)
			throw (RTI_InteractionClassNotPublished,
					RTI_InteractionClassNotDefined,
					RTI_InteractionParameterNotDefined,
					RTI_InvalidLogicalTime,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 6.10
		virtual void deleteObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_DeletePrivilegeNotHeld,
					RTI_ObjectInstanceNotKnown,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		virtual RTI_auto_ptr< RTI_MessageRetractionHandle > deleteObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_LogicalTime const & theTime)
			throw (RTI_DeletePrivilegeNotHeld,
					RTI_ObjectInstanceNotKnown,
					RTI_InvalidLogicalTime,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
#if 0
		// 6.12
		virtual void localDeleteObjectInstance
			(RTI_ObjectInstanceHandle const & theObject)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_FederateOwnsAttributes,
					RTI_OwnershipAcquisitionPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
#endif		
		// 6.13
		virtual void changeAttributeTransportationType
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes,
			 RTI_TransportationType const & theType)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
	
		// 6.14
		virtual void changeInteractionTransportationType
			(RTI_InteractionClassHandle const & theClass,
			 RTI_TransportationType const & theType)
			throw (RTI_InteractionClassNotDefined,
					RTI_InteractionClassNotPublished,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 6.17
		virtual void requestAttributeValueUpdate
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		virtual void requestAttributeValueUpdate
			(RTI_ObjectClassHandle const & theClass,
			 RTI_AttributeHandleSet const & theAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

#if 0
		///////////////////////////////////
		// Ownership Management Services //
		///////////////////////////////////

		// 7.2
		virtual void unconditionalAttributeOwnershipDivestiture
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.3
		virtual void negotiatedAttributeOwnershipDivestiture
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_AttributeAlreadyBeingDivested,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.6
		virtual void confirmDivestiture
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleSet >securedAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_AttributeDivestitureWasNotRequested,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.8
		virtual void attributeOwnershipAcquisition
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & desiredAttributes,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_ObjectClassNotPublished,
					RTI_AttributeNotDefined,
					RTI_AttributeNotPublished,
					RTI_FederateOwnsAttributes,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.9
		virtual void attributeOwnershipAcquisitionIfAvailable
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & desiredAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_ObjectClassNotPublished,
					RTI_AttributeNotDefined,
					RTI_AttributeNotPublished,
					RTI_FederateOwnsAttributes,
					RTI_AttributeAlreadyBeingAcquired,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		// 7.12
		virtual RTI_auto_ptr< RTI_AttributeHandleSet >
			attributeOwnershipDivestitureIfWanted
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.13
		virtual void cancelNegotiatedAttributeOwnershipDivestiture
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_AttributeDivestitureWasNotRequested,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.14
		virtual void cancelAttributeOwnershipAcquisition
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeAlreadyOwned,
					RTI_AttributeAcquisitionWasNotRequested,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.16
		virtual void queryAttributeOwnership
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandle const & theAttribute)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 7.18
		virtual RTI_bool isAttributeOwnedByFederate
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandle const & theAttribute)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		//////////////////////////////
		// Time Management Services //
		//////////////////////////////
		
		// 8.2
		virtual void enableTimeRegulation
			(RTI_LogicalTimeInterval const & theLookahead)
			throw (RTI_TimeRegulationAlreadyEnabled,
					RTI_InvalidLookahead,
					RTI_InTimeAdvancingState,
					RTI_RequestForTimeRegulationPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.4
		virtual void disableTimeRegulation ()
			throw (RTI_TimeRegulationIsNotEnabled,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.5
		virtual void enableTimeConstrained ()
			throw (RTI_TimeConstrainedAlreadyEnabled,
					RTI_InTimeAdvancingState,
					RTI_RequestForTimeConstrainedPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.7
		virtual void disableTimeConstrained ()
			throw (RTI_TimeConstrainedIsNotEnabled,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.8
		virtual void timeAdvanceRequest
			(RTI_LogicalTime const & theTime)
			throw (RTI_InvalidLogicalTime,
					RTI_LogicalTimeAlreadyPassed,
					RTI_InTimeAdvancingState,
					RTI_RequestForTimeRegulationPending,
					RTI_RequestForTimeConstrainedPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.9
		virtual void timeAdvanceRequestAvailable
			(RTI_LogicalTime const & theTime)
			throw (RTI_InvalidLogicalTime,
					RTI_LogicalTimeAlreadyPassed,
					RTI_InTimeAdvancingState,
					RTI_RequestForTimeRegulationPending,
					RTI_RequestForTimeConstrainedPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.10
		virtual void nextMessageRequest
			(RTI_LogicalTime const & theTime)
			throw (RTI_InvalidLogicalTime,
					RTI_LogicalTimeAlreadyPassed,
					RTI_InTimeAdvancingState,
					RTI_RequestForTimeRegulationPending,
					RTI_RequestForTimeConstrainedPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.11
		virtual void nextMessageRequestAvailable
			(RTI_LogicalTime const & theTime)
			throw (RTI_InvalidLogicalTime,
					RTI_LogicalTimeAlreadyPassed,
					RTI_InTimeAdvancingState,
					RTI_RequestForTimeRegulationPending,
					RTI_RequestForTimeConstrainedPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 8.12
		virtual void flushQueueRequest
			(RTI_LogicalTime const & theTime)
			throw (RTI_InvalidLogicalTime,
					RTI_LogicalTimeAlreadyPassed,
					RTI_InTimeAdvancingState,
					RTI_RequestForTimeRegulationPending,
					RTI_RequestForTimeConstrainedPending,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.14
		virtual void enableAsynchronousDelivery ()
			throw (RTI_AsynchronousDeliveryAlreadyEnabled,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.15
		virtual void disableAsynchronousDelivery ()
			throw (RTI_AsynchronousDeliveryAlreadyDisabled,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.16
		virtual RTI_auto_ptr< RTI_LogicalTime > queryGALT ()
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.17
		virtual RTI_LogicalTime const & queryLogicalTime ()
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.18
		virtual RTI_auto_ptr< RTI_LogicalTime > const & queryLITS ()
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.19
		virtual void modifyLookahead
			(RTI_LogicalTimeInterval const & theLookahead)
			throw (RTI_TimeRegulationIsNotEnabled,
					RTI_InvalidLookahead,
					RTI_InTimeAdvancingState,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.20
		virtual RTI_LogicalTimeInterval const & queryLookahead ()
			throw (RTI_TimeRegulationIsNotEnabled,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.21
		virtual void retract
			(RTI_MessageRetractionHandle const & theHandle)
			throw (RTI_InvalidRetractionHandle,
					RTI_TimeRegulationIsNotEnabled,
					RTI_MessageCanNoLongerBeRetracted,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.23
		virtual void changeAttributeOrderType
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSet const & theAttributes,
			 RTI_OrderType const & theType)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_AttributeNotOwned,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 8.24
		virtual void changeInteractionOrderType
			(RTI_InteractionClassHandle const & theClass,
			 RTI_OrderType const & theType)
			throw (RTI_InteractionClassNotDefined,
					RTI_InteractionClassNotPublished,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		//////////////////////////////////
		// Data Distribution Management //
		//////////////////////////////////
		
		// 9.2
		virtual RTI_auto_ptr< RTI_RegionHandle > createRegion
			(RTI_DimensionHandleSet const & theDimensions)
			throw (RTI_InvalidDimensionHandle,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.3
		virtual void commitRegionModifications
			(RTI_RegionHandleSet const & theRegionHandleSet)
			throw (RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.4
		virtual void deleteRegion
			(RTI_auto_ptr< RTI_RegionHandle > theRegion)
			throw (RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_RegionInUseForUpdateOrSubscription,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.5
		virtual RTI_ObjectInstanceHandle const & registerObjectInstanceWithRegions
			(RTI_ObjectClassHandle const & theClass,
			 RTI_AttributeHandleSetRegionHandleSetPairVector const &
			 theAttributeHandleSetRegionHandleSetPairVector)
			throw (RTI_ObjectClassNotDefined,
					RTI_ObjectClassNotPublished,
					RTI_AttributeNotDefined,
					RTI_AttributeNotPublished,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		virtual RTI_ObjectInstanceHandle const & registerObjectInstanceWithRegions
			(RTI_ObjectClassHandle const & theClass,
			 RTI_AttributeHandleSetRegionHandleSetPairVector const &
			 theAttributeHandleSetRegionHandleSetPairVector,
			 RTI_wstring const & theObjectInstanceName)
			throw (RTI_ObjectClassNotDefined,
					RTI_ObjectClassNotPublished,
					RTI_AttributeNotDefined,
					RTI_AttributeNotPublished,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_ObjectInstanceNameNotReserved,
					RTI_ObjectInstanceNameInUse,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.6
		virtual void associateRegionsForUpdates
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSetRegionHandleSetPairVector const &
			 theAttributeHandleSetRegionHandleSetPairVector)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.7
		virtual void unassociateRegionsForUpdates
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_AttributeHandleSetRegionHandleSetPairVector const &
			 theAttributeHandleSetRegionHandleSetPairVector)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.8
		virtual void subscribeObjectClassAttributesWithRegions
			(RTI_ObjectClassHandle const & theClass,
			 RTI_AttributeHandleSetRegionHandleSetPairVector const &
			 theAttributeHandleSetRegionHandleSetPairVector,
			 RTI_bool active = RTI_true)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.9
		virtual void unsubscribeObjectClassAttributesWithRegions
			(RTI_ObjectClassHandle const & theClass,
			 RTI_AttributeHandleSetRegionHandleSetPairVector const
			 & theAttributeHandleSetRegionHandleSetPairVector)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 9.10
		virtual void subscribeInteractionClassWithRegions
			(RTI_InteractionClassHandle const & theClass,
			 RTI_RegionHandleSet const & theRegionHandleSet,
			 RTI_bool active = RTI_true)
			throw (RTI_InteractionClassNotDefined,RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_FederateServiceInvocationsAreBeingReportedViaMOM,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		// 9.11
		virtual void unsubscribeInteractionClassWithRegions
			(RTI_InteractionClassHandle const & theClass,
			 RTI_RegionHandleSet const & theRegionHandleSet)
			throw (RTI_InteractionClassNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.12
		virtual void sendInteractionWithRegions
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_ParameterHandleValueMap const & theParameterValues,
			 RTI_RegionHandleSet const & theRegionHandleSet,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_InteractionClassNotDefined,
					RTI_InteractionClassNotPublished,
					RTI_InteractionParameterNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

		virtual RTI_auto_ptr< RTI_MessageRetractionHandle > sendInteractionWithRegions
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_ParameterHandleValueMap const & theParameterValues,
			 RTI_RegionHandleSet const & theRegionHandleSet,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_LogicalTime const & theTime)
			throw (RTI_InteractionClassNotDefined,
					RTI_InteractionClassNotPublished,
					RTI_InteractionParameterNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_InvalidLogicalTime,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 9.13
		virtual void requestAttributeValueUpdateWithRegions
			(RTI_ObjectClassHandle const & theClass,
			 RTI_AttributeHandleSetRegionHandleSetPairVector const & theSet,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_ObjectClassNotDefined,
					RTI_AttributeNotDefined,
					RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_InvalidRegionContext,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
#endif
		//////////////////////////
		// RTI Support Services //
		//////////////////////////
		
		// 10.2
		virtual RTI_ObjectClassHandle const & getObjectClassHandle
			(RTI_wstring const & theName)
			throw (RTI_NameNotFound,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.3
		virtual RTI_wstring const & getObjectClassName
			(RTI_ObjectClassHandle const & theHandle)
			throw (RTI_InvalidObjectClassHandle,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.4
		virtual RTI_AttributeHandle const & getAttributeHandle
			(RTI_ObjectClassHandle const & whichClass,
			 RTI_wstring const & theAttributeName)
			throw (RTI_InvalidObjectClassHandle,
					RTI_NameNotFound,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.5
		virtual RTI_wstring const & getAttributeName
			(RTI_ObjectClassHandle const & whichClass,
			 RTI_AttributeHandle const & theHandle)
			throw (RTI_InvalidObjectClassHandle,
					RTI_InvalidAttributeHandle,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.6
		virtual RTI_InteractionClassHandle const & getInteractionClassHandle
			(RTI_wstring const & theName)
			throw (RTI_NameNotFound,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.7
		virtual RTI_wstring const & getInteractionClassName
			(RTI_InteractionClassHandle const & theHandle)
			throw (RTI_InvalidInteractionClassHandle,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.8
		virtual RTI_ParameterHandle const & getParameterHandle
			(RTI_InteractionClassHandle const & whichClass,
			 RTI_wstring const & theName)
			throw (RTI_InvalidInteractionClassHandle,
					RTI_NameNotFound,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.9
		virtual RTI_wstring const & getParameterName
			(RTI_InteractionClassHandle const & whichClass,
			 RTI_ParameterHandle const & theHandle)
			throw (RTI_InvalidInteractionClassHandle,
					RTI_InvalidParameterHandle,
					RTI_InteractionParameterNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.10
		virtual RTI_ObjectInstanceHandle const & getObjectInstanceHandle
			(RTI_wstring const & theName)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.11
		virtual RTI_wstring const & getObjectInstanceName
			(RTI_ObjectInstanceHandle const & theHandle)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
	
#if 0
		// 10.12
		virtual RTI_DimensionHandle const & getDimensionHandle
			(RTI_wstring const & theName)
			throw (RTI_NameNotFound,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.13
		virtual RTI_wstring const & getDimensionName
			(RTI_DimensionHandle const & theHandle)
			throw (RTI_InvalidDimensionHandle,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.14
		virtual unsigned long getDimensionUpperBound
			(RTI_DimensionHandle const & theHandle)
			throw (RTI_InvalidDimensionHandle,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.15
		virtual RTI_DimensionHandleSet const & getAvailableDimensionsForClassAttribute
			(RTI_ObjectClassHandle const & theClass,
			 RTI_AttributeHandle const & theHandle)
			throw (RTI_InvalidObjectClassHandle,
					RTI_InvalidAttributeHandle,
					RTI_AttributeNotDefined,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.16
		virtual RTI_ObjectClassHandle const & getKnownObjectClassHandle
			(RTI_ObjectInstanceHandle const & theObject)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.17
		virtual RTI_DimensionHandleSet const & getAvailableDimensionsForInteractionClass
			(RTI_InteractionClassHandle const & theClass)
			throw (RTI_InvalidInteractionClassHandle,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.18
		virtual RTI_TransportationType const & getTransportationType
			(RTI_wstring const & transportationName)
			throw (RTI_InvalidTransportationName,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.19
		virtual RTI_wstring const & getTransportationName
			(RTI_TransportationType const & transportationType)
			throw (RTI_InvalidTransportationType,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.20
		virtual RTI_OrderType const & getOrderType
			(RTI_wstring const & orderName)
			throw (RTI_InvalidOrderName,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.21
		virtual RTI_wstring const & getOrderName
			(RTI_OrderType const & orderType)
			throw (RTI_InvalidOrderType,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.22
		virtual void enableObjectClassRelevanceAdvisorySwitch ()
			throw (RTI_ObjectClassRelevanceAdvisorySwitchIsOn,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.23
		virtual void disableObjectClassRelevanceAdvisorySwitch ()
			throw (RTI_ObjectClassRelevanceAdvisorySwitchIsOff,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.24
		virtual void enableAttributeRelevanceAdvisorySwitch ()
			throw (RTI_AttributeRelevanceAdvisorySwitchIsOn,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.25
		virtual void disableAttributeRelevanceAdvisorySwitch ()
			throw (RTI_AttributeRelevanceAdvisorySwitchIsOff,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.26
		virtual void enableAttributeScopeAdvisorySwitch ()
			throw (RTI_AttributeScopeAdvisorySwitchIsOn,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.27
		virtual void disableAttributeScopeAdvisorySwitch ()
			throw (RTI_AttributeScopeAdvisorySwitchIsOff,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.28
		virtual void enableInteractionRelevanceAdvisorySwitch ()
			throw (RTI_InteractionRelevanceAdvisorySwitchIsOn,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.29
		virtual void disableInteractionRelevanceAdvisorySwitch ()
			throw (RTI_InteractionRelevanceAdvisorySwitchIsOff,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.30
		virtual RTI_DimensionHandleSet const & getDimensionHandleSet
			(RTI_RegionHandle const & theRegionHandle)
			throw (RTI_InvalidRegion,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.31
		virtual RTI_RangeBounds const & getRangeBounds
			(RTI_RegionHandle const & theRegionHandle,
			 RTI_DimensionHandle const & theDimensionHandle)
			throw (RTI_InvalidRegion,
					RTI_RegionDoesNotContainSpecifiedDimension,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.32
		virtual void setRangeBounds
			(RTI_RegionHandle const & theRegionHandle,
			 RTI_DimensionHandle const & theDimensionHandle,
			 RTI_RangeBounds const & theRangeBounds)
			throw (RTI_InvalidRegion,
					RTI_RegionNotCreatedByThisFederate,
					RTI_RegionDoesNotContainSpecifiedDimension,
					RTI_InvalidRangeBounds,
					RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.33
		virtual unsigned long normalizeFederateHandle
			(RTI_FederateHandle const & theFederateHandle)
			throw (RTI_FederateHandle,
					RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.34
		virtual unsigned long normalizeServiceGroup
			(RTI_ServiceGroupIndicator const & theServiceGroup)
			throw (RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.37
		virtual RTI_bool evokeCallback(double approximateMinimumTimeInSeconds)
			throw (RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.38
		virtual RTI_bool evokeMultipleCallbacks(double approximateMinimumTimeInSeconds,
				double approximateMaximumTimeInSeconds)
			throw (RTI_FederateNotExecutionMember,
					RTI_RTIinternalError);
		
		// 10.39
		virtual void enableCallbacks ()
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);
		
		// 10.40
		virtual void disableCallbacks ()
			throw (RTI_FederateNotExecutionMember,
					RTI_SaveInProgress,
					RTI_RestoreInProgress,
					RTI_RTIinternalError);

#endif
		
};

#endif 

/*
 * vim: ts=4 sts=4 sw=4
 */
