#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>

//#define KEYBOARD_CONTROL





#define FB_TEST_TIME 1000
#define FB_TEST_SIZE 1024*1024*4
#define TESTSIZE FB_TEST_TIME
int counter;
float totaltime;
float avgtime;
float timeuse;

struct timeval tpstart[TESTSIZE];
struct timeval tpend[TESTSIZE];
int updatearray[FB_TEST_SIZE/4];
time_t t1, t2;
using namespace std;
class MyFedAmb;
class commonObjectClassA;
typedef commonObjectClassA commonObjectClassB;
typedef commonObjectClassA commonInteractionClassA;
typedef commonObjectClassA commonInteractionClassB;
extern commonInteractionClassA ActA;
extern commonInteractionClassB ActB;
extern int barrier;

extern RTIambImplementor a;
//extern instanceManager InstManager;

extern RTI_InteractionClassHandle	interactionClassA;
extern RTI_InteractionClassHandle	interactionClassB;
extern RTI_ParameterHandle		parameterA;
extern RTI_ParameterHandle		parameterB;
extern RTI_ParameterHandleValueMap	PHVM;
	

class commonObjectClassA{
public:
	commonObjectClassA():_value(0), _myaddress((int)(this)){ changeValue(0);}
	commonObjectClassA(int value):_myaddress((int)(this)){ changeValue(value);}
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
	bool operator==(commonObjectClassA const& rhs) const
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




/* Class instanceManager
 * Because our concpet of object instance is that we can not only new a object with some C++ class in local-view
 * but also we can cause the C++ object to be seen as RTI object instance at global-view.
 * So each Federate need a transform ,that is bijection, to query object from global RTI object instance to local C++ object
 * and to query object from local C++ object to global RTI object instance.
 * So, instanceManager afford a STL_MAP-interface-like interface(e.g. count,insert) to afford. If you use Mathematic view to
 * see instanceManager, It's like to see bijection as pair,of which (local_type,global_type) is type,set.
 * Thus, this class is convinient for you to coding a code of Federate.
 */

class instanceManager{
public:
	typedef commonObjectClassA		local_type;	//
	typedef RTI_ObjectInstanceHandle	global_type;	//	
	typedef map<local_type, global_type>	mapLTG;		//The Map to manage the view of instance from Local To Global.
	typedef map<global_type,local_type>	mapGTL;		//The Map to manage the view of instance from Global to Local.
		
	bool insert( local_type const& localInstance, global_type const& globalInstance )
	{	
		if( manageLTG.count(localInstance)!=0 || manageGTL.count(globalInstance)!=0)
		{
			return false;
		}
		else
		{
			manageLTG[localInstance]=globalInstance;
			manageGTL[globalInstance]=localInstance;
			return true;
		}
	}
	
	int count(local_type const& localInstance)
	{
		return (manageLTG.count(localInstance)!=0 ) ? 1 : 0;	
	}
	
	int count(global_type const& globalInstance)
	{
		return (manageGTL.count(globalInstance)!=0 ) ? 1 : 0;	
	}
	
	int count(global_type const& globalInstance, local_type const& localInstance)
	{
		return count(globalInstance);	
	}
	
	int count(local_type const& localInstance, global_type const& globalInstance)
	{
		return count(globalInstance);	
	}
	
	local_type const& query(global_type const& globalInstance)
	{
		return manageGTL[globalInstance];	
	}

	global_type const& query(local_type const & localInstance)
	{
		return manageLTG[localInstance];	
	}
	
	bool insert( global_type const& globalInstance, local_type const& localInstance )
	{
		return insert(localInstance, globalInstance);
	}
private:	
	mapLTG manageLTG;
	mapGTL manageGTL;
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

                                if(theInteraction == a.getInteractionClassHandle(L"interactionClassB"))
                                {
					gettimeofday(&tpend[counter],NULL);
					ActB.changeValue( *static_cast<const int*>( (*theParameterValues)[parameterB].data())  ); 
					ActA.changeValue(ActB.getValue()+1);
					if(ActA.getValue() < FB_TEST_TIME)
					{
						
						updatearray[0]=ActA.getValue();
						counter=updatearray[0];
						gettimeofday(&tpstart[counter],NULL);
						PHVM.clear();
						PHVM[parameterA] = RTI_ParameterValue(static_cast<void*>(&updatearray[0]),FB_TEST_SIZE);
						a.sendInteraction(interactionClassA,PHVM, RTI_UserSuppliedTag(static_cast<void*>(&updatearray[0]),sizeof(int)));
					}
					else
					{
						int i;
						for(i=0;i<TESTSIZE;i++)
					        {
					                timeuse=1000000*(tpend[i].tv_sec-tpstart[i].tv_sec)+
					                tpend[i].tv_usec-tpstart[i].tv_usec;
					                timeuse/=1000000;
					                totaltime +=timeuse;
					                printf("%d\t%f\n",i,timeuse);
					        }
					        avgtime=totaltime/TESTSIZE;
					        printf("average= %f",avgtime);
 	
							
						t2 = time(NULL);
							cout << "time = " << t2-t1 << endl;
							exit(0);

					}
                                }
                        }

		void synchronizationPointRegistrationSucceeded
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError)
			{
			};
		
		void announceSynchronizationPoint
			(RTI_wstring const & label,
			 RTI_UserSuppliedTag const & theUserSuppliedTag)
			throw (RTI_FederateInternalError)
			{
				if (label == L"end")
					a.synchronizationPointAchieved(label);
			};

		void federationSynchronized
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError)
			{
				barrier = 0;
			};


	
};






RTIambImplementor a;
RTI_InteractionClassHandle	interactionClassA;
RTI_InteractionClassHandle	interactionClassB;
RTI_ParameterHandle		parameterA;
RTI_ParameterHandle		parameterB;
RTI_ParameterHandleValueMap	PHVM;

commonInteractionClassA ActA;
commonInteractionClassB ActB;
int barrier=1;
int main()
{

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
		exit(1);
	}



		

	
	//  P/S(FedA)={ subOC=objectClassB, pubOC=objectClassA  }

	/* give values to RTI-used variable and then to Publish*/
	try {
		interactionClassA	= a.getInteractionClassHandle(L"interactionClassA");
		interactionClassB	= a.getInteractionClassHandle(L"interactionClassB");
		parameterA		= a.getParameterHandle(interactionClassA, L"parameterA");
		parameterB		= a.getParameterHandle(interactionClassB, L"parameterB");
		a.publishInteractionClass(interactionClassA);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}


	/* Barrier */
#ifdef KEYBOARD_CONTROL
	printf("\n\n Input any non-empty string for continue to Subscribe!! INTEGER \n");
	scanf("%d",&inputint);
#else
	
	while (barrier == 1)usleep(100);
	barrier = 1;
#endif

		
	/* Do Subscribe*/	
	try {
		a.subscribeInteractionClass(interactionClassB);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}

	
	/* Barrier */
#ifdef KEYBOARD_CONTROL	
	printf("\n\n Input any non-empty string for continue to send the first interaction!! \n");
	scanf("%d",&inputint);
#else
	
	while (barrier == 1)usleep(100);
	barrier = 1;
#endif	


	
	   
	/* start up to sendInteraction */
	i=0;
	t1 = time(NULL);
	counter=i;
	gettimeofday(&tpstart[counter],NULL);
	PHVM[parameterA]=RTI_ParameterValue(static_cast<void*>(&i), sizeof(int));
	
	try{
		a.sendInteraction(interactionClassA, PHVM, RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );
	}
	catch(RTI_Exception &e){
		cout<< e.what() <<endl;
	}

		
	
	

	
  
printf("\nwait\n");   

	
	


while(1)sleep(1);

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




