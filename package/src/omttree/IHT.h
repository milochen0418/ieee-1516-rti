#ifndef IHT_H
#define IHT_H
/******************************************************************************
 *            IHT.h
 *
 *  Tue Nov 21 12:55:23 2006
 *  Copyright  2006  Bruce Wang
 *  cwwang2@gmail.com
 *******************************************************************************/

#include <iostream>
#include "globle.h"
#include "String.h"
#include "RTI_SpecificTypedefs.h"

namespace omt{

template <typename T>
class IHT{

public:

    struct InteractionClassParsingInfo;//紀錄InteractionClass的Parsing Infomation
    struct parameterParsingInfo; //紀錄parameter的Parsing Infomation
    struct Node;

    typedef T value_type;
    typedef T* value_ptr;

    char* file_content; //代表整個XML file的字串。
    unsigned long biggest_handle;//整個樹的interaction class handle的最大值
    Node* root;



    IHT<T>();

    ~IHT<T>();



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
	 * Description	: 針對file_content作有關Interaction Class Table的parsing，
	 *					若parsing錯誤則return false
	 * Input		: 無
	 * Output		: 
	 *				 True  : 剖析檔案成功
	 *				 False : 剖析檔案失敗
	 * Algorithm	:
	 */
    bool parsing();

    //*************************************************************************
    //* Method 3    : isAncestorOf
    //* Description : 測試在樹結構底下 IChandleA 是否為 IChandleB的祖先(Ancestor)
    //*               若IChandleA是IChandleB的祖先則回傳1
    //*               若IChandleB是IChandleA的祖先則回傳-1
    //*               若IChandleA與IChandleB沒有祖先關係則回傳0
    //* Input       : IChandleA
    //*               IChandleB
    //* Output      :  1 :IChandleA是IChandleB的祖先
    //*               -1 :IChandleB是IChandleA的祖先
    //*                0 :IChandleA與IChandleB沒有祖先關係
    //* Algorithm   :
    //*************************************************************************
    int isAncestorOf(unsigned long IChandleA, unsigned long IChandleB);

    //*************************************************************************
    //* Method 4    : getInteractionClassName
    //* Description : 給定任何Interatction Class handle給OHT，透過此function可得到的回傳
    //*               值為該handle所對應的Interatction class name。倘若此handle不存在則回
    //*               傳空字串"\n"
    //* Input       : IChandle
    //* Output      : 回傳該class handle所對應的name
    //*               "\n" = 不存在
    //* Algorithm   :
    //*************************************************************************
    char* getInteractionClassName(unsigned long IChandle);
	RTI_wstring* getRTIInteractionClassName(unsigned long IChandle);

    //*************************************************************************
    //* Method 5    : getInteractionClassHandle
    //* Description : 給定任何Interaction Class name給OHT，透過此function可得到的回傳值
    //*               為該name所對應的Interaction class handle。倘若此name不存在則回傳值0
    //* Input       : IChandle
    //* Output      : 回傳該class name所對應的handle值
    //*               0 = 不存在
    //* Algorithm   :
    //*************************************************************************
    unsigned long getInteractionClassHandle(char* ICname);
	RTI_InteractionClassHandle *getRTIInteractionClassHandle(char* ICname);

    //*************************************************************************
    //* Method 6    : getParameterName
    //* Description : 給定IChandle可得知此handle所對應的特定Interaction Class，本函式回
    //*               傳在特定Interaction Class底下Parameter handle所對應的Parameter name
    //* Input       : IChandle
    //*               parameter_handle
    //* Output      : parameter name
    //* Algorithm   :
    //*************************************************************************
    char* getParameterName(unsigned long IChandle,unsigned long parameter_handle);
	RTI_wstring *getRTIParameterName(unsigned long IChandle,unsigned long Parameter_handle);

    //*************************************************************************
    //* Method 7    : getParameterHandle
    //* Description : 給定IChandle可得知此handle所對應的特定Interatction Class，本函式回
    //*               傳在特定Interatction Class底下attribute name所對應的attribute handle
    //* Input       : IChandle
    //*               parameter_name
    //* Output      : parameter handle
    //* Algorithm   :
    //*************************************************************************
    unsigned long getParameterHandle(unsigned long IChandle,char* parameter_name);
	RTI_ParameterHandle *getRTIParameterHandle(unsigned long IChandle,char* parameter_name);

    //*************************************************************************
    //* Method 8    : getInteractionClass
    //* Description : 回傳指向obj T的指標 (所以必定滿足 T的Interaction Class Handle等於
    //*               IChandle值)
    //* Input       : IChandle
    //* Output      : T的指標
    //* Algorithm   :
    //*************************************************************************
    value_ptr getInteractionClass(unsigned long IChandle);

    //*************************************************************************
    //* Method 9    : getInheritedNumber
    //* Description : 回傳該Interaction Class至root這條path上所有的Node數量 (這些Node包含
    //*               root與IChandle)
    //* Input       : IChandle
    //* Output      : 回傳該Interaction Class至root這條path上所有的Node數量
    //* Algorithm   :
    //*************************************************************************
    unsigned long getInheritedNumber(unsigned long IChandle);

    //*************************************************************************
    //* Method 10   : isInteractionClass
    //* Description : 判斷此IChandle是否有對應到Interaction Class Tree中的其中一個Interaction Class
    //* Input       : IChandle
    //* Output      : True：有對應
    //*               False：無對應
    //* Algorithm   :
    //*************************************************************************
    bool isInteractionClass (unsigned long IChandle);

    //*************************************************************************
    //* Method 11   : isInteractionClass
    //* Description : 判斷此ICname是否有對應到Interaction Class Tree中的其中一個Interatction Class
    //* Input       : ICname
    //* Output      : True：有對應
    //*               False：無對應
    //* Algorithm   :
    //*************************************************************************
    bool isInteractionClass (char* ICname);

    //*************************************************************************
    //* Method 12   : isInheritedClassParameter
    //* Description : 在IChandle所對應的Interaction Class下，若此parameter_handle對應的
    //*               handle對應的parameter是繼承而來則回傳TRUE
    //* Input       : IChandle, ParameterHandle
    //* Output      : True,有對應
    //*               False,無對應
    //* Algorithm   :
    //*************************************************************************
    bool isInheritedClassParameter(unsigned long IChandle, unsigned long parameter_handle);

    //*************************************************************************
    //* Method 13   : getNumberOfParameter
    //* Description : 在IChandle對應Interaction Class之下，回傳該Interaction Class的
    //*               parameter的數量
    //* Input       : IChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfParameter(unsigned long IChandle);

    //*************************************************************************
    //* Method 14   : getNumberOfInheritedParameter
    //* Description : 回傳該Interaction Class繼承而來的Parameter之數量(不包含自己Object
    //*               Class新增的Parameter)
    //* Input       : IChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedParameter(unsigned long IChandle);

    //*************************************************************************
    //* Method 15   : getNumberOfInheritedClass
    //* Description : 回傳該Interaction Class至root這條path上所有的Node數量 (這些Node
    //*               不包IChandle)
    //* Input       : IChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedClass(unsigned long IChandle);

    //*************************************************************************
    //* Method 16   : getInteractionClassIterator
    //* Description : 回傳一個iterator指向代表"Interaction Class Handle所對應的Interaction
    //*               Class"之Node，譬如此Node _node型態為(Node*)為，那麼此函數在此時
    //*               應回傳 iterator(_node,_node);
    //* Input       : IChandle
    //* Output      : iterator
    //* Algorithm   :
    //*************************************************************************
	
    unsigned long getClassNodeCount();
    unsigned long getClassTailValue(unsigned long OChandle);
    unsigned long getClassParentNode(unsigned long OChandle);
    /*--------------------- Tree operation END --------------------*/


    /*---------------------- 單純紀錄Parsing的資訊 START-------------------------*/

    struct InteractionClassParsingInfo{
        InteractionClassParsingInfo()
        {
            name = 0;
			rti_name = NULL;
            sharing = 0;
            dimensions = 0;
            transportation = 0;
            order = 0;
            semantics = 0;
            parameterParsingInfoList = new parameterParsingInfo* [MAX_PARAMETER_NO];
        }
        //XML Parsing完後，對於每個InteractionClass的資訊，存放在此Structure

        char* name;
		RTI_wstring *rti_name;
        char* sharing;
        char* dimensions;
        char* transportation;
        char* order;
        char* semantics;
        parameterParsingInfo** parameterParsingInfoList;

    };

    struct parameterParsingInfo{
        parameterParsingInfo()
        {
            name = 0;
			rti_handle = NULL;
			rti_name = NULL;
            dataType = 0;
            semantics = 0;
        }
        //XML Parsing完後，對於每個parameter的資訊，存放在此Structure

        char* name;
		RTI_ParameterHandle *rti_handle;
		RTI_wstring *rti_name;
        char* dataType;
        char* semantics;
    };

    /*---------------------- 單純紀錄Parsing的資訊 END-------------------------*/


    struct Node{ //命名規則：Node裡的每個variable之命名，應以連續兩個底線為開頭
        Node()
        {
            __parsing_info = new InteractionClassParsingInfo;
            __child_no = 0;
            __child_List = new Node* [MAX_CHILD_NO];
            __parent = 0;
            __prev = 0;
            __next = 0;
            __forward = 0;
            __backward = 0;
            __biggest_handle = 0;
            __data = 0;
            __level = 0;
            __tail_value = 0;
            __handle = 0;
			rti_handle = NULL;
            __inherited_param_no=0;
        }

        unsigned long __handle;//此為Interatction Class Handle
		RTI_InteractionClassHandle *rti_handle;
		
        InteractionClassParsingInfo* __parsing_info; //此Node紀錄於XML上的資訊
        unsigned long __biggest_handle;//此InteractionClass中parameter handle最大值
        value_ptr __data; //value_type之型態為T,為我們日後保留之彈性

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
        unsigned long __inherited_param_no;
    };

private:
    unsigned long nodeCounter;
    Node** nodeArray;
    unsigned long Level;

    //辨識此段XML字串為何種資料
    int IdentifyNode(const char* p);
    const char* ParseInteractionClass(const char* , Node*);
    const char* ParseParameterNode(const char* , Node *,parameterParsingInfo *);
    const char* ParseParameter(const char* , XML_STRING* ,XML_STRING* );
    const char* ReadName(const char* , XML_STRING* );
    const char* ReadValue(const char* , Node *);
    const char* ReadText(const char* , XML_STRING *, bool , const char*, bool );
    const char* SkipTag(const char * );
    const char* SkipObjects(const char *);
    void  addNode(Node * parentNode, Node* childNode);
    void  addParameterInfo(Node * , parameterParsingInfo* );
    void addInheriteParameter(Node *);
    void assignTailValue();
    void treeReAllocation();

};
}

#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
