#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>


//#define KEYBOARD_CONTROL




#define FB_TEST_TIME 1000
#define FB_TEST_SIZE 1024*1024*4
int updatearray[FB_TEST_SIZE/4];
using namespace std;
class MyFedAmb;
class commonObjectClassA;
typedef commonObjectClassA commonObjectClassB;
typedef commonObjectClassA commonInteractionClassA;
typedef commonObjectClassA commonInteractionClassB;

extern commonObjectClassA ActA;
extern commonObjectClassB ActB;
extern RTIambImplementor a;


extern RTI_InteractionClassHandle	interactionClassA;
extern RTI_InteractionClassHandle	interactionClassB;
extern RTI_ParameterHandle		parameterA;
extern RTI_ParameterHandle		parameterB;
extern RTI_ParameterHandleValueMap	PHVM;
extern int barrier;	

class commonObjectClassA{
public:
	commonObjectClassA():_value(0),_myaddress((int)(this)) { changeValue(0);}
	commonObjectClassA(int value):_myaddress((int)(this)){ changeValue(value); }
	~commonObjectClassA(){}
	void changeValue(int const& value)
	{
		_value = value;
		//printf("commonObjectClassA::_value = %d\n",_value); 
	}
	
	int getValue()
	{
		return _value;
	}
	
	commonObjectClassA const& operator=(commonObjectClassA const& theObj)
	{ 
		changeValue(theObj._value);
		return *this;
	}
	
	commonObjectClassA const& operator=(int const& theValue)
	{ 
		changeValue(theValue);
		return *this;
	}
	
	bool operator==(commonObjectClassA const& rhs)
	{
		return _myaddress == rhs._myaddress;
	}
	
	bool operator<(commonObjectClassA const& rhs) const
	{
		return _myaddress < rhs._myaddress;
	}
private:
	int _value;
	int _myaddress;
};



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
                                if(theInteraction == a.getInteractionClassHandle(L"interactionClassA"))
                                {

                                        ActA.changeValue( *static_cast<const int*>( (*theParameterValues)[parameterA].data())  );
                                        ActB.changeValue(ActA.getValue());
					updatearray[0]=ActB.getValue();
					PHVM.clear();
					PHVM[parameterB] = RTI_ParameterValue(static_cast<void*>(&updatearray[0]),FB_TEST_SIZE);
					
					a.sendInteraction(interactionClassB, PHVM, RTI_UserSuppliedTag(static_cast<void*>(&updatearray[0]),sizeof(int)));

				}

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






commonObjectClassA ActA;
commonObjectClassB ActB;
RTIambImplementor a;
RTI_InteractionClassHandle	interactionClassA;
RTI_InteractionClassHandle	interactionClassB;
RTI_ParameterHandle		parameterA;
RTI_ParameterHandle		parameterB;
RTI_ParameterHandleValueMap	PHVM;
int barrier = 1;

int main()
{
#if 1
	int inputint;
	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;

	/*Create and Join*/
	try {
		a.createFederationExecution(fedName, FDDfile);
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}


	

		
	
	//  P/S(FedB)={ pubOC=objectClassB, subOC=objectClassA  }
	
	/*Step 1. give value to RTI-used variable , and then to do  Publish*/
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

	/*Barrier*/
#ifdef KEYBOARD_CONTROL	
	printf("\n\n this is FederateB: Input any non-empty string for continue to Subscribe!!INTEGER \n");
	scanf("%d", &inputint);
#else 	

	a.registerFederationSynchronizationPoint(L"end", RTI_UserSuppliedTag("end", 3));
	while (barrier == 1)usleep(100);
	barrier = 1;

#endif
	/* Do Subscribe*/
	try{
		a.subscribeInteractionClass(interactionClassA);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}

	/* Barrier */
#ifdef KEYBOARD_CONTROL
	printf("\n\n this is FederateB: Input any non-empty string for continue to Subscribe!!INTEGER \n");
	scanf("%d", &inputint);
#else

	a.registerFederationSynchronizationPoint(L"end", RTI_UserSuppliedTag("end", 3));
	while (barrier == 1)usleep(100);
	barrier = 1;
#endif


	
	printf("\n\n wait for continue to Resign&Destroy Federation!! \n");
	
	
	

	



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
#endif
	return 0;
}

/*
 * vim: ts=4 sts=4 sw=4
 */




