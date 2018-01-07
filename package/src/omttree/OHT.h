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
    struct      ObjectClassParsingInfo; //����ObjectClass��Parsing Infomation
    struct      attributeParsingInfo;   //����attribute��Parsing Infomation
    struct      Node;
	
    typedef     T value_type;
    typedef     T* value_ptr;

    char*               file_content; //�N����XML file���r��C
    unsigned long       biggest_handle;//��Ӿ�object class handle���̤j��
    Node*               root;
    
	OHT();


    //Destructor
    ~OHT();

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
	 * Description	: �w��file_contentdd��Interaction Class Tree�@parsing�A�Y
	 *					parsing���~�hreturn false�A�YŪ�����ѫhreturn FALSE
	 * Input		: �L
	 * Output		: 
	 *				 True  : ��R�ɮצ��\
	 *				 False : ��R�ɮץ���
	 * Algorithm	:
	 */
    bool parsing();


    //*************************************************************************
    //* Method 3    : isAncestorOf
    //* Description : ���զb�𵲺c���U OChandleA �O�_�� OChandleB������(Ancestor)
    //*               �YOChandleA�OOChandleB�������h�^��1
    //*               �YOChandleB�OOChandleA�������h�^��-1
    //*               �YOChandleA�POChandleB�S���������Y�h�^��0
    //* Input       : OChandleA
    //*               OChandleB
    //* Output      :  1 :OChandleA�OOChandleB������
    //*               -1 :OChandleB�OOChandleA������
    //*                0 :OChandleA�POChandleB�S���������Y
    //* Algorithm   :
    //*************************************************************************
    int isAncestorOf(unsigned long OChandleA, unsigned long OChandleB);

    //*************************************************************************
    //* Method 4    : getObjectClassName
    //* Description : ���w����Object Class handle��OHT�A�z�L��function�i�o�쪺�^��
    //*               �Ȭ���handle�ҹ�����object class name�C�խY��handle���s�b�h�^
    //*               �ǪŦr�ꡨ\n��
    //* Input       : OChandle
    //* Output      : object class name
    //* Algorithm   :
    //*************************************************************************
    char* getObjectClassName(unsigned long OChandle);
	RTI_wstring* getRTIObjectClassName(unsigned long OChandle);



    //*************************************************************************
    //* Method 5    : getObjectClassHandle
    //* Description : ���w����Object Class name��OHT�A�z�L��function�i�o�쪺�^�ǭ�
    //*               ����name�ҹ�����object class handle�C�խY��name���s�b�h�^�ǭ�0
    //* Input       : OCname
    //* Output      : object class handle
    //*               0 = ���s�b
    //* Algorithm   :
    //*************************************************************************
    unsigned long getObjectClassHandle(char* OCname);
	RTI_ObjectClassHandle *getRTIObjectClassHandle(char *OCname);

    //*************************************************************************
    //* Method 6    : getAttributeName
    //* Description : ���wOChandle�i�o����handle�ҹ������S�wObject Class�A���禡�^
    //*               �Ǧb�S�wObject Class���Uattribute handle�ҹ�����attribute name
    //* Input       : OCname
    //*             attribute_handle
    //* Output      : attribute name
    //* Algorithm   :
    //*************************************************************************
    char* getAttributeName(unsigned long OChandle,unsigned long attribute_handle);
	RTI_wstring* getRTIAttributeName(unsigned long OChandle,unsigned long attribute_handle);

    //*************************************************************************
    //* Method 7    : getAttributeHandle
    //* Description : ���wOChandle�i�o����handle�ҹ������S�wObject Class�A���禡�^
    //*               �Ǧb�S�wObject Class���Uattribute name�ҹ�����attribute handle
    //* Input       : OChandleS
    //*               attribute_name
    //* Output      : attribute handle
    //* Algorithm   :
    //*************************************************************************
    unsigned long getAttributeHandle(unsigned long OChandle,char* attribute_name);
	RTI_AttributeHandle *getRTIAttributeHandle(unsigned long OChandle,char* attribute_name);

    //*************************************************************************
    //* Method 8    : getObjectClass
    //* Description : �^�ǫ��Vobj T������ (�ҥH���w���� T��Object Class Handle����
    //*               OChandle��)
    //* Input       : OChandle
    //* Output      : T������
    //* Algorithm   :
    //*************************************************************************
    value_ptr getObjectClass(unsigned long OChandle);

    //*************************************************************************
    //* Method 9    : getInheritedNumber
    //* Description : �^�Ǹ�Object Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node�]�t
    //*               root�POChandle)
    //* Input       : OChandle
    //* Output      : �^�Ǹ�Object Class��root�o��path�W�Ҧ���Node�ƶq
    //* Algorithm   :
    //*************************************************************************
    unsigned long getInheritedNumber(unsigned long OChandle);

    //*************************************************************************
    //* Method 10   : isObjectClass
    //* Description : �P�_��OChandle�O�_��������Object Class Tree�����䤤�@��Object Class
    //* Input       : isObjectClass
    //* Output      : True�G������
    //*               False�G�L����
    //* Algorithm   :
    //*************************************************************************
    bool isObjectClass (unsigned long OChandle);


    //*************************************************************************
    //* Method 11   : isObjectClass
    //* Description : �P�_��OCname�O�_��������Object Class Tree�����䤤�@��Object Class
    //* Input       : OChandle
    //* Output      : True�G������
    //*              False�G�L����
    //* Algorithm   :
    //*************************************************************************
    bool isObjectClass (char* OCname);

    //*************************************************************************
    //* Method 12   : isInheritedClassAttribute
    //* Description : �bOChandle�ҹ�����Object Class�U�A�Y��attributeHandle������
    //*               attribute�O�~�ӦӨӪ�iff�^��TRUE
    //* Input       : OChandle, attributeHandle
    //* Output      : True,������
    //*               False,�L����
    //* Algorithm   :
    //*************************************************************************
    bool isInheritedClassAttribute(unsigned long OChandle, unsigned long attributeHandle);
    //

    //*************************************************************************
    //* Method 13   : getNumberOfAttribute
    //* Description : �bOChandle����Object Class���U�A�^�Ǹ�Object Class��attriubte���ƶq
    //* Input       : OChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfAttribute(unsigned long OChandle);

    //*************************************************************************
    //* Method 14   : getNumberOfInheritedAttribute
    //* Description : �^�Ǹ�Object Class�~�ӦӨӪ�attribute���ƶq(���]�t�ۤvObject
    //*               Class�s�W��attribute)
    //* Input       : OChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedAttribute(unsigned long OChandle);
    //

    //*************************************************************************
    //* Method 15   : getNumberOfInheritedClass
    //* Description : �^�Ǹ�Object Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node���]�tOChandle)
    //* Input       : OChandle
    //* Output      : unsigned long
    //* Algorithm   :
    //*************************************************************************
    unsigned long getNumberOfInheritedClass(unsigned long OChandle);

	unsigned long getClassNodeCount();

	unsigned long getClassTailValue(unsigned long OChandle);

	unsigned long getClassParentNode(unsigned long OChandle);
	
	
    /*--------------------- Tree operation END --------------------*/
    /*---------------------- ��¬���Parsing����T START-------------------------*/
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
        //XML Parsing����A���C��ObjectClass����T�A�s��b��Structure
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
        //XML Parsing����A���C��attribute����T�A�s��b��Structure
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
    /*---------------------- ��¬���Parsing����T END-------------------------*/



    struct Node{ //�R�W�W�h�GNode�̪��C��variable���R�W�A���H�s���ө��u���}�Y
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

        unsigned long   __handle;//����Object Class Handle
		RTI_ObjectClassHandle *rti_handle;
		
        ObjectClassParsingInfo* __parsing_info; //��Node������XML�W����T
        unsigned long __biggest_handle;//��ObjectClass��attribute handle�̤j��
        value_ptr       __data; //value_type�����A��T,���ڭ̤��O�d���u��
        //XmlCursor __location;
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
        unsigned long __inherited_attr_no;

    };
    

private:
	unsigned long nodeCounter;
    Node** nodeArray;
    unsigned long Level;

    //���Ѧ��qXML�r�ꬰ��ظ��
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
