#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>
#include <math.h>

#define SIZE 800
#define MAX 10


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
extern RTI_AttributeHandleValueMap	AHVM;
	

struct myfishes{
	int x;
	int y;
	int age;
}local[1000],food[1000];

map<RTI_ObjectInstanceHandle,int>  foodlocal;
map<int,RTI_ObjectInstanceHandle>  localfood;
int foodtotal;

RTI_AttributeHandle   sposX;
RTI_AttributeHandle   sposY;
RTI_AttributeHandle   btype;
RTI_ObjectClassHandle SmallFish;
RTI_ObjectClassHandle BigFish;
RTI_AttributeHandle   bposX;
RTI_AttributeHandle   bposY;
RTI_ObjectInstanceHandle fishes[1000];

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
				int p=foodlocal[theObject];
				if((*theAttributeValues).count(sposX)){
					food[p].x=*((int*)((*theAttributeValues)[sposX].data()));
				}
				if((*theAttributeValues).count(sposY)){
					food[p].y=*((int*)((*theAttributeValues)[sposY].data()));
				}

			}
		
		void discoverObjectInstance	//Used by FederateB
			(RTI_ObjectInstanceHandle const & theObject,
			 RTI_ObjectClassHandle const & theObjectClass,
			 RTI_wstring const & theObjectInstanceName)
			throw (RTI_CouldNotDiscover,
                   RTI_ObjectClassNotKnown,
                   RTI_FederateInternalError)
			{
				foodlocal[theObject]=foodtotal;
				localfood[foodtotal]=theObject;
				foodtotal++;
			}
		
		void synchronizationPointRegistrationSucceeded
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError)
			{
				cout << "synchronizationPointRegistrationSucceeded is called" << endl;
			}
		
		    
};



pthread_mutex_t totalmux;
void killfish(int i);
void addfish(int i);

RTIambImplementor a;
RTI_AttributeHandleValueMap	AHVM;
RTI_AttributeHandleValueMap	FIVM;

RTI_InteractionClassHandle       eat;
RTI_ParameterHandle              fishid;
RTI_ParameterHandleValueMap      PHVM;

int total;
int objnumber;

int main()
{
#if 1
	/*Step 0.1. main�{���ϰ��ܼư��ܼƫŧi*/
	int inputint;
	int i;
	foodtotal=0;
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
		bposX		= a.getAttributeHandle(BigFish, L"posX");
		bposY		= a.getAttributeHandle(BigFish, L"posY");
		btype           = a.getAttributeHandle(BigFish, L"type");

		bfishattr.insert(bposX);
		bfishattr.insert(bposY);
		bfishattr.insert(btype);
		a.publishObjectClassAttributes(BigFish,bfishattr);

		eat= a.getInteractionClassHandle(L"interactionClassA");
		fishid= a.getParameterHandle(eat, L"parameterA");
		a.publishInteractionClass(eat);
		 
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}


	/*Step 2. ���ݿ�J���*/
	printf("\n\n Input any non-empty string for continue to Subscribe!! INTEGER \n");
	scanf("%d",&inputint);



	
	sposX=a.getAttributeHandle(SmallFish, L"posX");
	sposY=a.getAttributeHandle(SmallFish, L"posY");
	/*Step 3. �@ Subscribe*/	
	try {
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

	try{
		srand(time(NULL));
		int posx,posy,type;
		type=BIG;
		int j;
		int eatable;
		for(i=0;i<MAX;i++){
			
			fishes[i]=a.registerObjectInstance(BigFish);
			posx=rand()%SIZE;
			posy=rand()%SIZE;
			cout << "x=" << posx << " y=" << posy << endl;
			local[i].x=posx;
			local[i].y=posy;
			local[i].age=0;
			AHVM[btype]=RTI_AttributeValue(static_cast<void*>(&type), sizeof(int));
			AHVM[bposX]=RTI_AttributeValue(static_cast<void*>(&posx), sizeof(int));
			AHVM[bposY]=RTI_AttributeValue(static_cast<void*>(&posy), sizeof(int));
			a.updateAttributeValues(fishes[i], AHVM, 
					RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );

		}
		//for(i=total;i<100;i++) fishes[i]=a.registerObjectInstance(BigFish,L"SF1");
		AHVM.clear();
		total=MAX;
		objnumber=MAX;
		while(1){
			for(i=0;i<total;i++){
				local[i].age++;
				
				if(local[i].age>=10&&rand()%2==0){
					eatable=0;
					for(j=0;j<foodtotal;j++){
						if(abs(food[j].x-local[i].x)<20){
							eatable=1;
							break;
						}
						if(abs(food[j].y-local[i].y)<20){
							eatable=1;
							break;
						}
					}
					if(eatable==0){ 
						killfish(i);
						continue;
					}
					PHVM[fishid] = RTI_ParameterValue(static_cast<void*>(&j),sizeof(int));
					a.sendInteraction(eat, PHVM, 
							RTI_UserSuppliedTag(static_cast<void*>(&j),sizeof(int)));
					local[i].age=0;
					addfish(i);
					//killfish(i);
				}
								
				posx=rand()%11-5;
				posy=rand()%11-5;
				local[i].x+=posx;
				local[i].y+=posy;
				if(local[i].x<0) local[i].x=0;
				if(local[i].y<0) local[i].y=0;
				if(local[i].x>=SIZE) local[i].x=SIZE-1;	
				if(local[i].y>=SIZE) local[i].y=SIZE-1;	
				AHVM[bposX]=RTI_AttributeValue(static_cast<void*>(&local[i].x), sizeof(int));
				AHVM[bposY]=RTI_AttributeValue(static_cast<void*>(&local[i].y), sizeof(int));
				a.updateAttributeValues(fishes[i], AHVM,
						RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );
			}
		sleep(1);
		}
		
	}catch(RTI_Exception &e){
		cout << e.what() <<endl;
	}
	


#endif
	printf("Fish over\n");
	        
		
	//sleep(10);
			
	return 0;
}

void killfish(int i){
	int fishleave=0;
	pthread_mutex_lock (&totalmux);
	if(i!=total-1){
		local[i]=local[total-1];
		FIVM[bposX]=RTI_AttributeValue(static_cast<void*>(&local[i].x), sizeof(int));
		FIVM[bposY]=RTI_AttributeValue(static_cast<void*>(&local[i].y), sizeof(int));
		a.updateAttributeValues(fishes[i], FIVM,
				RTI_UserSuppliedTag(static_cast<void*>(&i), sizeof(int)) );
	}
	FIVM.clear();
	FIVM[btype]=RTI_AttributeValue(static_cast<void*>(&fishleave), sizeof(int));
	a.updateAttributeValues(fishes[total-1], FIVM,
			RTI_UserSuppliedTag(static_cast<void*>(&fishleave),
				sizeof(int)) );
	total--;
	FIVM.clear();
	pthread_mutex_unlock (&totalmux);
}

void addfish(int i){
	int joinfish=BIG;
	if(total>50) return;
	pthread_mutex_lock (&totalmux);
	if(total>=objnumber){
		fishes[total]=a.registerObjectInstance(BigFish);
		objnumber++;
	}
	FIVM.clear();
	FIVM[btype]=RTI_AttributeValue(static_cast<void*>(&joinfish), sizeof(int));
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


/*
 * vim: ts=4 sts=4 sw=4
 */




