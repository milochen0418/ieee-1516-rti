#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
#include "FedAmbImplementor.h"
#include <map>
#include <X11/Xlib.h>



/* Federation大架構解釋
 * 變數方面：
 * 有FederateA 與 Federate B
 * FederateA publish objectClassA, subscribe objectClassB
 * FederateB publish objectClassB, subscribe objectClassA
 * FederateA register InstA with handle=InstAHandle  through the class objectClassA
 * FederateB register InstB with hadnle=InstBHandle  through the class objectClassB
 * attributeA為InstA對應的 instance attribute
 * attributeB為InstB對應的 instance attribute
 * 
 * 行為方面：
 * 在變數方面的部份都定下來後，FederateA的main程式會首先
 * 對InstAHandle上所指定的attributeA作value update，之後main就不作事。
 * FederateB的main程式在作完變數方面的宣告之後不作任何事。
 * 主要的程式執行時間是落在 FederateA上的reflectAttributeValue與 FederateB的reflectAttributeValue
 * FederateB.LRC.reflectAttributeValue將InstA.attributeA的值複製到InstB.attributeB就直作作傳送
 * FederateA.LRC.reflectAttributeValue令InstA.attributeA=InstB.attributeB+1，此時程式
 * 會判斷InstA.attributeA的這個RTI_AttributeValue是否小於等於FB_TEST_TIME，若小於此值則繼續，反之
 * 則程式結束。
 */

/* FederateA's Algorithm
 * 
 * Step 0.1. main程式區域變數宣告
 * Step 0.2. 宣告相關的classhandle, attribute
 * Step 1. 配置實體內容給相關的classhandle, attributehandle，緊接著作Publish
 * Step 2. 等待輸入整數
 * Step 3. 作Subscribe
 * Step 4. 等待輸入整數
 * Step 5. 作Register Object Instance
 * Step 6. 開始送出唯一一次的Value Update
 * 
 */

/* FederateA's FedAmbassador
 * reflectAttributeValues:= (
 *                           preCondition 1: 該Object Instance已經先被我們discover到的(靠InstManager可作此檢查)
 *                           preCondition 2: 該Object Instance代表的是 InstB 的instance
 *			    ){
 *         InstB._value = theAttributeList[attributeB]
 *         InstA._value = InstB._value + 1
 *         IF InstA._value <= FB_TEST_TIME THEN
 *               LRC.updateAttributeValues(InstA, AHVM[attributeA] = InstA._value)
 *         ELSE
 *               程式結束
 *         END_IF
 * }			    
 *
 * discoverAttributeValues:= (
 * 			       preCondition 1: 該instnace的object class應為objectClassB
 * 			     ){
 *         InstBHandle = DiscoverObjectInstanceHandle 
 *         InstManager.insert(InstB, InstBHandle)
 * }			     
 */

/* InstA與InstB的關係
 * InstA是FederateA 所 register,其objectInstanceHandle為 InstAHandle
 * InstB是FederateB 所 register,其objectInstanceHandle為 InstBHandle
 * InstManager目前所管理的資料為 {InstA,InstAHandle}與{InstB,InstBHandle}
 */


Window create_simple_window(Display* display, int width, int height, int x, int y);
GC create_gc(Display* display, Window win, int reverse_video);
Display* display;		/* pointer to X Display structure.           */
int screen_num;		/* number of screen to place the window on.  */
Window win;			/* pointer to the newly created window.      */

GC gc;			/* GC (graphics context) used for drawing    */
				/*  in our window.			     */


int counter;
float totaltime;
float avgtime;
float timeuse;

time_t t1, t2;
using namespace std;
class MyFedAmb;

extern RTIambImplementor a;
RTI_AttributeHandleSet           sfishattr;
RTI_AttributeHandleSet           bfishattr;
extern RTI_AttributeHandleValueMap	AHVM;
	

RTI_AttributeHandle   posX;
RTI_AttributeHandle   posY;
RTI_AttributeHandle   type;
RTI_ObjectClassHandle Fish;
RTI_ObjectClassHandle SmallFish;
RTI_ObjectClassHandle BigFish;

map<RTI_ObjectInstanceHandle,int>  localmap;
map<int,RTI_ObjectInstanceHandle>  globalmap;
int total;

#define MAXFISH 1000
#define SIZE 800
#define SMALL 1
#define BIG 2

struct myfishes{
   int x;
   int y;
   int type;
}local[MAXFISH];

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
				int p=localmap[theObject];
				if((*theAttributeValues).count(posX)){
					local[p].x=*((int*)((*theAttributeValues)[posX].data()));
				}
				if((*theAttributeValues).count(posY)){
					local[p].y=*((int*)((*theAttributeValues)[posY].data()));
				}
				if((*theAttributeValues).count(type)){
					local[p].type=*((int*)((*theAttributeValues)[type].data()));
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
				localmap[theObject]=total;
				globalmap[total]=theObject;
				total++;
				cout << "Discovered Fish " <<total << endl;
			}
		
		void synchronizationPointRegistrationSucceeded
			(RTI_wstring const & label)
			throw (RTI_FederateInternalError)
			{
				cout << "synchronizationPointRegistrationSucceeded is called" << endl;
			}
		
		    
};






RTIambImplementor a;
RTI_AttributeHandleValueMap	AHVM;


int main()
{
#if 1
	/*Step 0.1. main程式區域變數基本變數宣告*/
	int inputint;
	total=0;
	int i;
	RTI_auto_ptr< RTI_FederateAmbassador > federateAmbassador(new MyFedAmb());
	RTI_auto_ptr< RTI_LogicalTimeFactory > logicalTimeFactory;
	RTI_auto_ptr< RTI_LogicalTimeIntervalFactory > logicalTimeIntervalFactory;

	RTI_wstring fedName(L"MyFederation");
	RTI_wstring FDDfile(L"sample.xml");
	RTI_wstring fedType(L"go");
	RTI_FederateHandle theID;

	/*Step 0.2. 對RTI作 Create and Join*/
	try {
		a.createFederationExecution(fedName, FDDfile);
		theID=a.joinFederationExecution(fedType, fedName, federateAmbassador, logicalTimeFactory,logicalTimeIntervalFactory);
	}
	catch (RTI_Exception &e){
		cout << e.what() << endl;
		exit(1);
	}



		

	
	//  P/S(FedA)={ subOC=objectClassB, pubOC=objectClassA  }

	/*Step 1. 配置實體內容給相關的classhandle,attributehandle ，緊接著作 Publish*/


	printf("\n\n Input any non-empty string for continue to Subscribe!! INTEGER \n");
	scanf("%d",&inputint);
	
	try {
		SmallFish		= a.getObjectClassHandle(L"SmallFish");
		BigFish		= a.getObjectClassHandle(L"BigFish");
		Fish               = a.getObjectClassHandle(L"Fish");
		posX		= a.getAttributeHandle(Fish, L"posX");
		posY		= a.getAttributeHandle(Fish, L"posY");
		type            = a.getAttributeHandle(Fish, L"type");
		sfishattr.insert(posX);
		sfishattr.insert(type);
		sfishattr.insert(posY);
		a.subscribeObjectClassAttributes(Fish,sfishattr);
	}
	catch (RTI_Exception &e){
		cout << e.what() <<endl;
	}


	/*Step 2. 等待輸入整數*/



	
	
	/*Step 4. 等待輸入整數*/
	printf("\n\n Input any non-empty string for continue to Register!! \n");
	scanf("%d",&inputint);
	char *display_name = getenv("DISPLAY");  /* address of the X display.    */
	display = XOpenDisplay(display_name);
	if (display == NULL) {
		fprintf(stderr, " cannot connect to X server '%s'\n"
				, display_name);
		exit(1);
	}		
	screen_num = DefaultScreen(display);
	win = create_simple_window(display, SIZE, SIZE, 0, 0);  
	gc = create_gc(display, win, 0);
	XSync(display, False);

	while (1){
		XSetForeground(display, gc, WhitePixel(display, screen_num));
		XFillRectangle(display, win, gc, 0, 0,SIZE,SIZE);
		XSetForeground(display, gc, BlackPixel(display, screen_num));
		for(i=0;i<total;i++){
			switch(local[i].type){
				case SMALL:
					XFillArc(display, win, gc, local[i].x-(15/2)
							, local[i].y-(15/2), 15, 15, 0, 360*64);
					break;
				case BIG:
					XFillRectangle(display, win, gc, local[i].x,
							local[i].y,15,15);
					break;
			}
		}
		XSync(display, False);
		usleep(100000);
	}
  
#endif
	printf("xwin over\n");
	for(i=0;i<total;i++){
		cout << "type: " << local[i].type << " ";
		cout << "position" << i << "(" << local[i].x << "," << local[i].y << ")"  << endl;
	}
	sleep(10);
			
	return 0;
}


Window
create_simple_window(Display* display, int width, int height, int x, int y)
{
	int screen_num = DefaultScreen(display);
	int win_border_width = 2;
	Window win;
	win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
			x, y, width, height, win_border_width,
			BlackPixel(display, screen_num),
			WhitePixel(display, screen_num));
	XMapWindow(display, win);
	XFlush(display);
	return win;
}

GC
create_gc(Display* display, Window win, int reverse_video)
{
	GC gc;				/* handle of newly created GC.  */
	unsigned long valuemask = 0;		/* which values in 'values' to  */
	XGCValues values;			/* initial values for the GC.   */
	unsigned int line_width = 2;		/* line width for the GC.       */
	int line_style = LineSolid;		/* style for lines drawing and  */
	int cap_style = CapButt;		/* style of the line's edje and */
	int join_style = JoinBevel;		/*  joined lines.		*/
	int screen_num = DefaultScreen(display);
	gc = XCreateGC(display, win, valuemask, &values);
	if (gc < 0) {
		fprintf(stderr, "XCreateGC: \n");
	}
	if (reverse_video) {
		XSetForeground(display, gc, WhitePixel(display, screen_num));
		XSetBackground(display, gc, BlackPixel(display, screen_num));
	}
	else {
		XSetForeground(display, gc, BlackPixel(display, screen_num));
		XSetBackground(display, gc, WhitePixel(display, screen_num));
	}
	XSetLineAttributes(display, gc,
			line_width, line_style, cap_style, join_style);
	XSetFillStyle(display, gc, FillSolid);
	return gc;
}


/*
 * vim: ts=4 sts=4 sw=4
 */




