#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>

#define DO_EXIT throw RTI_InternalError(e.what())
#define catching()\
cout<< __FILE__ << ":" << __LINE__ << "->" << e.what() << endl;\
DO_EXIT

/*if you comment the follow #define, 
 * then the test programm will execute reliable on HLA-Synchronization.
 * Otherwise, you need to use keyboard to control the execution.
 */
//#define KEYBOARD_CONTROL

#ifdef KEYBOARD_CONTROL
	extern int inputint;
#endif 




#define FB_TEST_TIME 100
#define FB_TEST_SIZE 1024*64
int updatearray[FB_TEST_SIZE/4];
using namespace std;
class MyFedAmb;
class commonObjectClassA;
typedef commonObjectClassA commonObjectClassB;
class instanceManager;

extern commonObjectClassA InstA;
extern commonObjectClassB InstB;
extern RTI_ObjectInstanceHandle InstAHandle;
extern RTI_ObjectInstanceHandle InstBHandle;
extern RTIambImplementor a;
extern instanceManager InstManager;
extern RTI_ObjectClassHandle		objectClassA;
extern RTI_ObjectClassHandle		objectClassB;
extern RTI_AttributeHandle		attributeA;
extern RTI_AttributeHandle		attributeB;
extern RTI_AttributeHandleSet		classA_AttributeList;
extern RTI_AttributeHandleSet		classB_AttributeList;
extern RTI_AttributeHandleValueMap	AHVM;
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
	virtual void removeObjectInstance
	(RTI_ObjectInstanceHandle const & theObject,
	 RTI_UserSuppliedTag const & theUserSuppliedTag,
	 RTI_OrderType const & sentOrder)
	throw (RTI_ObjectInstanceNotKnown,
		RTI_FederateInternalError)
	{
		printf("\n\nFederate B:\n    Having Called Remove Object Instance with ID=%ld\n\n",theObject.getHandle());
		
	}
	
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
	{
		cout << "reflectAttributeValues is called" << endl;
		/*verify preCondition 1:*/
		if( InstManager.count(theObject))
		{
			/*verify preCondition 2:*/
			if(InstManager.query(InstA) == theObject )
			{
				InstA.changeValue( *static_cast<const int*>( (*theAttributeValues)[attributeA].data() )   );
				InstB.changeValue(InstA.getValue());
				
				updatearray[0] = InstB.getValue();
				
				RTI_AttributeValue someValue(updatearray, FB_TEST_SIZE);
				
				try{
					AHVM.clear();
					AHVM[attributeB] = someValue;
					
				}
				catch(exception &e){
					catching();
				}
				
				try{
					a.updateAttributeValues(InstBHandle, AHVM, RTI_UserSuppliedTag(static_cast<void*>(&updatearray[0]),sizeof(int) ));
				}
				catch(RTI_Exception &e){
					catching();
				}
			}
			
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
				cout<<""<<endl;
				cout<<"discoverObjectInstance"<<endl;
				cout<< "theObject = "<<theObject.getHandle()<< endl;
				/*verify preCondition 1:*/
				if( theObjectClass == a.getObjectClassHandle(L"objectClassA"))
				{
					printf("\npreCondition 1 is legal\n");	
					InstA = *(new commonObjectClassA());
					InstManager.insert(InstA, theObject);
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



#ifdef KEYBOARD_CONTROL
	int inputint;
#endif 


int barrier =1;

commonObjectClassA InstA;
commonObjectClassB InstB;
RTI_ObjectInstanceHandle InstAHandle;
RTI_ObjectInstanceHandle InstBHandle;
RTIambImplementor a;
instanceManager InstManager;
RTI_ObjectClassHandle		objectClassA;
RTI_ObjectClassHandle		objectClassB;
RTI_AttributeHandle		attributeA;
RTI_AttributeHandle		attributeB;
RTI_AttributeHandleSet		classA_AttributeList;
RTI_AttributeHandleSet		classB_AttributeList;
RTI_AttributeHandleValueMap	AHVM;

int main()
{
	/*definition of local variable for main() */
	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;

	/*ToDo Create and Join via RTI*/
	try {
		a.createFederationExecution(fedName, FDDfile);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
	}
	
	try {
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		catching();
	}


	

		
	
	//  P/S(FedB)={ pubOC=objectClassB, subOC=objectClassA  }
	
	/*allocate memory for classhandle,attributehandle w.r.t this Federation, and then to Publish*/
	try {
		objectClassA		= a.getObjectClassHandle(L"objectClassA");
		objectClassB		= a.getObjectClassHandle(L"objectClassB");
		attributeA		= a.getAttributeHandle(objectClassA, L"attributeA");
		attributeB		= a.getAttributeHandle(objectClassB, L"attributeB");
		classA_AttributeList.insert(attributeA);
		classB_AttributeList.insert(attributeB);	
		a.publishObjectClassAttributes(objectClassB, classB_AttributeList  );
	}
	catch (RTI_Exception &e){
		catching();
	}

#ifdef KEYBOARD_CONTROL
	printf("\n\n this is FederateB: Input any INTEGER  to Subscribe!! \n");
	scanf("%d", &inputint);	
#else	
	a.registerFederationSynchronizationPoint(L"end", RTI_UserSuppliedTag("end", 3));
	while (barrier == 1)usleep(100);
	barrier = 1;	
#endif

	
	/*ToDo Subscribe*/	
	try{
		a.subscribeObjectClassAttributes(objectClassA, classA_AttributeList);
	}
	catch (RTI_Exception &e){
		catching();
	}




#ifdef KEYBOARD_CONTROL
	printf("\n\n this is FederateB: Input any INTEGER  to Register!! \n");
	scanf("%d", &inputint);	
#else	
	a.registerFederationSynchronizationPoint(L"end", RTI_UserSuppliedTag("end", 3));
	while (barrier == 1)usleep(100);
	barrier = 1;	
#endif




	/*ToDo Resgieter Object Instance*/
	try{
		InstBHandle = a.registerObjectInstance(objectClassB);
	}
	catch(RTI_Exception &e){
		catching();
	}

	


	InstManager.insert(InstB, InstBHandle);
	
	
	printf("\n\n Input any non-empty string for continue to Resign&Destroy Federation!! \n");
	while(1)sleep(1);
	
	
	

	



while(1)
	sleep(1);

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




