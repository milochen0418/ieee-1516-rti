#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>



/* Federation�j�[�c����
 * �ܼƤ譱�G
 * ��FederateA �P Federate B
 * FederateA publish objectClassA, subscribe objectClassB
 * FederateB publish objectClassB, subscribe objectClassA
 * FederateA register InstA with handle=InstAHandle  through the class objectClassA
 * FederateB register InstB with hadnle=InstBHandle  through the class objectClassB
 * attributeA��InstA������ instance attribute
 * attributeB��InstB������ instance attribute
 * 
 * �欰�譱�G
 * �b�ܼƤ譱���������w�U�ӫ�AFederateA��main�{���|����
 * ��InstAHandle�W�ҫ��w��attributeA�@value update�A����main�N���@�ơC
 * FederateB��main�{���b�@���ܼƤ譱���ŧi���ᤣ�@����ơC
 * �D�n���{������ɶ��O���b FederateA�W��reflectAttributeValue�P FederateB��reflectAttributeValue
 * FederateB.LRC.reflectAttributeValue�NInstA.attributeA���Ƚƻs��InstB.attributeB�N���@�@�ǰe
 * FederateA.LRC.reflectAttributeValue�OInstA.attributeA=InstB.attributeB+1�A���ɵ{��
 * �|�P�_InstA.attributeA���o��RTI_AttributeValue�O�_�p�󵥩�FB_TEST_TIME�A�Y�p�󦹭ȫh�~��A�Ϥ�
 * �h�{�������C
 */

/* FederateA's Algorithm
 * 
 * Step 0.1. main�{���ϰ��ܼƫŧi
 * Step 0.2. �ŧi������classhandle, attribute
 * Step 1. �t�m���餺�e��������classhandle, attributehandle�A�򱵵ۧ@Publish
 * Step 2. ���ݿ�J���
 * Step 3. �@Subscribe
 * Step 4. ���ݿ�J���
 * Step 5. �@Register Object Instance
 * Step 6. �}�l�e�X�ߤ@�@����Value Update
 * 
 */

/* FederateA's FedAmbassador
 * reflectAttributeValues:= (
 *                           preCondition 1: ��Object Instance�w�g���Q�ڭ�discover�쪺(�aInstManager�i�@���ˬd)
 *                           preCondition 2: ��Object Instance�N���O InstB ��instance
 *			    ){
 *         InstB._value = theAttributeList[attributeB]
 *         InstA._value = InstB._value + 1
 *         IF InstA._value <= FB_TEST_TIME THEN
 *               LRC.updateAttributeValues(InstA, AHVM[attributeA] = InstA._value)
 *         ELSE
 *               �{������
 *         END_IF
 * }			    
 *
 * discoverAttributeValues:= (
 * 			       preCondition 1: ��instnace��object class����objectClassB
 * 			     ){
 *         InstBHandle = DiscoverObjectInstanceHandle 
 *         InstManager.insert(InstB, InstBHandle)
 * }			     
 */

/* InstA�PInstB�����Y
 * InstA�OFederateA �� register,��objectInstanceHandle�� InstAHandle
 * InstB�OFederateB �� register,��objectInstanceHandle�� InstBHandle
 * InstManager�ثe�Һ޲z����Ƭ� {InstA,InstAHandle}�P{InstB,InstBHandle}
 */




#define FB_TEST_TIME 100
#define FB_TEST_SIZE 65536*4*4
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
RTI_AttributeHandleSet           sfishattr;
RTI_AttributeHandleSet           bfishattr;
extern RTI_AttributeHandleValueMap	AHVM;
	

//RTI_AttributeHandle   posX;
//RTI_AttributeHandle   posY;
//RTI_ObjectClassHandle SmallFish;
//RTI_ObjectClassHandle BigFish;




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
				
			}
		
		void discoverObjectInstance	//Used by FederateB
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_ObjectClassHandle const & theObjectClass,
			 RTI_wstring const & theObjectInstanceName)
			throw (RTI_CouldNotDiscover,
                   RTI_ObjectClassNotKnown,
                   RTI_FederateInternalError)
			{
				
				
			}
		
		void synchronizationPointRegistrationSucceeded
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError)
			{
				cout << "synchronizationPointRegistrationSucceeded is called" << endl;
			}
		
		    
};






RTI_ObjectInstanceHandle InstAHandle;
RTI_ObjectInstanceHandle InstBHandle;
RTIambImplementor a;
RTI_AttributeHandleValueMap	AHVM;


int main()
{
#if 1
	/*Step 0.1. main�{���ϰ��ܼư��ܼƫŧi*/
	int inputint;
	int i;
	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;

	/*Step 0.2. ��RTI�@ Create and Join*/
	try {
		a.createFederationExecution(fedName, FDDfile);
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}



		

	
	//  P/S(FedA)={ subOC=objectClassB, pubOC=objectClassA  }

	/*Step 1. �t�m���餺�e��������classhandle,attributehandle �A�򱵵ۧ@ Publish*/

       RTI_AttributeHandle   sposX;
       RTI_AttributeHandle   sposY;
       RTI_ObjectClassHandle SmallFish;
       RTI_ObjectClassHandle BigFish;

	
	try {
		//a.publishObjectClassAttributes(SmallFish,sfishattr);

		//

		SmallFish		= a.getObjectClassHandle(L"SmallFish");
		BigFish		= a.getObjectClassHandle(L"BigFish");
	        RTI_AttributeHandle   bposX=a.getAttributeHandle(BigFish, L"posX");
		RTI_AttributeHandle   bposY=a.getAttributeHandle(BigFish, L"posY");
		bfishattr.insert(bposX);
		bfishattr.insert(bposY);
		a.publishObjectClassAttributes(BigFish,bfishattr);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}


	/*Step 2. ���ݿ�J���*/
	printf("\n\n Input any non-empty string for continue to Subscribe!! INTEGER \n");
	scanf("%d",&inputint);



	
	/*Step 3. �@ Subscribe*/	
	try {
				 
		sposX		= a.getAttributeHandle(SmallFish, L"posX");
		sposY		= a.getAttributeHandle(SmallFish, L"posY");
		sfishattr.insert(sposX);
		sfishattr.insert(sposY);
		a.subscribeObjectClassAttributes(SmallFish, sfishattr );
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}

	
	/*Step 4. ���ݿ�J���*/
	printf("\n\n Input any non-empty string for continue to Register!! \n");
	scanf("%d",&inputint);
		
	/*Step 5. �@ Resgieter Object Instance*/

	RTI_ObjectInstanceHandle fishes[100];
		
	try{
		for(i=0;i<100;i++){
			fishes[i]=a.registerObjectInstance(BigFish,L"BF1");
		}
	}catch(RTI_Exception &e){
		cout << e.what() <<endl;
	}
	//InstManager.insert(InstA, InstAHandle);
	


	
	   
	/*Step 6. �}�l�e�X�ߤ@�@����Value Update*/
	/*i=0;
	t1 = time(NULL);
	counter=i;
	gettimeofday(&tpstart[counter],NULL);
	AHVM[attributeA]=RTI_AttributeValue(static_cast<void*>(&i), sizeof(int));
	a.updateAttributeValues(InstAHandle, AHVM, RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );
*/
		
	
	
	
	
  
   

	
	


/*while(1)
	sleep(1);

	try {
		a.resignFederationExecution(RTI_UNCONDITIONALLY_DIVEST_ATTRIBUTES);
		a.destroyFederationExecution(fedName);
		
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}
*/
#endif
	printf("Fish over\n");
	sleep(10);
	return 0;
}

/*
 * vim: ts=4 sts=4 sw=4
 */




