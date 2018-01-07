#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>

using namespace std;

#define FB_TEST_TIME 1000
#define FB_TEST_SIZE 64*1024

RTIambImplementor a;
RTI_InteractionClassHandle	interactionClassA;
RTI_InteractionClassHandle	interactionClassB;
RTI_ParameterHandle		parameterA;
RTI_ParameterHandle		parameterB;
RTI_ParameterHandleValueMap	PHVM;

class MyFedAmb : public FedAmbImplementor
{
	public:
		void receiveInteraction
			(RTI_InteractionClassHandle const & theInteraction,
			 RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
			 RTI_UserSuppliedTag const & theUserSuppliedTag,
			 RTI_OrderType const & sentOrder,
			 RTI_TransportationType const & theType)
			throw (RTI_InteractionClassNotRecognized,
					RTI_InteractionParameterNotRecognized,
					RTI_InteractionClassNotSubscribed,
					RTI_FederateInternalError)
			{
				cout << "receiveInteraction is called" << endl;
				
				a.sendInteraction(interactionClassB, PHVM, theUserSuppliedTag);
			}
};

int main()
{
	int inputint;
	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;

	try {
		a.createFederationExecution(fedName, FDDfile);
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, 
				logicalTimeFactory, logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}

	try {
		interactionClassA	= a.getInteractionClassHandle(L"interactionClassA");
		interactionClassB	= a.getInteractionClassHandle(L"interactionClassB");
		parameterA		= a.getParameterHandle(interactionClassA, L"parameterA");
		parameterB		= a.getParameterHandle(interactionClassB, L"parameterB");

		a.publishInteractionClass(interactionClassB);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}

	PHVM[parameterB] = RTI_ParameterValue(new char[FB_TEST_SIZE], FB_TEST_SIZE);

	printf("\n\n this is FederateB: Input any non-empty string for continue to Subscribe!!INTEGER \n");
	scanf("%d", &inputint);

	try{
		a.subscribeInteractionClass(interactionClassA);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
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
