#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"

using namespace std;

class MyFedAmb : public FedAmbImplementor
{
	public:
		void reflectAttributeValues
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType)
			throw (RTI_ObjectInstanceNotKnown,
					RTI_AttributeNotRecognized,
					RTI_AttributeNotSubscribed,
					RTI_FederateInternalError)
			{
				cout << "reflectAttributeValues is called" << endl;
				cout<<"RTI_ObjectInstanceHandle : "<<theObject.getHandle()<<endl;
			}
		
		void discoverObjectInstance
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_ObjectClassHandle const & theObjectClass,
			 RTI_wstring const & theObjectInstanceName)
			throw (RTI_CouldNotDiscover,
                   RTI_ObjectClassNotKnown,
                   RTI_FederateInternalError)
			{
				cout<<"discoverObjectInstance"<<endl;
			}
		
		void synchronizationPointRegistrationSucceeded
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError)
			{
				cout << "synchronizationPointRegistrationSucceeded is called" << endl;
			}
		
        void receiveInteraction
            (RTI_InteractionClassHandle const & theInteraction,
             RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
             RTI_UserSuppliedTag const & theUserSuppliedTag,
             RTI_OrderType const & sentOrder,
             RTI_TransportationType const & theType)
            throw (RTI_InteractionClassNotRecognized,
                    RTI_InteractionParameterNotRecognized,
                    RTI_InteractionClassNotSubscribed,
                    RTI_FederateInternalError){
				cout << "receiveInteraction is called" << endl;
			}
};

int main()
{
	RTIambImplementor a;

	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;
	try {
		a.createFederationExecution(fedName, FDDfile);
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}
	
	RTI_ObjectClassHandle OChandle(9);
	RTI_ObjectClassHandle OChandle2(2);
	RTI_ObjectClassHandle OChandle3(7);
	RTI_InteractionClassHandle IChandle(34);
	RTI_InteractionClassHandle IChandle2(31);
	RTI_InteractionClassHandle IChandle3(33);

#if 0	
	RTI_AttributeHandleSet AHS;
	AHS.insert(RTI_AttributeHandle(1));
	AHS.insert(RTI_AttributeHandle(2));
	AHS.insert(RTI_AttributeHandle(3));
#endif
	
	RTI_AttributeHandleSet AHS_100_P;
	AHS_100_P.insert(RTI_AttributeHandle(3));
	AHS_100_P.insert(RTI_AttributeHandle(4));
	AHS_100_P.insert(RTI_AttributeHandle(5));

	RTI_AttributeHandleSet AHS_100_S;
	AHS_100_S.insert(RTI_AttributeHandle(2));
	AHS_100_S.insert(RTI_AttributeHandle(3));
	AHS_100_S.insert(RTI_AttributeHandle(4));

	RTI_AttributeHandleSet AHS_101_S;
	AHS_101_S.insert(RTI_AttributeHandle(1));
	AHS_101_S.insert(RTI_AttributeHandle(4));
	AHS_101_S.insert(RTI_AttributeHandle(5));

	

	RTI_ObjectInstanceHandle theObject;
	try {
		if(theID.getHandle()==1){
			a.publishObjectClassAttributes(OChandle, AHS_100_P);
			a.publishInteractionClass(IChandle);
		}
		
		//a.unpublishObjectClass(OChandle);
		//a.unpublishObjectClassAttributes(OChandle,AHS);
		if(theID.getHandle()==1){
			a.subscribeObjectClassAttributes(OChandle3, AHS_100_S);
			a.subscribeInteractionClass(IChandle3);
		}
		
		if(theID.getHandle()==2){
			a.subscribeObjectClassAttributes(OChandle2,AHS_101_S);
			a.subscribeInteractionClass(IChandle2);
		}
			
		//a.unsubscribeObjectClass(OChandle);
		//a.unsubscribeObjectClassAttributes(OChandle,AHS);
		
		//a.publishInteractionClass(IChandle);
		//a.unpublishInteractionClass(IChandle);
		//a.subscribeInteractionClass(IChandle);
		//a.unsubscribeInteractionClass(IChandle);
		if(theID.getHandle()==1)
		{
			int commd;
			printf("enter command: go value update\n");
			scanf("%d",&commd);
		theObject=a.registerObjectInstance(OChandle,L"");
		
		int num1 = 1, num2 = 3;

		
		RTI_AttributeHandle handle1(3), handle2(5);
		RTI_AttributeHandleValueMap theAttributeValues;
		RTI_AttributeValue value1((void *)&num1, sizeof(int)), value2((void *)&num2, sizeof(int));
		RTI_UserSuppliedTag mytag("TAG 1", strlen("TAG 1")+1);
		theAttributeValues.clear();
		
		theAttributeValues.insert(RTI_AttributeHandleValueMap::value_type(handle1, value1));
		theAttributeValues.insert(RTI_AttributeHandleValueMap::value_type(handle2, value2));
		
		a.updateAttributeValues(theObject, theAttributeValues, mytag);




		
		RTI_ParameterHandle phandle1(3), phandle2(5);
		RTI_ParameterHandleValueMap theParameterValues;
		RTI_ParameterValue pvalue1((void *)&num1, sizeof(int)), pvalue2((void *)&num2, sizeof(int));
		RTI_UserSuppliedTag pmytag("TAG 1", strlen("TAG 1")+1);
		theAttributeValues.clear();
		
		theParameterValues.insert(RTI_ParameterHandleValueMap::value_type(phandle1, pvalue1));
		theParameterValues.insert(RTI_ParameterHandleValueMap::value_type(phandle2, pvalue2));
		
		a.sendInteraction(IChandle, theParameterValues, pmytag);



		
		}
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
	}


while(1)
	sleep(1);

	try {
		a.resignFederationExecution(RTI_UNCONDITIONALLY_DIVEST_ATTRIBUTES);
		a.destroyFederationExecution(fedName);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}

	return 0;
}

/*
 * vim: ts=4 sts=4 sw=4
 */
