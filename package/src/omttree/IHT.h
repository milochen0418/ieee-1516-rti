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

    struct InteractionClassParsingInfo;//����InteractionClass��Parsing Infomation
    struct parameterParsingInfo; //����parameter��Parsing Infomation
    struct Node;

    typedef T value_type;
    typedef T* value_ptr;

    char* file_content; //�N����XML file���r��C
    unsigned long biggest_handle;//��Ӿ�interaction class handle���̤j��
    Node* root;



    IHT<T>();

    ~IHT<T>();



    /*------------------- Tree operation START -----------------*/

    
	/*
	 * Method 1		: read_XMLfile
	 * Description	: ��xml��Ū�Jfile_content�ܼƸ̡A�䬰���Achar*���ܼơC�YŪ������
	 *					�^��false
	 * Input		: fullPathFileName
	 * Output		: 
	 *				 True  : Ū��xml file���\
	 *				 False : Ū��xml file����
	 * Algorithm	:
	 */
    bool read_XMLfile(char* fullPathFileName);

	/*
	 * Method 2		: parsing
	 * Description	: �w��file_content�@����Interaction Class Table��parsing�A
	 *					�Yparsing���~�hreturn false
	 * Input		: �L
	 * Output		: 
	 *				 True  : ��R�ɮצ��\
	 *				 False : ��R�ɮץ���
	 * Algorithm	:
	 */
    bool parsing();

    //*************************************************************************
    //* Method 3    : isAncestorOf
    //* Description : ���զb�𵲺c���U IChandleA �O�_�� IChandleB������(Ancestor)
    //*               �YIChandleA�OIChandleB�������h�^��1
    //*               �YIChandleB�OIChandleA�������h�^��-1
    //*               �YIChandleA�PIChandleB�S���������Y�h�^��0
    //* Input       : IChandleA
    //*               IChandleB
    //* Output      :  1 :IChandleA�OIChandleB������
    //*               -1 :IChandleB�OIChandleA������
    //*                0 :IChandleA�PIChandleB�S���������Y
    //* Algorithm   :
    //*************************************************************************
    int isAncestorOf(unsigned long IChandleA, unsigned long IChandleB);

    //*************************************************************************
    //* Method 4    : getInteractionClassName
    //* Description : ���w����Interatction Class handle��OHT�A�z�L��function�i�o�쪺�^��
    //*               �Ȭ���handle�ҹ�����Interatction class name�C�խY��handle���s�b�h�^
    //*               �ǪŦr��"\n"
    //* Input       : IChandle
    //* Output      : �^�Ǹ�class handle�ҹ�����name
    //*               "\n" = ���s�b
    //* Algorithm   :
    //*************************************************************************
    char* getInteractionClassName(unsigned long IChandle);
	RTI_wstring* getRTIInteractionClassName(unsigned long IChandle);

    //*************************************************************************
    //* Method 5    : getInteractionClassHandle
    //* Description : ���w����Interaction Class name��OHT�A�z�L��function�i�o�쪺�^�ǭ�
    //*               ����name�ҹ�����Interaction class handle�C�խY��name���s�b�h�^�ǭ�0
    //* Input       : IChandle
    //* Output      : �^�Ǹ�class name�ҹ�����handle��
    //*               0 = ���s�b
    //* Algorithm   :
    //*************************************************************************
    unsigned long getInteractionClassHandle(char* ICname);
	RTI_InteractionClassHandle *getRTIInteractionClassHandle(char* ICname);

    //*************************************************************************
    //* Method 6    : getParameterName
    //* Description : ���wIChandle�i�o����handle�ҹ������S�wInteraction Class�A���禡�^
    //*               �Ǧb�S�wInteraction Class���UParameter handle�ҹ�����Parameter name
    //* Input       : IChandle
    //*               parameter_handle
    //* Output      : parameter name
    //* Algorithm   :
    //*************************************************************************
    char* getParameterName(unsigned long IChandle,unsigned long parameter_handle);
	RTI_wstring *getRTIParameterName(unsigned long IChandle,unsigned long Parameter_handle);

    //*************************************************************************
    //* Method 7    : getParameterHandle
    //* Description : ���wIChandle�i�o����handle�ҹ������S�wInteratction Class�A���禡�^
    //*               �Ǧb�S�wInteratction Class���Uattribute name�ҹ�����attribute handle
    //* Input       : IChandle
    //*               parameter_name
    //* Output      : parameter handle
    //* Algorithm   :
    //*************************************************************************
    unsigned long getParameterHandle(unsigned long IChandle,char* parameter_name);
	RTI_ParameterHandle *getRTIParameterHandle(unsigned long IChandle,char* parameter_name);

    //*************************************************************************
    //* Method 8    : getInteractionClass
    //* Description : �^�ǫ��Vobj T������ (�ҥH���w���� T��Interaction Class Handle����
    //*               IChandle��)
    //* Input       : IChandle
    //* Output      : T������
    //* Algorithm   :
    //*************************************************************************
    value_ptr getInteractionClass(unsigned long IChandle);

    //*************************************************************************
    //* Method 9    : getInheritedNumber
    //* Description : �^�Ǹ�Interaction Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node�]�t
    //*               root�PIChandle)
    //* Input       : IChandle
    //* Output      : �^�Ǹ�Interaction Class��root�o��path�W�Ҧ���Node�ƶq
    //* Algorithm   :
    //*************************************************************************
    unsigned long getInheritedNumber(unsigned long IChandle);

    //*************************************************************************
    //* Method 10   : isInteractionClass
    //* Description : �P�_��IChandle�O�_��������Interaction Class Tree�����䤤�@��Interaction Class
    //* Input       : IChandle
    //* Output      : True�G������
    //*               False�G�L����
    //* Algorithm   :
    //*************************************************************************
    bool isInteractionClass (unsigned long IChandle);

    //*************************************************************************
    //* Method 11   : isInteractionClass
    //* Description : �P�_��ICname�O�_��������Interaction Class Tree�����䤤�@��Interatction Class
    //* Input       : ICname
    //* Output      : True�G������
    //*               False�G�L����
    //* Algorithm   :
    //*************************************************************************
    bool isInteractionClass (char* ICname);

    //*************************************************************************
    //* Method 12   : isInheritedClassParameter
    //* Description : �bIChandle�ҹ�����Interaction Class�U�A�Y��parameter_handle������
    //*               handle������parameter�O�~�ӦӨӫh�^��TRUE
    //* Input       : IChandle, ParameterHandle
    //* Output      : True,������
    //*               False,�L����
    //* Algorithm   :
    //*************************************************************************
    bool isInheritedClassParameter(unsigned long IChandle, unsigned long parameter_handle);

    //*************************************************************************
    //* Method 13   : getNumberOfParameter
    //* Description : �bIChandle����Interaction Class���U�A�^�Ǹ�Interaction Class��
    //*               parameter���ƶq
    //* Input       : IChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfParameter(unsigned long IChandle);

    //*************************************************************************
    //* Method 14   : getNumberOfInheritedParameter
    //* Description : �^�Ǹ�Interaction Class�~�ӦӨӪ�Parameter���ƶq(���]�t�ۤvObject
    //*               Class�s�W��Parameter)
    //* Input       : IChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedParameter(unsigned long IChandle);

    //*************************************************************************
    //* Method 15   : getNumberOfInheritedClass
    //* Description : �^�Ǹ�Interaction Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node
    //*               ���]IChandle)
    //* Input       : IChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedClass(unsigned long IChandle);

    //*************************************************************************
    //* Method 16   : getInteractionClassIterator
    //* Description : �^�Ǥ@��iterator���V�N��"Interaction Class Handle�ҹ�����Interaction
    //*               Class"��Node�AĴ�p��Node _node���A��(Node*)���A���򦹨�Ʀb����
    //*               ���^�� iterator(_node,_node);
    //* Input       : IChandle
    //* Output      : iterator
    //* Algorithm   :
    //*************************************************************************
	
    unsigned long getClassNodeCount();
    unsigned long getClassTailValue(unsigned long OChandle);
    unsigned long getClassParentNode(unsigned long OChandle);
    /*--------------------- Tree operation END --------------------*/


    /*---------------------- ��¬���Parsing����T START-------------------------*/

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
        //XML Parsing����A���C��InteractionClass����T�A�s��b��Structure

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
        //XML Parsing����A���C��parameter����T�A�s��b��Structure

        char* name;
		RTI_ParameterHandle *rti_handle;
		RTI_wstring *rti_name;
        char* dataType;
        char* semantics;
    };

    /*---------------------- ��¬���Parsing����T END-------------------------*/


    struct Node{ //�R�W�W�h�GNode�̪��C��variable���R�W�A���H�s���ө��u���}�Y
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

        unsigned long __handle;//����Interatction Class Handle
		RTI_InteractionClassHandle *rti_handle;
		
        InteractionClassParsingInfo* __parsing_info; //��Node������XML�W����T
        unsigned long __biggest_handle;//��InteractionClass��parameter handle�̤j��
        value_ptr __data; //value_type�����A��T,���ڭ̤��O�d���u��

        unsigned long __level;
        //�Y�O�ϥ� sibiling��{�i��|���H�U���
        //Node* __parent;
        //Node* __child;
        Node* __prev;
        Node* __next;
        Node* __forward;
        Node* __backward;
        //�Y�O�ϥ� �@��h��������{��k�A�i��|���H�U���
        Node* __parent;
        Node** __child_List;
        int   __child_no;   //�O���ثe���X��child Node
        //int   __attrInfoNo;
        unsigned long __tail_value;
        unsigned long __inherited_param_no;
    };

private:
    unsigned long nodeCounter;
    Node** nodeArray;
    unsigned long Level;

    //���Ѧ��qXML�r�ꬰ��ظ��
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
