#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>

using namespace std;

#define FB_TEST_TIME 1000
#define FB_TEST_SIZE 64*1024
#define TESTSIZE FB_TEST_TIME
int counter;
struct timeval t1, t2;
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
				cout << "receiveInteraction is called, counter = " << counter << endl;

				counter++;
				
				a.sendInteraction(interactionClassA, PHVM, RTI_UserSuppliedTag((void *)&counter, sizeof(int)));

				if (counter == FB_TEST_TIME + 1)
				{
					gettimeofday(&t2, NULL);
					if (t2.tv_usec < t1.tv_usec)
						cout << t2.tv_sec-t1.tv_sec-1 << "." << 1000000+t2.tv_usec-t1.tv_usec << endl;
					else
						cout << t2.tv_sec-t1.tv_sec << "." << t2.tv_usec-t1.tv_usec << endl;
					exit(0);
				}
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
		theID = a.joinFederationExecution(
				fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}

	try {
		interactionClassA = a.getInteractionClassHandle(L"interactionClassA");
		interactionClassB = a.getInteractionClassHandle(L"interactionClassB");
		parameterA = a.getParameterHandle(interactionClassA, L"parameterA");
		parameterB = a.getParameterHandle(interactionClassB, L"parameterB");

		a.publishInteractionClass(interactionClassA);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}

	PHVM[parameterA] = RTI_ParameterValue(new char[FB_TEST_SIZE], FB_TEST_SIZE);

	printf("\n\n Input any non-empty string for continue to Subscribe!! INTEGER \n");
	scanf("%d",&inputint);
		
	try {
		a.subscribeInteractionClass(interactionClassB);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}
	
	printf("\n\n Input any non-empty string for continue to send the first interaction!! \n");
	scanf("%d",&inputint);
	
	gettimeofday(&t1, NULL);
	
	try{
		a.sendInteraction(interactionClassA, PHVM, RTI_UserSuppliedTag((void *)&counter, sizeof(int)));
	}
	catch(RTI_Exception &e){
		cout<< e.what() <<endl;
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

