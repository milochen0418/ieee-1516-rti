/* Revisions
 *	1. Changed to build with DLC FDK RTI 
 *		Author: Jagrut Dave
 *		Date: 11/22/03
 *
 *
 */
/*
#define RTI_USES_STD_FSTREAM
#define DRTI
#define FEDFILE "fedfile3"

#include <iostream>
#include <string>
#include "DRTI13.h"
#include "fedtime.hh"
#include "NullFederateAmbassador13.h"
#include "arch.h"

using namespace std;
using namespace rti13;

char *dummy;

#define STOPPER cout << "DRTI-Latency at line " \
                 << __LINE__ << ", hit return" <<\
		 endl ; cin >> dummy ;
*/

//#define IS_DEBUG
#ifdef IS_DEBUG
	int checkpoint=0;
#	define CHK printf("%d line = %d\n",checkpoint++,__LINE__);
#else
#	define CHK
#endif

#include <iostream>
#include <string>
#include <stdlib.h>
#include "FDK_arch.h"
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map> 
#define DO_EXIT throw RTI_InternalError(e.what())
#define catching()\
cout<< __FILE__ << ":" << __LINE__ << "->" << e.what() << endl;\
DO_EXIT
#define FIRST_FEDERATE_HANDLE 1
#define FIRST_ATTRIBUTE_HANDLE 2
#define BARRIER1516()\
	if(userdefined_ID==1)RTIamb->registerFederationSynchronizationPoint(L"end", RTI_UserSuppliedTag("end", 3));\
	while (barrier == 1)usleep(100);\
	barrier = 1

/*
	printf("This is Barrier(): press INTEGER to continue...\n");\
	scanf("%d", &barrierinput);\
	usleep(2)
*/
#define TICK1516()\
	usleep(1000)
#define FEDFILE L"sample.xml"
int barrierinput;
int barrier=1;
int userdefined_ID;


enum TickStyle  {
    TICK_NODELAY    = 0,
    TICK_CONSTDELAY = 1,
    TICK_RANDDELAY  = 2
};


struct MessageS  {
    int sender;             // message sender
    float federate_delay;   // time b/t recv'ing and sending a message
};


// config
TickStyle __tick_style = TICK_NODELAY;
const long __delay_loop_size = 1000;
long __message_size = 128;
long __total_cycles = 100;
const float __range = 0.003;          // range begins at 0
const long __num_bins = 100;


// control
/* long __nodeid; */
RTI_FederateHandle __nodeid;

long __numnodes;

char *__buffer;

/*
long __class_handle;
long __attr_handle;
long __my_obj;
*/
RTI_ObjectClassHandle 		__class_handle;
RTI_AttributeHandle		__attr_handle;
RTI_ObjectInstanceHandle	__my_obj;

/* rti13::AttributeHandleValuePairSet* __my_ahvps; */
RTI_AttributeHandleValueMap AHVM;
RTI_AttributeHandleValueMap* __my_ahvps;

long __msg_recvd = 0;
long __curr_cycle = 0;

bool __done = false;

float __bins[__num_bins];

TIMER_TYPE __time_msg_recvd;
TIMER_TYPE* __times;
float *__delays;
#define TIMES(_cycle_, _node_)  __times[_node_ + (_cycle_ * __numnodes)]
#define DELAYS(_cycle_, _node_) __delays[_node_ + (_cycle_ * __numnodes)]

#define RTT(_cycle_, _node_)                                                   \
    TIMER_DIFF(TIMES(_cycle_,_node_), TIMES(_cycle_,0)) - DELAYS(_cycle_,_node_)


/* rti13::RTIambassador* RTIamb; */
RTIambImplementor* RTIamb;



// Define the federate ambassador
/* class MyFedAmb : public rti13::NullFederateAmbassador {*/
class MyFedAmb : public FedAmbImplementor {
public :
/*  
    void reflectAttributeValues(
        rti13::ObjectHandle                       theObject,
        const rti13::AttributeHandleValuePairSet& theAttributes,
        const char*                             )
      throw ()
*/
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
		TIMER_NOW(__time_msg_recvd);
		
		/* if (__nodeid == 0)  { */
		if(__nodeid == RTI_FederateHandle(FIRST_FEDERATE_HANDLE)){
			MessageS* msg;
			/* unsigned long attr_size; */
			
			/* msg = (MessageS *)theAttributes.getValuePointer(0, attr_size); */
			MessageS* __msg =(MessageS* )( (*theAttributeValues)[RTI_AttributeHandle(FIRST_ATTRIBUTE_HANDLE)].data() );
			msg = new MessageS();
			memcpy(msg, __msg, sizeof(MessageS));
		
			
			const int index = msg->sender + (__curr_cycle * __numnodes);
			
			__times[index] = __time_msg_recvd;
			__delays[index] = msg->federate_delay;
		}
		
		__msg_recvd++;
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
			RTIamb->synchronizationPointAchieved(label);
	}

void federationSynchronized
	(RTI_wstring const & label)
	throw (RTI_FederateInternalError)
	{
		barrier = 0;
	}

};


void BuildAHVPS()  {
	MessageS* msg = (MessageS *)__buffer;
CHK
	
	/* msg->sender = __nodeid; */
	msg->sender = static_cast<int>(__nodeid.getHandle()) - FIRST_FEDERATE_HANDLE;
CHK
	
	/*
	__my_ahvps = rti13::AttributeSetFactory::create(1);
	__my_ahvps->add(__attr_handle, (char *)msg, __message_size);
	*/
	__my_ahvps = &AHVM;
CHK
	__my_ahvps->clear();	
CHK
	//(*__my_ahvps)[__attr_handle]=RTI_AttributeValue(static_cast<void*>(msg), sizeof(MessageS));
	(*__my_ahvps)[__attr_handle]=RTI_AttributeValue(static_cast<void*>(msg), __message_size);
CHK

}


void SendMessage()  {
	/* unsigned long attr_size; */
	/* MessageS* msg = (MessageS *)__my_ahvps->getValuePointer(0, attr_size); */
	MessageS* __msg =(MessageS* )( (*__my_ahvps)[RTI_AttributeHandle(FIRST_ATTRIBUTE_HANDLE)].data() );
	MessageS* msg = new MessageS();
	memcpy(msg, __msg, sizeof(MessageS));
		

	/* if (__nodeid == 0) */
	if(__nodeid == RTI_FederateHandle(FIRST_FEDERATE_HANDLE))
		TIMER_NOW(__times[__curr_cycle * __numnodes]);
	else  {
		TIMER_TYPE now;
		TIMER_NOW(now);
		msg->federate_delay = TIMER_DIFF(now, __time_msg_recvd);
	}
	
	/* RTIamb->updateAttributeValues(__my_obj, *__my_ahvps, NULL); */
	try{
		int qq;
		RTIamb->updateAttributeValues(__my_obj, *__my_ahvps,  RTI_UserSuppliedTag(static_cast<void*>(&qq),sizeof(int))  );	
	}
	catch(RTI_Exception &e){
		catching();
	}
}


void PublishAndSubscribe()  {
	
	/* rti13::AttributeHandleSet* pAHS; */
	RTI_AttributeHandleSet AHS;
CHK
	RTI_AttributeHandleSet *pAHS=&AHS;
CHK
	
	/*
	__class_handle = RTIamb->getObjectClassHandle("Obj");
	__attr_handle = RTIamb->getAttributeHandle("buffer", __class_handle);
	*/
	__class_handle = RTIamb->getObjectClassHandle(L"Obj");
CHK
	__attr_handle = RTIamb->getAttributeHandle(__class_handle, L"buffer");
CHK
	
	/*
	pAHS = rti13::AttributeHandleSetFactory::create(1);
	pAHS->empty();
	pAHS->add(__attr_handle);
	*/
	pAHS->clear();
CHK
	pAHS->insert(__attr_handle);
CHK
	
	/*
	RTIamb->subscribeObjectClassAttributes(__class_handle, *pAHS);
	RTIamb->publishObjectClass(__class_handle, *pAHS);
	*/
	try{
		RTIamb->publishObjectClassAttributes(__class_handle, *pAHS);
CHK
		printf("Waiting Subscribe(): press INTEGER to continue...\n");
CHK
		scanf("%d",&barrierinput);
CHK
		RTIamb->subscribeObjectClassAttributes(__class_handle, *pAHS);
CHK
		printf("Waiting StartRun(): press INTEGER to continue...\n");
CHK
		scanf("%d",&barrierinput);
CHK

	}
	catch(RTI_Exception &e){
		catching();
	}
	
	
	
	/* RTIamb->barrier(); */
CHK
	BARRIER1516();
CHK
	
	/* __my_obj = RTIamb->registerObjectInstance(__class_handle); */
CHK
	__my_obj = RTIamb->registerObjectInstance(__class_handle);
CHK
	printf("Waiting updateAttributeValues(): press INTEGER to continue...\n");
CHK
	//scanf("%d",&barrierinput);
CHK

	/*
	RTIamb->barrier();
	RTIamb->tick();
	RTIamb->barrier();
	*/
	BARRIER1516();
CHK
	TICK1516();
CHK
	BARRIER1516();	
CHK
}



int main(int argc, char** argv)  {
	printf("\nThe Federate that User-Defined ID==1 have the Power to do registerSynPoint\n");
	printf("input your User-Defined ID:");
	scanf("%d", &userdefined_ID);
CHK	
	MyFedAmb FEDamb;
	static int work[__delay_loop_size];
	bool sendMsg = false;
	long i;
	
CHK	
	/*
	RTIamb = new rti13::RTIambassador(); 
	try  {
		
		RTIamb->createFederationExecution(FEDFILE, FEDFILE);
	}
	catch (rti13::Exception& e)  {
		cout << "Got Create Exception: " << &e << endl;
	}
	try  {
		__nodeid = RTIamb->joinFederationExecution(FEDFILE,FEDFILE,
				&FEDamb);
		
		
	}
	catch (rti13::Exception& e)  {
		cout << "Got Join Exception: " << &e << endl;
	}
	*/	
	RTIamb = new RTIambImplementor();
        RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
        RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
        RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;
        RTI_wstring fedName(L"MyFederation");
        RTI_wstring FDDfile(L"sample.xml");
        RTI_wstring fedType(L"go");
CHK	
	try {
                RTIamb->createFederationExecution(fedName, FDDfile);
CHK	
        }
        catch (RTI_Exception &e){
		cout << "Got Create Exception: " << e.what() << endl;
CHK	
        }
	try{
                
		RTIamb->joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
		__nodeid = RTI_FederateHandle((long)userdefined_ID);
CHK	
	}
	catch(RTI_Exception &e){	
		cout << "Got Join Exception: " << &e << endl;
CHK	
	}

	
	
	try {
		
		/* __numnodes = RTIamb->getNumNodes(); */
		__numnodes = 2;
		
		__buffer = new char[__message_size];
		
		cout << "Publish and Subscribe " << endl;
CHK	
		PublishAndSubscribe();
CHK	
		BuildAHVPS();
CHK	
		
		/* if (__nodeid == 0)  { */
		if(__nodeid == RTI_FederateHandle(FIRST_FEDERATE_HANDLE)) {
			__times = new TIMER_TYPE[__numnodes * __total_cycles];
CHK	
			__delays = new float[__numnodes * __total_cycles];
CHK	
			SendMessage();
CHK	
		}
		
		while (!__done)  {
CHK	
			/* RTIamb->tick(); */
			TICK1516();
CHK	
			
			/* if (__nodeid == 0)  { */
			if (__nodeid == RTI_FederateHandle(FIRST_FEDERATE_HANDLE)) 
			{
CHK	
				if (__msg_recvd == (__numnodes - 1))  
				{
CHK	
					__msg_recvd = 0;
CHK	
					
					if (++__curr_cycle == __total_cycles)  
					{
CHK	
						__done = true;
CHK	
						continue;
					}
					else
					{
CHK	
						sendMsg = true;
CHK	
					}
CHK	
				}
CHK	
			}
			else  
			{
CHK	
				if (__msg_recvd == 1)
				{
CHK	
					sendMsg = true;
				}
				
				if (__msg_recvd == (__numnodes - 1))  
				{
CHK	
					__msg_recvd = 0;
					
					if (++__curr_cycle == __total_cycles)
					{
CHK	
						__done = true;
					}
				}
			}
			
			switch(__tick_style)  
			{
				case TICK_NODELAY:
CHK	
					break;
					
				case TICK_CONSTDELAY:
CHK	
					for (i = 0; i < __delay_loop_size; i++)
					{
CHK	
						work[rand() % __delay_loop_size] = i;
					}
					break;
					
				case TICK_RANDDELAY:
CHK	
					const int upper_bound = rand() % __delay_loop_size;
					for (i = 0; i <= upper_bound; i++)
					{
CHK	
						work[rand() % __delay_loop_size] = i;
					}
					break;
			}
			
			if (sendMsg)  {
CHK	
				SendMessage();
CHK	
				sendMsg = false;
CHK	
			}
		}
		
		/* RTIamb->barrier(); // Wait for all to complete */
		BARRIER1516();
CHK	
		
		
		/* if (__nodeid == 0)  { */
		if (__nodeid == RTI_FederateHandle(FIRST_FEDERATE_HANDLE)) {
CHK	
			//memset(__bins, 0, sizeof(float)*__num_bins);
			
			memset(__bins, 0, 100);
CHK	
			for (i = 0; i < __total_cycles; i++)
	
				for (long j = 1; j < __numnodes; j++)  {
					const float time = RTT(i, j);
CHK	
					const int bin = (int)((time / __range) * __num_bins);;
CHK	
					if (bin >= __num_bins){
						cout << "Time outside of __range: " << time << endl;
CHK	
					}
					else{
						__bins[bin]++;
CHK	
					}
				}
			
			cout << "Histogram" << endl;
CHK	
			cout << "=========" << endl << endl;
CHK	
			for (i = 0; i < __num_bins; i++)
				cout << ((float)i / (float)__num_bins) * __range << "\t"
					<< __bins[i] << endl;
			
			delete __times;
CHK	
		}
	/*	
	}	
	catch (rti13::Exception& e)  {
		cout << "Got Exception: " << &e << endl;
	}
	*/
	}
	catch(RTI_Exception &e ){
		cout << "Got Exception: " <<e.what() << endl;
	}

	while(1)usleep(1);
	delete __buffer;

	
	return 0;
}





