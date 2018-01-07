#ifndef OHT_H
#define OHT_H
/******************************************************************************
 *            OHT.h
 *
 *  Tue Sep 19 01:55:23 2006
 *  Copyright  2006  Bruce Wang
 *  cwwang2@gmail.com
 *******************************************************************************/

#include <iostream>
#include "globle.h"
#include "String.h"
#include "RTI_SpecificTypedefs.h"


namespace omt{

template <class T>
class OHT
{

public:
    struct      ObjectClassParsingInfo; //紀錄ObjectClass的Parsing Infomation
    struct      attributeParsingInfo;   //紀錄attribute的Parsing Infomation
    struct      Node;
	
    typedef     T value_type;
    typedef     T* value_ptr;

    char*               file_content; //代表整個XML file的字串。
    unsigned long       biggest_handle;//整個樹的object class handle的最大值
    Node*               root;
    
	OHT();


    //Destructor
    ~OHT();

    /*------------------- Tree operation START -----------------*/

    
	/*
	 * Method 1		: read_XMLfile
	 * Description	: 把xml案讀入file_content變數裡，其為型態char*之變數。若讀取失敗
	 *					回傳false
	 * Input		: fullPathFileName
	 * Output		: 
	 *				 True  : 讀取xml file成功
	 *				 False : 讀取xml file失敗
	 * Algorithm	:
	 */
    bool read_XMLfile(char* fullPathFileName);

	/*
	 * Method 2		: parsing
	 * Description	: 針對file_contentdd的Interaction Class Tree作parsing，若
	 *					parsing錯誤則return false，若讀取失敗則return FALSE
	 * Input		: 無
	 * Output		: 
	 *				 True  : 剖析檔案成功
	 *				 False : 剖析檔案失敗
	 * Algorithm	:
	 */
    bool parsing();


    //*************************************************************************
    //* Method 3    : isAncestorOf
    //* Description : 測試在樹結構底下 OChandleA 是否為 OChandleB的祖先(Ancestor)
    //*               若OChandleA是OChandleB的祖先則回傳1
    //*               若OChandleB是OChandleA的祖先則回傳-1
    //*               若OChandleA與OChandleB沒有祖先關係則回傳0
    //* Input       : OChandleA
    //*               OChandleB
    //* Output      :  1 :OChandleA是OChandleB的祖先
    //*               -1 :OChandleB是OChandleA的祖先
    //*                0 :OChandleA與OChandleB沒有祖先關係
    //* Algorithm   :
    //*************************************************************************
    int isAncestorOf(unsigned long OChandleA, unsigned long OChandleB);

    //*************************************************************************
    //* Method 4    : getObjectClassName
    //* Description : 給定任何Object Class handle給OHT，透過此function可得到的回傳
    //*               值為該handle所對應的object class name。倘若此handle不存在則回
    //*               傳空字串”\n”
    //* Input       : OChandle
    //* Output      : object class name
    //* Algorithm   :
    //*************************************************************************
    char* getObjectClassName(unsigned long OChandle);
	RTI_wstring* getRTIObjectClassName(unsigned long OChandle);



    //*************************************************************************
    //* Method 5    : getObjectClassHandle
    //* Description : 給定任何Object Class name給OHT，透過此function可得到的回傳值
    //*               為該name所對應的object class handle。倘若此name不存在則回傳值0
    //* Input       : OCname
    //* Output      : object class handle
    //*               0 = 不存在
    //* Algorithm   :
    //*************************************************************************
    unsigned long getObjectClassHandle(char* OCname);
	RTI_ObjectClassHandle *getRTIObjectClassHandle(char *OCname);

    //*************************************************************************
    //* Method 6    : getAttributeName
    //* Description : 給定OChandle可得知此handle所對應的特定Object Class，本函式回
    //*               傳在特定Object Class底下attribute handle所對應的attribute name
    //* Input       : OCname
    //*             attribute_handle
    //* Output      : attribute name
    //* Algorithm   :
    //*************************************************************************
    char* getAttributeName(unsigned long OChandle,unsigned long attribute_handle);
	RTI_wstring* getRTIAttributeName(unsigned long OChandle,unsigned long attribute_handle);

    //*************************************************************************
    //* Method 7    : getAttributeHandle
    //* Description : 給定OChandle可得知此handle所對應的特定Object Class，本函式回
    //*               傳在特定Object Class底下attribute name所對應的attribute handle
    //* Input       : OChandleS
    //*               attribute_name
    //* Output      : attribute handle
    //* Algorithm   :
    //*************************************************************************
    unsigned long getAttributeHandle(unsigned long OChandle,char* attribute_name);
	RTI_AttributeHandle *getRTIAttributeHandle(unsigned long OChandle,char* attribute_name);

    //*************************************************************************
    //* Method 8    : getObjectClass
    //* Description : 回傳指向obj T的指標 (所以必定滿足 T的Object Class Handle等於
    //*               OChandle值)
    //* Input       : OChandle
    //* Output      : T的指標
    //* Algorithm   :
    //*************************************************************************
    value_ptr getObjectClass(unsigned long OChandle);

    //*************************************************************************
    //* Method 9    : getInheritedNumber
    //* Description : 回傳該Object Class至root這條path上所有的Node數量 (這些Node包含
    //*               root與OChandle)
    //* Input       : OChandle
    //* Output      : 回傳該Object Class至root這條path上所有的Node數量
    //* Algorithm   :
    //*************************************************************************
    unsigned long getInheritedNumber(unsigned long OChandle);

    //*************************************************************************
    //* Method 10   : isObjectClass
    //* Description : 判斷此OChandle是否有對應到Object Class Tree中的其中一個Object Class
    //* Input       : isObjectClass
    //* Output      : True：有對應
    //*               False：無對應
    //* Algorithm   :
    //*************************************************************************
    bool isObjectClass (unsigned long OChandle);


    //*************************************************************************
    //* Method 11   : isObjectClass
    //* Description : 判斷此OCname是否有對應到Object Class Tree中的其中一個Object Class
    //* Input       : OChandle
    //* Output      : True：有對應
    //*              False：無對應
    //* Algorithm   :
    //*************************************************************************
    bool isObjectClass (char* OCname);

    //*************************************************************************
    //* Method 12   : isInheritedClassAttribute
    //* Description : 在OChandle所對應的Object Class下，若此attributeHandle對應的
    //*               attribute是繼承而來的iff回傳TRUE
    //* Input       : OChandle, attributeHandle
    //* Output      : True,有對應
    //*               False,無對應
    //* Algorithm   :
    //*************************************************************************
    bool isInheritedClassAttribute(unsigned long OChandle, unsigned long attributeHandle);
    //

    //*************************************************************************
    //* Method 13   : getNumberOfAttribute
    //* Description : 在OChandle對應Object Class之下，回傳該Object Class的attriubte的數量
    //* Input       : OChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfAttribute(unsigned long OChandle);

    //*************************************************************************
    //* Method 14   : getNumberOfInheritedAttribute
    //* Description : 回傳該Object Class繼承而來的attribute之數量(不包含自己Object
    //*               Class新增的attribute)
    //* Input       : OChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedAttribute(unsigned long OChandle);
    //

    //*************************************************************************
    //* Method 15   : getNumberOfInheritedClass
    //* Description : 回傳該Object Class至root這條path上所有的Node數量 (這些Node不包含OChandle)
    //* Input       : OChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedClass(unsigned long OChandle);

	unsigned long getClassNodeCount();

	unsigned long getClassTailValue(unsigned long OChandle);

	unsigned long getClassParentNode(unsigned long OChandle);
	
	
    /*--------------------- Tree operation END --------------------*/
    /*---------------------- 單純紀錄Parsing的資訊 START-------------------------*/
    struct ObjectClassParsingInfo{
        ObjectClassParsingInfo()
        {
            name = 0;
			rti_name = NULL;
            sharing = 0;
            semantics = 0;
            //attributeParsingInfo *attrInfoList[MAX_ABBRIBUTE_NO];
            //attributeParsingInfoList = attrInfoList;
            attributeParsingInfoList = new attributeParsingInfo* [MAX_ABBRIBUTE_NO];
            //memset(attrInfoList,0,MAX_ABBRIBUTE_NO);
        };
        //XML Parsing完後，對於每個ObjectClass的資訊，存放在此Structure
        char* name;
		RTI_wstring *rti_name;	/* ObjectClass name */
        char* sharing;
        char* semantics;
        attributeParsingInfo** attributeParsingInfoList;
    };
    struct attributeParsingInfo{
        attributeParsingInfo()
        {
            name = 0;
			rti_handle = NULL;
			rti_name = NULL;
            dataType = 0;
            updateType = 0;
            updateCondition = 0;
            updateConditionNotes = 0;
            ownership = 0;
            sharing = 0;
            dimensions = 0;
            transportation = 0;
            order = 0 ;
            semantics = 0;
        };
        //XML Parsing完後，對於每個attribute的資訊，存放在此Structure
        char* name;
		RTI_AttributeHandle *rti_handle;
		RTI_wstring *rti_name;
        char* dataType;
        char* updateType;
        char* updateCondition;
        char* updateConditionNotes;
        char* ownership;
        char* sharing;
        char* dimensions;
        char* transportation;
        char* order;
        char* semantics;
    };
    /*---------------------- 單純紀錄Parsing的資訊 END-------------------------*/



    struct Node{ //命名規則：Node裡的每個variable之命名，應以連續兩個底線為開頭
        Node(){
            __parsing_info = new ObjectClassParsingInfo;
            __child_no = 0;
            //Node *NodeList[MAX_CHILD_NO];
            //for(int i = 0; i< MAX_CHILD_NO; i++)
            //{
            //  NodeList[i] = 0;
            //}
            __child_List = new Node* [MAX_CHILD_NO];
            __parent = 0;
            //__child = 0;
            __prev = 0;
            __next = 0;
            __forward = 0;
            __backward = 0;
            // __first = 0;
            // __last = 0;
            //__attrInfoNo = 0;
            __biggest_handle = 0;
            __data = 0;
            __level = 0;
            __tail_value = 0;
            __handle = 0;
			rti_handle = NULL;
            __inherited_attr_no=0;
        };

        unsigned long   __handle;//此為Object Class Handle
		RTI_ObjectClassHandle *rti_handle;
		
        ObjectClassParsingInfo* __parsing_info; //此Node紀錄於XML上的資訊
        unsigned long __biggest_handle;//此ObjectClass中attribute handle最大值
        value_ptr       __data; //value_type之型態為T,為我們日後保留之彈性
        //XmlCursor __location;
        unsigned long __level;
        //若是使用 sibiling實現可能會有以下欄位
        //Node* __parent;
        //Node* __child;
        Node* __prev;
        Node* __next;
        Node* __forward;
        Node* __backward;
        //若是使用 一般多元素的實現方法，可能會有以下欄位
        Node* __parent;
        Node** __child_List;
        int   __child_no;   //記錄目前有幾個child Node
        //int   __attrInfoNo;
        unsigned long __tail_value;
        unsigned long __inherited_attr_no;

    };
    

private:
	unsigned long nodeCounter;
    Node** nodeArray;
    unsigned long Level;

    //辨識此段XML字串為何種資料
    int IdentifyNode(const char* p);
    const char* ParseObjectClass(const char* , Node*);
    const char* ParseAttributeNode(const char* , Node *,attributeParsingInfo *);
    const char* ParseAttribute(const char* , XML_STRING* ,XML_STRING* );
    const char* ReadName(const char* , XML_STRING* );
    const char* ReadValue(const char* , Node *);
    const char* ReadText(const char* , XML_STRING *, bool , const char*, bool );
    const char* SkipTag(const char * );
    void  addNode(Node * parentNode, Node* childNode);
    void  addAttributeInfo(Node * , attributeParsingInfo* );
    void addInheriteAttribute(Node *);
    void assignTailValue();
    void treeReAllocation();
};

//#ifndef OHT_CPP
//#include "OHT.cpp"
//#endif
}
#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
