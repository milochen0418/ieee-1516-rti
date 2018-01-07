#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>


#define DO_EXIT throw RTI_InternalError(e.what())
#define catching()\
cout<< __FILE__ << ":" << __LINE__ << "->" << e.what() << endl;\
DO_EXIT


/* The model of the testing programming is NTPU-like for BandWidth test.
 * FederateA send 1000 time updateAttibuteValue to FederateB.
 * After Federate B receiving 1000 time, it send interaction to FederateA.
 * So FederateA will know it's time to end. and then FederateA
 * show how efficient of the testing.
 */

/* Federation Explaination
 * 
 * Static view:
 * We have FederateA and Federate B.
 * 
 * FederateA publish objectClassA, subscribe interactionClassB
 * FederateB publish interactionClassB, subscribe objectClassA
 * FederateA register InstA with handle=InstAHandle  through the class objectClassA
 * attributeA is  instance attribute w.r.t InstA
 * 
 * Behavior view:
 * At first, main() of FederateA do updateAttributeValue for attributeA of InstAHandle,
 * After 1000 time, the main() of FederateA don't do any thing but just wait.
 * main() of FederateB cannot do anything but just waiting.
 * The major running time of FederateB is almost on reflectAttributeValue of FedAmbassador.
 * FederateB.LRC.reflectAttributeValue will wait all of 1000 time message arrive via counter, 
 * and after counter is counting from 1 to 1000, the function reflectAttributeValue will send
 * endInteraction to FederateA
 * In the final, FederateA.LRC.receiveInteraction, will show how efficient of the testing.
 * Federation ending.
 */



/* FederateB's Algorithm
 *
 * Step 0.1. decalaration and definition of local variable for main()
 * Step 0.2. decaleration some HLA-used variable(e.g. classhandle, attributehandle, parameterhandle)
 * Step 1. give a value for HLA-used variable. and then to PublishInteractionClass(interactionClassB)
 * Step 2. Halt to wait some user keyin any integer, and then to continue after keyin over.
 * Step 3. Do SubscribeObjectClass(objectClassA)
 * Step 4. Halt to wait some user keyin any integer, and then to continue after keyin over.
 *                                                                                                                                                                       */

/* FederateB's FedAmbassador
 * reflectAttributeValues:= (){
 * 	counterM++
 * 	if(counter==FB_TEST_TIME) sendInteraction(interactionClassB);
 * }
 *
 * discoverAttributeValues:= (){
 *         InstAHandle = DiscoverObjectInstanceHandle
 * }
 */





#define FB_TEST_TIME 1000
#define FB_TEST_SIZE 4*4
int updatearray[FB_TEST_SIZE/4];
int counterM;
using namespace std;
class MyFedAmb;

extern RTI_ObjectInstanceHandle InstAHandle;
extern RTIambImplementor a;

extern RTI_ObjectClassHandle		objectClassA;
extern RTI_AttributeHandle		attributeA;
extern RTI_AttributeHandleSet		classA_AttributeList;
extern RTI_AttributeHandleValueMap	AHVM;
	
extern RTI_InteractionClassHandle       interactionClassB;
extern RTI_ParameterHandle              parameterB;
extern RTI_ParameterHandleValueMap      PHVM;
extern RTI_AttributeHandleSet           classB_ParameterList;
extern int barrier;


class MyFedAmb : public FedAmbImplementor
{
public:
	void reflectAttributeValues
		(RTI_ObjectInstanceHandle const & theObject,
		 RTI_auto_ptr< RTI_AttributeHandleValueMap > theAttributeValues,
		 RTI_UserSuppliedTag const & theUserSuppliedTag,
		 RTI_OrderType const & sentOrder,
		 RTI_TransportationType const & theType)
		throw (	RTI_ObjectInstanceNotKnown,
			RTI_AttributeNotRecognized,
			RTI_AttributeNotSubscribed,
			RTI_FederateInternalError)
	{	/* After 1000 time message are all arrived, to info FederateA via sendInteraction */
		cout << "reflectAttributeValues is called" << endl;
		//cout << counterM++ << endl;
		counterM++;
		if(counterM > FB_TEST_TIME){
			printf("\n\n\nIt's Impossible Error !!\n\n\n");
		}
		if(counterM==FB_TEST_TIME){
			a.sendInteraction(interactionClassB, PHVM, RTI_UserSuppliedTag(static_cast<void*>(&updatearray[0]),sizeof(int) ));
		}
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
		InstAHandle = theObject;

		
	a.registerFederationSynchronizationPoint(L"end", RTI_UserSuppliedTag("end", 3));

	}
	
	void synchronizationPointRegistrationSucceeded
		(RTI_wstring const & label)
		throw (RTI_FederateInternalError)
		{
		}
		
	void announceSynchronizationPoint
		(RTI_wstring const & label,
		 RTI_UserSuppliedTag const & theUserSuppliedTag)
		throw (RTI_FederateInternalError)
		{
			if (label == L"end")
				a.synchronizationPointAchieved(label);
		}

	void federationSynchronized
		(RTI_wstring const & label)
		throw (RTI_FederateInternalError)
		{
			barrier = 0;
		}
	
		
};






RTI_ObjectInstanceHandle InstAHandle;
RTIambImplementor a;
RTI_ObjectClassHandle		objectClassA;
RTI_AttributeHandle		attributeA;
RTI_AttributeHandleSet		classA_AttributeList;
RTI_AttributeHandleValueMap	AHVM;
	

RTI_InteractionClassHandle      interactionClassB;
RTI_ParameterHandle             parameterB;
RTI_ParameterHandleValueMap     PHVM;
RTI_AttributeHandleSet          classB_ParameterList;
int barrier=1;


int main()
{

	/* Step 0.1. decalaration and definition of local variable for main()*/
	int inputint;
	counterM=0;
	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;

	/* Create and Join */
	try {
		a.createFederationExecution(fedName, FDDfile);
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
	}


	

		
	
	//  P/S(FedB)={ pubOC=objectClassB, subOC=objectClassA  }
	
	/* Step 1. give a value for HLA-used variable. and then to PublishInteractionClass(interactionClassB) */
	try {
		objectClassA		= a.getObjectClassHandle(L"objectClassA");
		interactionClassB	= a.getInteractionClassHandle(L"interactionClassB");
		attributeA		= a.getAttributeHandle(objectClassA, L"attributeA");
		parameterB		= a.getParameterHandle(interactionClassB, L"parameterB");
		classA_AttributeList.insert(attributeA);
		a.publishInteractionClass(interactionClassB);
	}
	catch (RTI_Exception &e){
		catching();
	}

	/* Step 2. Halt to wait some user keyin any integer, and then to continue after keyin over. */
	/*
	printf("\n\n this is FederateB: Input any non-empty string for continue to Subscribe!!INTEGER \n");
	scanf("%d", &inputint);
	*/
		a.registerFederationSynchronizationPoint(L"end", RTI_UserSuppliedTag("end", 3));

	while (barrier == 1)usleep(100);
	barrier =1;
	
	/*Step 3. §@Subscribe*/
	try{
		a.subscribeObjectClassAttributes(objectClassA, classA_AttributeList);
	}
	catch (RTI_Exception &e){
		catching();
	}

	



	printf("\n\n the Federate B is sleeping \n");

	while(1)sleep(1);
	
	
	

	




	try {
		a.resignFederationExecution(RTI_UNCONDITIONALLY_DIVEST_ATTRIBUTES);
		a.destroyFederationExecution(fedName);
	}
	catch (RTI_Exception &e){
		catching();
	}
	return 0;
}

/*
 * vim: ts=4 sts=4 sw=4
 */




