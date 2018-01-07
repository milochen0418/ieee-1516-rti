#include <time.h>
#include <unistd.h>
#include <sys/time.h>

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
 * and after counter is counting from 1 to 1000, the function reflectAttributeValue will 
 * sendInteraction to FederateA
 * In the final, FederateA.LRC.receiveInteraction, will show how efficient of the testing.
 * Federation ending.
 */

/* FederateA's Algorithm
 * 
 * Step 0.1. decaleration and definition of local variable for main() 
 * Step 0.2. decaleration some HLA-used variable(e.g. classhandle, attributehandle, parameterhandle)
 * Step 1. give a value for HLA-used variable. and then to PublishObjectClass(objectClassA)
 * Step 2. Halt to wait some user keyin any integer, and then to continue after keyin over.
 * Step 3. Do SubscribeIneractionClass(interactionClassB)
 * Step 4. Halt to wait some user keyin any integer, and then to continue after keyin over.
 * Step 5. Do InstAHandle = registerObjectInstance(objectClassA, classA_AttributeList)
 * Step 6. Start up to send 1000 time of updateAttributeValue.
 * 
 */

/* FederateA's FedAmbassador
 * receiveInteraction:= (
 *			    ){
 *			Show the result of how efficient of the testing.
 *			  
 * }			    
 *
 */


int counterM;	//this counter will counting from 0 to FB_TEST_TIME

/*the control variable for time computing */
#define FB_TEST_TIME 1000
#define FB_TEST_SIZE 4
#define TESTSIZE FB_TEST_TIME

int counter;
float totaltime;
float avgtime;
float timeuse;

struct timeval tpstart[TESTSIZE];
struct timeval tpend[TESTSIZE];
struct timeval timeStart;
struct timeval timeEnd;
int updatearray[FB_TEST_SIZE/4];
time_t t1, t2;

/*variable that we need to used*/
using namespace std;
class MyFedAmb;

extern RTI_ObjectInstanceHandle InstAHandle;
extern RTIambImplementor a;

extern RTI_ObjectClassHandle		objectClassA;
extern RTI_AttributeHandle		attributeA;
extern RTI_AttributeHandleSet		classA_AttributeList;
extern RTI_AttributeHandleValueMap	AHVM;
	
extern RTI_InteractionClassHandle	interactionClassB;
extern RTI_ParameterHandle		parameterB;
extern RTI_ParameterHandleValueMap	PHVM;
extern RTI_AttributeHandleSet		classB_ParameterList;
extern int barrier;

class MyFedAmb : public FedAmbImplementor
{
	public:

        void receiveInteraction
            (RTI_InteractionClassHandle const & theInteraction,                                                                                                                      RTI_auto_ptr< RTI_ParameterHandleValueMap > theParameterValues,
             RTI_UserSuppliedTag const & theUserSuppliedTag,
             RTI_OrderType const & sentOrder,
             RTI_TransportationType const & theType)
            throw (RTI_InteractionClassNotRecognized,
                    RTI_InteractionParameterNotRecognized,
                    RTI_InteractionClassNotSubscribed,                                                                                                                                      RTI_FederateInternalError){
		    
		cout << "receiveInteraction is called" << endl;
		gettimeofday(&timeEnd,NULL);
		timeuse = 1000000*(timeEnd.tv_sec-timeStart.tv_sec) + (timeEnd.tv_usec-timeStart.tv_usec);
		timeuse/= 1000000;
		printf("\n\ntime = %f\n",timeuse);
		
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


RTI_InteractionClassHandle	interactionClassB;
RTI_ParameterHandle		parameterB;
RTI_ParameterHandleValueMap	PHVM;
RTI_AttributeHandleSet		classB_ParameterList;

int barrier = 1;


int main()
{

	/* decaleration and definition of local variable for main() */
	int inputint;
	int i;
	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;

	/* Create and Join*/
	try {
		a.createFederationExecution(fedName, FDDfile);
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
	}



		

	
	//  P/S(FedA)={ subOC=objectClassB, pubOC=objectClassA  }

	 /* Step 1. give a value for HLA-used variable. and then to PublishObjectClass(objectClassA)*/
	try {
		objectClassA		= a.getObjectClassHandle(L"objectClassA");
		interactionClassB	= a.getInteractionClassHandle(L"interactionClassB");
		attributeA		= a.getAttributeHandle(objectClassA, L"attributeA");
		parameterB		= a.getParameterHandle(interactionClassB, L"parameterB");

		classA_AttributeList.insert(attributeA);
		a.publishObjectClassAttributes(objectClassA, classA_AttributeList);
	}
	catch (RTI_Exception &e){
		catching();
	}


	/* Step 2. Halt to wait some user keyin any integer, and then to continue after keyin over.*/
	/*
	printf("\n\n Input any non-empty string for continue to Subscribe!! INTEGER \n");
	scanf("%d",&inputint);
	*/
	while (barrier == 1)usleep(100);
	barrier=1;

		
	/* Step 3. Do SubscribeIneractionClass(interactionClassB)*/
	try {
		a.subscribeInteractionClass(interactionClassB);
	}
	catch (RTI_Exception &e){
		catching();
	}

	
	/* Step 4. Halt to wait some user keyin any integer, and then to continue after keyin over.*/
	/*
	printf("\n\n Input any non-empty string for continue to Register!! \n");
	scanf("%d",&inputint);
	*/	
	
 	/* Step 5. Do InstAHandle = registerObjectInstance(objectClassA, classA_AttributeList)*/
	try{
		InstAHandle = a.registerObjectInstance(objectClassA);
	}
	catch(RTI_Exception &e){
		catching();
	}
	
	while (barrier == 1)usleep(100);
	barrier=1;


	/* Step 6. Start up to send 1000 time of updateAttributeValue. */
	i=0;
	t1 = time(NULL);
	counter=i;
	gettimeofday(&timeStart,NULL);
	
	AHVM[attributeA] = RTI_AttributeValue(static_cast<void*>(&updatearray[0]),FB_TEST_SIZE);
	for(i=0;i<FB_TEST_TIME;i++){
		try{
			a.updateAttributeValues(InstAHandle, AHVM, RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );
		}
		catch(RTI_Exception &e){
			catching();
		}
		catch(exception &e){
			catching();
		}
	}

		
	
	
  
	printf("\n\n the Federate A is sleeping \n");
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




