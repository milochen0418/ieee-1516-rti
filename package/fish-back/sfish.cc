#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>


#define SIZE 800
#define MAX 20


#define SMALL 1
#define BIG 2

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

extern RTIambImplementor a;

RTI_AttributeHandleSet           sfishattr;
RTI_AttributeHandleSet           bfishattr;
RTI_AttributeHandleValueMap	AHVM;
	

struct myfishes{
	int x;
	int y;
	int age;
}local[1000];

int total;
int objnumber;

pthread_mutex_t totalmux;
void killfish(int i);
void addfish(int i);


RTI_InteractionClassHandle       eat;
RTI_ParameterHandle              fishid;
RTI_ParameterHandleValueMap      PHVM;
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
				int eatenid=*static_cast<const int*>( (*theParameterValues)[fishid].data());
				killfish(eatenid);
			}	
				
};






RTIambImplementor a;
RTI_AttributeHandleValueMap	FIVM;

       
RTI_AttributeHandle   sposX;
RTI_AttributeHandle   sposY;
RTI_AttributeHandle   stype;
RTI_ObjectClassHandle SmallFish;
RTI_ObjectClassHandle BigFish;
RTI_AttributeHandle   bposX;
RTI_AttributeHandle   bposY;
RTI_ObjectInstanceHandle fishes[1000];


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


	
	try {
		SmallFish		= a.getObjectClassHandle(L"SmallFish");
		BigFish		= a.getObjectClassHandle(L"BigFish");
		sposX		= a.getAttributeHandle(SmallFish, L"posX");
		sposY		= a.getAttributeHandle(SmallFish, L"posY");
		stype           = a.getAttributeHandle(SmallFish, L"type");

		sfishattr.insert(sposX);
		sfishattr.insert(sposY);
		sfishattr.insert(stype);
		a.publishObjectClassAttributes(SmallFish,sfishattr);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}


	/*Step 2. ���ݿ�J���*/
	printf("\n\n Input any non-empty string for continue to Subscribe!! INTEGER \n");
	scanf("%d",&inputint);

	
	/*Step 3. �@ Subscribe*/	
	try {
		bposX=a.getAttributeHandle(BigFish, L"posX");
		bposY=a.getAttributeHandle(BigFish, L"posY");
		bfishattr.insert(bposX);
		bfishattr.insert(bposY);

		eat= a.getInteractionClassHandle(L"interactionClassA");
		fishid= a.getParameterHandle(eat, L"parameterA");

		a.subscribeInteractionClass(eat);
				 
		//a.subscribeObjectClassAttributes(BigFish, bfishattr );
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}

	
	/*Step 4. ���ݿ�J���*/
	printf("\n\n Input any non-empty string for continue to Register!! \n");
	scanf("%d",&inputint);
		
	/*Step 5. �@ Resgieter Object Instance*/

	int posx,posy,type;

	pthread_mutex_init(&totalmux, NULL);
	try{
		srand(time(NULL));
		type=SMALL;
		for(i=0;i<MAX;i++){
			
			fishes[i]=a.registerObjectInstance(SmallFish);
			posx=rand()%SIZE;
			posy=rand()%SIZE;
			cout << "x=" << posx << " y=" << posy << endl;
			local[i].x=posx;
			local[i].y=posy;
			local[i].age=0;
			AHVM[stype]=RTI_AttributeValue(static_cast<void*>(&type), sizeof(int));
			AHVM[sposX]=RTI_AttributeValue(static_cast<void*>(&posx), sizeof(int));
			AHVM[sposY]=RTI_AttributeValue(static_cast<void*>(&posy), sizeof(int));
			a.updateAttributeValues(fishes[i], AHVM, 
					RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );

		}
		AHVM.clear();
		total=MAX;
		objnumber=MAX;
		while(1){
			for(i=0;i<total;i++){
				printf("updating fish:%d\n",i);
				local[i].age++;
				/*if(local[i].age>=10&&rand()%2==1){
					printf("add fish:%d\n",total);
					fishes[total]=a.registerObjectInstance(SmallFish,L"SF1");	
					printf("ABCDEFG: fish%d handle:%ld\n",total,fishes[total].getHandle());
					posx=rand()%SIZE;
					posy=rand()%SIZE;
					local[total].x=posx;
					local[total].y=posy;
					local[total].age=0;
					AHVM[stype]=RTI_AttributeValue(static_cast<void*>(&type), sizeof(int));
					a.updateAttributeValues(fishes[total], AHVM,
							RTI_UserSuppliedTag(static_cast<void*>(&total), sizeof(int)) );
					total++;
					AHVM.clear();
					local[i].age=0;
				}*/
				if(local[i].age>=20&&rand()%3==0){
					addfish(i);
					local[i].age=0;
				}
				if(local[i].age>=100&&rand()%5==0){
					killfish(i);
					continue;
				}
				posx=rand()%11-5;
				posy=rand()%11-5;
				local[i].x+=posx;
				local[i].y+=posy;
				if(local[i].x<0) local[i].x=0;
				if(local[i].y<0) local[i].y=0;
				if(local[i].x>=SIZE) local[i].x=SIZE-1;	
				if(local[i].y>=SIZE) local[i].y=SIZE-1;	
				AHVM[sposX]=RTI_AttributeValue(static_cast<void*>(&local[i].x), sizeof(int));
				AHVM[sposY]=RTI_AttributeValue(static_cast<void*>(&local[i].y), sizeof(int));
				a.updateAttributeValues(fishes[i], AHVM,
						RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );
			}
		sleep(1);
		}
		
	}catch(RTI_Exception &e){
		cout << e.what() <<endl;
		printf("now:%d total==%d\n",i,total);
	}
	


#endif
	printf("Fish over\n");
	        
		
	//sleep(10);
			
	return 0;
}

void addfish(int i){
	int joinfish=SMALL;
	if(total>100) return;
	pthread_mutex_lock (&totalmux);
	if(total>=objnumber){
		fishes[total]=a.registerObjectInstance(SmallFish);
		objnumber++;
	}
	FIVM.clear();
	FIVM[stype]=RTI_AttributeValue(static_cast<void*>(&joinfish), sizeof(int));
	a.updateAttributeValues(fishes[total], FIVM,
			RTI_UserSuppliedTag(static_cast<void*>(&joinfish),
				sizeof(int)) );
	local[total].x=rand()%800;
	local[total].y=rand()%800;
	local[total].age=0;
	total++;
	FIVM.clear();
	pthread_mutex_unlock (&totalmux);
}

void killfish(int i){
	int fishleave=0;
	if(i>=total) return;
	pthread_mutex_lock (&totalmux);
	if(i!=total-1){
		local[i]=local[total-1];
		FIVM[sposX]=RTI_AttributeValue(static_cast<void*>(&local[i].x), sizeof(int));
		FIVM[sposY]=RTI_AttributeValue(static_cast<void*>(&local[i].y), sizeof(int));
		a.updateAttributeValues(fishes[i], FIVM,
				RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );
	}
	FIVM.clear();
	local[total-1].x=-100;
	local[total-1].y=-100;
	FIVM[sposX]=RTI_AttributeValue(static_cast<void*>(&local[total-1].x), sizeof(int));
	FIVM[sposY]=RTI_AttributeValue(static_cast<void*>(&local[total-1].y), sizeof(int));
	FIVM[stype]=RTI_AttributeValue(static_cast<void*>(&fishleave), sizeof(int));
	a.updateAttributeValues(fishes[total-1], FIVM,
			RTI_UserSuppliedTag(static_cast<void*>(&fishleave), 
				sizeof(int)) );
	total--;
	FIVM.clear();
	pthread_mutex_unlock (&totalmux);					
}

/*
 * vim: ts=4 sts=4 sw=4
 */




