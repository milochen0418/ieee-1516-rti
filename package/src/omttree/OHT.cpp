#ifndef OHT_CPP
#define OHT_CPP
/***************************************************************************
 *            oht.cpp
 *
 *  Tue Sep 19 01:55:23 2006
 *  Copyright  2006  Bruce Wang
 *  cwwang2@gmail.com
 ****************************************************************************/

#include <cstdio>
#include <malloc.h>
#include <assert.h>
#include <iostream>
#include "globle.h"
#include "OHT.h"

static Entity entity[ NUM_ENTITY ];
static bool condenseWhiteSpace;

namespace omt{

/////////////////////////////////////////////////////////////////////////////////////
// String helper function
/////////////////////////////////////////////////////////////////////////////////////
static bool StringEqual( const char* p, const char* tag, bool ignoreCase )
{

    if ( !p || !*p )
    {
        return false;
    }

    const char* q = p;

    if ( ignoreCase )
    {
        while ( *q && *tag && tolower(*q) == tolower(*tag))
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )
            return true;
    }
    else
    {
        while ( *q && *tag && *q == *tag )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )                // Have we found the end of the tag, and everything equal?
            return true;
    }
    return false;
}

static bool IsWhiteSpace( char c )
{
    return ( isspace( (unsigned char) c ) || c == '\n' || c == '\r' );
}

static bool IsWhiteSpace( int c )
{
    if ( c < 256 )
        return IsWhiteSpace( (char) c );
    return false;   // Again, only truly correct for English/Latin...but usually works.
}

static const char* SkipWhiteSpace( const char* p )
{
    if ( !p || !*p )
    {
        return 0;
    }

    while ( *p && IsWhiteSpace( *p ) || *p == '\n' || *p =='\r' )
        ++p;

    return p;
}

static int IsAlphaNum(unsigned char anyByte)
{
    if ( anyByte < 127 )
        return isalnum( anyByte );
    else
        return 1;
}


static const char* GetEntity( const char* p, char* value, int* length )
{
    // Presume an entity, and pull it out.
    int i;
    *length = 0;

    if ( *(p+1) && *(p+1) == '#' && *(p+2) )
    {
        unsigned long ucs = 0;
        ptrdiff_t delta = 0;
        unsigned mult = 1;

        if ( *(p+2) == 'x' )
        {
            // Hexadecimal.
            if ( !*(p+3) ) return 0;

            const char* q = p+3;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != 'x' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else if ( *q >= 'a' && *q <= 'f' )
                    ucs += mult * (*q - 'a' + 10);
                else if ( *q >= 'A' && *q <= 'F' )
                    ucs += mult * (*q - 'A' + 10 );
                else
                    return 0;
                mult *= 16;
                --q;
            }
        }
        else
        {
            // Decimal.
            if ( !*(p+2) ) return 0;

            const char* q = p+2;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != '#' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else
                    return 0;
                mult *= 10;
                --q;
            }
        }
        *value = (char)ucs;
        *length = 1;

        return p + delta + 1;
    }

    // Now try to match it.
    for ( i=0; i<NUM_ENTITY; ++i )
    {
        if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 )
        {
            *value = entity[i].chr;
            *length = 1;
            return ( p + entity[i].strLength );
        }
    }

    // So it wasn't an entity, its unrecognized, or something like that.
    *value = *p;    // Don't put back the last one, since we return it!
    //*length = 1;  // Leave unrecognized entities - this doesn't really work.
    // Just writes strange XML.
    return p+1;
}

static const char* GetChar(const char* p, char* _value, int* length)
{

    //for none UTF-8 char
    *length = 1;

    if ( *length == 1 )
    {
        if ( *p == '&' )
            return GetEntity( p, _value, length);
        *_value = *p;
        return p+1;
    }
    else if ( *length )
    {
        // lots of compilers don't like this function (unsafe),
        // and the null terminator isn't needed
        for ( int i=0; p[i] && i<*length; ++i )
        {
            _value[i] = p[i];
        }
        return p + (*length);
    }
    else
    {
        // Not valid text.
        return 0;
    }
}

template <typename T>
OHT<T>::OHT()
{
    nodeCounter = 0;
    root = 0;
    file_content = 0;
    biggest_handle = 0;
    Level = 0;
    nodeArray = new Node*[MAX_NODE_NO];
    for(int i = 0; i <= MAX_NODE_NO; i++)
    {
        nodeArray[i] = 0;
    }
}

template <typename T>
OHT<T>::~OHT()
{

    //std::cout <<"OHT deconstructor..."<<std::endl;

    Node *pNode;
    Node *pChildNode;
    attributeParsingInfo *pAttrInfo;
    int childNo;
    int biggest_handle;
    int inherited_attr_no;
    if(nodeCounter != 0)
    {
        for(int i = nodeCounter ; i >= 1; i--)
        {
            //std::cout << "delete Node..."<< i << std::endl;
            pNode = nodeArray[i];

            childNo = pNode->__child_no;

            biggest_handle = pNode->__biggest_handle;
            inherited_attr_no =  pNode->__inherited_attr_no;

            for(int k = inherited_attr_no+1; k <= biggest_handle ; k++)
            {

                //�R��attributeParsingInfo�Ψ�r��
                pAttrInfo = pNode->__parsing_info->attributeParsingInfoList[k];
                //std::cout << "delete attributeParsingInfo..."<< k << ",name = "<< pAttrInfo->name <<std::endl;
                delete [] pAttrInfo->dataType;
                delete [] pAttrInfo->dimensions;
                delete [] pAttrInfo->name;
		if (pAttrInfo->rti_handle != NULL)
		    delete pAttrInfo->rti_handle;
		delete pAttrInfo->rti_name;
                delete [] pAttrInfo->order;
                delete [] pAttrInfo->ownership;
                delete [] pAttrInfo->semantics;
                delete [] pAttrInfo->sharing;
                delete [] pAttrInfo->transportation;
                delete [] pAttrInfo->updateCondition;
                delete [] pAttrInfo->updateConditionNotes;
                delete [] pAttrInfo->updateType;

                delete [] pAttrInfo;
            }
            //std::cout << "delete ObjectClassParsignInfo..."<< i << std::endl;
            delete [] pNode->__parsing_info->attributeParsingInfoList;
            delete [] pNode->__parsing_info->name;
	    delete pNode->__parsing_info->rti_name;
            delete [] pNode->__parsing_info->semantics;
            delete [] pNode->__parsing_info->sharing;

            delete [] pNode->__child_List;

            delete pNode;
        }
        delete  [] nodeArray;
    }

}

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
//* Time complexity : O(1)
//**********************************************************************

template <typename T>
int OHT<T>::isAncestorOf(unsigned long OChandleA, unsigned long OChandleB)
{
    if(OChandleA <= 0 || OChandleB <= 0 || OChandleA > nodeCounter || OChandleB > nodeCounter)
        return 0;
    else
        // if(OChandleA != OChandleB)
    {
        //OCHandleB��Haldle�bOC HandleA��Interval����
        if((OChandleA < OChandleB) && (nodeArray[OChandleA]->__tail_value >= OChandleB))
        {
            return 1;
        }//OCHandleA��Haldle�bOC HandleB��Interval����
        else if((OChandleB < OChandleA) && (nodeArray[OChandleB]->__tail_value >= OChandleA))
        {
            return -1;
        }
        else //�YOChandleA�POChandleB���ۦPObjectClass�S���������Y
            return 0;
    }
}

//*************************************************************************
//* Method 4    : getObjectClassName
//* Description : ���w����Object Class handle��OHT�A�z�L��function�i�o�쪺�^��
//*               �Ȭ���handle�ҹ�����object class name�C�խY��handle���s�b�h�^
//*               �ǪŦr�ꡨ\n��
//* Input       : OChandle
//* Output      : object class name
//* Algorithm : ���w����Object Class handle��OHT�A�z�L��function�i�o�쪺�^��
//*             �Ȭ���handle�ҹ�����object class name�C�խY��handle���s�b�h�^
//*             �ǪŦr�ꡨ\n��
//* Time complexity : O(1)
//**********************************************************************
template <typename T>
char* OHT<T>::getObjectClassName(unsigned long OChandle)
{        //childCounter++;
    if(OChandle > 0 && OChandle <= nodeCounter)
        return nodeArray[OChandle]->__parsing_info->name;
    else
        return "\n";
}

template <typename T>
RTI_wstring* OHT<T>::getRTIObjectClassName(unsigned long OChandle)
{        //childCounter++;
    if(OChandle > 0 && OChandle <= nodeCounter)
        return nodeArray[OChandle]->__parsing_info->rti_name;
    else
        return NULL;
}


//*************************************************************************
//* Method 5    : getObjectClassHandle
//* Description : ���w����Object Class name��OHT�A�z�L��function�i�o�쪺�^�ǭ�
//*               ����name�ҹ�����object class handle�C�խY��name���s�b�h�^�ǭ�0
//* Input       : OCname
//* Output      : object class handle
//*               0 = ���s�b
//* Algorithm : ���w����Object Class name��OHT�A�z�L��function�i�o�쪺�^�ǭ�
//*             ����name�ҹ�����object class handle�C�խY��name���s�b�h�^�ǭ�
//*             0�C
//* Time complexity : O(n)
//**********************************************************************

template <typename T>
unsigned long OHT<T>::getObjectClassHandle(char* OCname)
{
    if(OCname == NULL)
        return 0;
    for(int i = 1; i<= nodeCounter; i++)
    {
        if(!strcmp(nodeArray[i]->__parsing_info->name, OCname))
            return nodeArray[i]->__handle;
    }

    return 0;
}

template <typename T>
RTI_ObjectClassHandle *OHT<T>::getRTIObjectClassHandle(char *OCname)
{
    if(OCname == NULL)
        return 0;
    for(int i = 1; i<= nodeCounter; i++)
    {
        if(!strcmp(nodeArray[i]->__parsing_info->name, OCname))
            return nodeArray[i]->rti_handle;
    }

    return 0;
}

//*************************************************************************
//* Method 6    : getAttributeName
//* Description : ���wOChandle�i�o����handle�ҹ������S�wObject Class�A���禡�^
//*               �Ǧb�S�wObject Class���Uattribute handle�ҹ�����attribute name
//* Input       : OCname
//*             attribute_handle
//* Output      : attribute name
//* Algorithm : ���wOChandle�i�o����handle�ҹ������S�wObject Class�A���禡�^
//*             �Ǧb�S�wObject Class���Uattribute handle�ҹ�����attribute
//*             name�C
//* Time complexity : O(1)
//**********************************************************************

template <typename T>
char* OHT<T>::getAttributeName(unsigned long OChandle,unsigned long attribute_handle)
{

    if(OChandle <= 0 || attribute_handle <= 0)
        return "\n";
    if(OChandle <= nodeCounter && (attribute_handle <= nodeArray[OChandle]->__biggest_handle))
        return nodeArray[OChandle]->__parsing_info->attributeParsingInfoList[attribute_handle]->name;
    else
        return "\n";
}

template <typename T>
RTI_wstring* OHT<T>::getRTIAttributeName(unsigned long OChandle,unsigned long attribute_handle)
{

    if(OChandle <= 0 || attribute_handle <= 0)
        return NULL;
    if(OChandle <= nodeCounter && (attribute_handle <= nodeArray[OChandle]->__biggest_handle))
        return nodeArray[OChandle]->__parsing_info->attributeParsingInfoList[attribute_handle]->rti_name;
    else
        return NULL;
}

//*************************************************************************
//* Method 7    : getAttributeHandle
//* Description : ���wOChandle�i�o����handle�ҹ������S�wObject Class�A���禡�^
//*               �Ǧb�S�wObject Class���Uattribute name�ҹ�����attribute handle
//* Input       : OChandleS
//*               attribute_name
//* Output      : attribute handle
//* Algorithm : ���wOChandle�i�o����handle�ҹ������S�wObject Class�A���禡�^
//*             �Ǧb�S�wObject Class���Uattribute name�ҹ�����attribute
//*             handle�C
//* Time complexity : O(n), n���ݩʼƶq
//**********************************************************************

template <typename T>
unsigned long OHT<T>::getAttributeHandle(unsigned long OChandle,char* attribute_name)
{
    Node *pNode;
    attributeParsingInfo *pAttrInfo;

    if((OChandle > 0 && OChandle <= nodeCounter) && attribute_name != NULL)
    {
        pNode = nodeArray[OChandle];
        for(int i = 1; i<= pNode->__biggest_handle; i++)
        {
            pAttrInfo = pNode->__parsing_info->attributeParsingInfoList[i];
            //strcmp����ۦP��0
            if (strcmp(pAttrInfo->name, attribute_name) == 0)
                return i;
        }
    }
    return 0;

}

template <typename T>
RTI_AttributeHandle *OHT<T>::getRTIAttributeHandle(unsigned long OChandle,char* attribute_name)
{
    Node *pNode;
    attributeParsingInfo *pAttrInfo;

    if((OChandle > 0 && OChandle <= nodeCounter) && attribute_name != NULL)
    {
        pNode = nodeArray[OChandle];
        for(int i = 1; i<= pNode->__biggest_handle; i++)
        {
            pAttrInfo = pNode->__parsing_info->attributeParsingInfoList[i];
            //strcmp����ۦP��0
            if (strcmp(pAttrInfo->name, attribute_name) == 0)
	    {
		if (pAttrInfo->rti_handle == NULL)
		    pAttrInfo->rti_handle = new RTI_AttributeHandle(i);

                return pAttrInfo->rti_handle;
	    }
        }
    }
    return 0;

}

///*************************************************************************
//* Method 8    : getObjectClass
//* Description : �^�ǫ��Vobj T������ (�ҥH���w���� T��Object Class Handle����
//*               OChandle��)
//* Input       : OChandle
//* Output      : T������
//* Algorithm :
//* Time complexity : O(1)
//**********************************************************************
template <typename T>
typename OHT<T>::value_ptr OHT<T>::getObjectClass(unsigned long OChandle)
{
    if(OChandle > 0 && OChandle <= nodeCounter)
    {
        return nodeArray[OChandle]->__data;
    }
    else
        return NULL;
}

//*************************************************************************
//* Method 9    : getInheritedNumber
//* Description : �^�Ǹ�Object Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node�]�t
//*               root�POChandle)
//* Input       : OChandle
//* Output      : �^�Ǹ�Object Class��root�o��path�W�Ҧ���Node�ƶq
//* Algorithm :
//**********************************************************************
template <typename T>
unsigned long OHT<T>::getInheritedNumber(unsigned long OChandle)
{
    unsigned long tailValue;
    unsigned long counter = 0;
    if(OChandle > 0 && OChandle <= nodeCounter)
    {

        tailValue = nodeArray[OChandle]->__tail_value;
        //for(int i = OChandle; i <= tailValue; i++)
        //{
        //    counter++;
        //}
        if (tailValue > 0)
            counter = (tailValue - OChandle + 1);
    }
    return counter;
}

//*************************************************************************
//* Method 10   : isObjectClass
//* Description : �P�_��OCname�O�_��������Object Class Tree�����䤤�@��Object Class
//* Input       : isObjectClass
//* Output      : True�G������
//*               False�G�L����
//* Algorithm :
//* Time complexity : O(n)
//**********************************************************************
template <typename T>
bool OHT<T>::isObjectClass ( char* OCname)
{
    if (OCname == NULL)
        return false;
    //��DFS�覡�j�M�W��
    for(int i = 1; i<= nodeCounter; i++)
    {
        if(!strcmp(nodeArray[i]->__parsing_info->name, OCname))
            return true;
    }
    return false;
}

//*************************************************************************
//* Method 11   : isObjectClass
//* Description : �P�_��OChandle�O�_��������Object Class Tree�����䤤�@��Object Class
//* Input       : OChandle
//* Output      : True�G������
//*              False�G�L����
//* Algorithm :
//* Time complexity : O(1)
//**********************************************************************
template <typename T>
bool OHT<T>::isObjectClass (unsigned long OChandle)
{
    if(OChandle > 0 && OChandle <= nodeCounter)
        return true;
    else
        return false;

}

//*************************************************************************
//* Method 12   : isInheritedClassAttribute
//* Description : �bOChandle�ҹ�����Object Class�U�A�Y��attributeHandle������
//*               attribute�O�~�ӦӨӪ�iff�^��TRUE
//* Input       : OChandle, attributeHandle
//* Output      : True,������
//*               False,�L����
//* Algorithm :
//**********************************************************************
template <typename T>
bool OHT<T>::isInheritedClassAttribute(unsigned long OChandle, unsigned long attributeHandle)
{
    if((OChandle > 0 && OChandle <= nodeCounter))
    {
        if (attributeHandle > 0 && attributeHandle <= nodeArray[OChandle]->__biggest_handle)
        {
            if(nodeArray[OChandle]->__biggest_handle > 0)
            {
                //attributeHandle��inherite attribute handle�j
                //�B�p�󵥩�̤j��attribute handle
                if( attributeHandle <= nodeArray[OChandle]->__biggest_handle &&
                        attributeHandle > nodeArray[OChandle]->__inherited_attr_no)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//*************************************************************************
//* Method 13   : getNumberOfAttribute
//* Description : �bOChandle����Object Class���U�A�^�Ǹ�Object Class��attriubte���ƶq
//* Input       : OChandle
//* Output      : unsigned long
//* Algorithm :
//**********************************************************************
template <typename T>
unsigned long OHT<T>::getNumberOfAttribute(unsigned long OChandle)
{
    if(OChandle > 0 && OChandle <= nodeCounter)
    {
        return (nodeArray[OChandle]->__biggest_handle);
    }
    else
        return 0;
}
//*************************************************************************
//* Method 14   : getNumberOfInheritedAttribute
//* Description : �^�Ǹ�Object Class�~�ӦӨӪ�attribute���ƶq(���]�t�ۤvObject
//*               Class�s�W��attribute)
//* Input       : OChandle
//* Output      : unsigned long
//* Algorithm   :
//*************************************************************************
template <typename T>
unsigned long OHT<T>::getNumberOfInheritedAttribute(unsigned long OChandle)
{
    if(OChandle > 0 && OChandle <= nodeCounter)
        return (nodeArray[OChandle]->__inherited_attr_no);
    else
        return 0;
}

//*************************************************************************
//* Method 15   : getNumberOfInheritedClass
//* Description : �^�Ǹ�Object Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node���]�tOChandle)
//* Input       : OChandle
//* Output      : unsigned long
//* Algorithm   :
//*************************************************************************
template <typename T>
unsigned long OHT<T>::getNumberOfInheritedClass(unsigned long OChandle)
{
    //unsigned long tailValue;
    //unsigned long counter = 0;
    //tailValue = (nodeArray[OChandle]->__tail_value);

    //�q�l�`�I�}�l��l�𪺳̫�@�Ӹ`�I
    //for(int i = (OChandle + 1); i <= tailValue; i++)
    //{
    //  counter++;
    //}
    //return counter;
    if(OChandle > 0 && OChandle <= nodeCounter)
        return ((nodeArray[OChandle]->__level) - 1);
    else
        return 0;

}
template <typename T>
unsigned long OHT<T>::getClassNodeCount()
{
	return nodeCounter;
}

template <typename T>
unsigned long OHT<T>::getClassTailValue(unsigned long OChandle)
{
	return nodeArray[OChandle]->__tail_value;
}
template <typename T>
unsigned long OHT<T>::getClassParentNode(unsigned long OChandle)
{
	return nodeArray[OChandle]->__parent->__handle;
}

template<typename T>
int OHT<T>::IdentifyNode(const char* p)
//int IdentifyNode(const char* p)
{
    //std::cout << "in IdentifyNode... => " << *p << " LINE : "<<__LINE__ <<std::endl;
    const char* xmlHeader = { "<?xml" };
    const char* commentHeader = { "<!--" };
    const char* dtdHeader = { "<!" };

    int nodeType = 0;

    p = SkipWhiteSpace( p );
    if( !p || !*p || *p != '<' )//���_�l�A�ư��@��L��ܦr���r����A�����n�nŪ��'<'�A�_�h�N�^�� type_error
    {
        //std::cout << "type error => " << *p << std::endl;
        return type_error;
    }

    if ( StringEqual(p, xmlHeader, true) )//�ݥثetape�W�O�_���V"<?xml"�o�Ӥl�r�ꪺ�_�l��m�A�j�p�g����(���A�Pa���ۦP)�C
    {					//�i�J���{���϶��Y�N��"<?xml"�T��O�ثetape�ҫ����a��
        // found XML Header
        p = SkipWhiteSpace(p);

        if ( !p || !*p || !StringEqual(p, "<?xml", true) )//�p�G�b<?xml����A���F�ťդS�J���F<?xml�h�b�H�U�{���϶��^��type_error
        {
            // Not a valid XML format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_xmlHeader;
    }
    else if ( StringEqual(p, commentHeader, false) )//�p�G�ثetape�O���V�b<!--�W����(�j�p�g�n�Ϥ�)
    {
        if ( !p || !*p || !StringEqual(p, "<!--", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_commentHeader;
    }
    else if ( StringEqual( p, dtdHeader, false) )
    {
        if ( !p || !*p || !StringEqual(p, "<!", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_dtdHeader;
    }
    else if ( StringEqual( p, "<objectModel", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<objectModel", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_objectModel;
    }
    else if ( StringEqual( p, "<objects", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<objects", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_objectCollection;
    }
    else if ( StringEqual( p, "<attribute", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<attribute", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_attribute;
    }
    else if ( StringEqual( p, "<interactions", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<interactions", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_attribute;
    }
    else if ( isalpha(*(p+1)) || *(p+1) == '_')
    {
        //std::cout << "find a node => " << *p << "type_node => " << type_objectclass << std::endl;
        nodeType = type_objectclass;
    }
    else
    {
        // Unknown
        //std::cout << "type_unknown => " << *p << std::endl;
        nodeType = type_unknown;
    }

    return nodeType;
}

template<typename T>
const char* OHT<T>::SkipTag(const char * p)
{

    XML_STRING value;

    if ( *p != '<' )
    {
        return 0;
    }

    p = SkipWhiteSpace(p+1);
    p = ReadName(p, &value);
    if ( !p || !*p )
    {
        return 0;
    }

    XML_STRING endTag ("</");
    endTag += value;
    endTag += ">";
    while ( p && *p )
    {

        if ( *p == '/' )
        {
            ++p;
            // Empty tag.
            if ( *p  != '>' )
            {
                return 0;
            }
            //std::cout << "find the end tag.(<value/>) p = "<< &p << std::endl;
            return (p+1);
        }
        else if ( *p == '>' )
        {
            // Done with attributes (if there were any.)
            // Read the value -- which can include other
            // elements -- read the end tag, and return.
            ++p;
            // We should find the end tag now
            if ( StringEqual(p, endTag.c_str(), false) )
            {
                //p += endTag.length();
                p += endTag.size();

                //std::cout << "find the end tag.(</)" << std::endl;
                return p;
            }
            else
            {
                return 0;
            }
        }
    }
}

template<typename T>
const char* OHT<T>::ReadName(const char* p, XML_STRING * name)//�̤U�����R�W�W�h�ӧ�X�ҭn���W�r�A�A�N�o�W�r���r��start�ϱoname=name�۳sstart
{								//�짹����^�ǳo�ӳ̷s�w�Q���ʹL��tape�A�Y��name�����N���F�A�N�^��NULL�C
    *name = "";
    assert( p );

    // Names start with letters or underscores.
    // Of course, in unicode, tinyxml has no idea what a letter *is*. The
    // algorithm is generous.
    //
    // After that, they can be letters, underscores, numbers,
    // hyphens, or colons. (Colons are valid ony for namespaces,
    // but tinyxml can't tell namespaces from names.)
    if ( p && *p && ( isalpha( (unsigned char) *p ) || *p == '_' ) )//�Ѧ��i���A�ڭ̪��R�W�W�h�O�AName���}�Y�r���@�w�O�@�w�O'_'�Φr���A�Y���O�h�^��NULL
    {
        const char* start = p;
        while( p && *p && (     IsAlphaNum( (unsigned char ) *p )
                                || *p == '_' || *p == '-' || *p == '.' || *p == ':' ) )//�R�W���ĤG�H�ᤧ�r���A�䥲���O�r���B�Ʀr�� _ - . :���|�ӲŸ�
        {
            ++p;
        }										//�@����r���A����r�����A�O�X�k�r�����ɭ�
        if ( p-start > 0 )
        {
            name->assign( start, p-start );	//name->assign(const char* str, int len)�O�����A�̤W���ҧ�쪺�r��A�N���s���b�쥻name�̦r�ꪺ�᭱�C
        }					//�ҥH��R�W�W�h���� _ad341aA_-.:���ɭԡA�r��N�@�w�|�}�l�@�۳s(Append)���ʧ@
        return p;				//�W�r�w�g�Q���T����X�ӡA�]���b�o��^�ǳ̷s�w�Q���ʹL��tape��m
    }
    return 0;					//�]����W�r�L�A�o�{�o���O�W�r�r�A�ҥHcallee�^��NULL�A�Ψӧi�Dcaller�o��T�C
}

template<typename T>
const char* OHT<T>::ReadText(const char* p, XML_STRING * text,
                             bool trimWhiteSpace, const char* endTag,
                             bool caseInsensitive)
{
    *text = "";
    if (    !trimWhiteSpace                 // certain tags always keep whitespace
            || !condenseWhiteSpace )       // if true, whitespace is always kept
    {
        // Keep all the white space.
        while (    p && *p
                   && !StringEqual(p, endTag, caseInsensitive)
              )
        {
            int len;
            char cArr[4] = { 0, 0, 0, 0 };
            p = GetChar(p, cArr, &len);
            text->append(cArr, len);
            //std::cout << "text : " << text->c_str() << ", len : " << len << std::endl;
            //std::cout << ", cArr : " << cArr << std::endl;
        }
    }
    else
    {
        bool whitespace = false;

        // Remove leading white space:
        p = SkipWhiteSpace(p);
        while (    p && *p
                   && !StringEqual(p, endTag, caseInsensitive) )
        {
            if ( *p == '\r' || *p == '\n' )
            {
                whitespace = true;
                ++p;
            }
            else if ( IsWhiteSpace( *p ) )
            {
                whitespace = true;
                ++p;
            }
            else
            {
                // If we've found whitespace, add it before the
                // new character. Any whitespace just becomes a space.
                if ( whitespace )
                {
                    (*text) += ' ';
                    whitespace = false;
                }
                int len;
                char cArr[4] = { 0, 0, 0, 0 };
                p = GetChar(p, cArr, &len);
                if ( len == 1 )
                    (*text) += cArr[0];     // more efficient
                else
                    text->append( cArr, len );
            }
        }
    }

    if ( p )
        p += strlen( endTag );
    return p;
}

template<typename T>
const char* OHT<T>::ReadValue(const char* p, Node *node)
{
    // Read in text and elements in any order.
    p = SkipWhiteSpace(p);

    Node* currNode;
    attributeParsingInfo *pAttrInfo;

    while ( p && *p )
    {
        if( *p != '<')
        {
            //std::cout <<"[" << node->__handle << "]"<< "ReadValue PraseObjectClass() in (p != '<')" << std::endl;
            p = ParseObjectClass(p, node);
        }
        else
        {
            // We hit a '<'
            // Have we hit a new element or an end tag?
            if ( StringEqual(p, "</", false) )
            {
                return p;
            }
            else
            {
                //std::cout << " in OHT<T>::ReadValue==> IdentifyNode()" << std::endl;
                switch (IdentifyNode(p))
                {
                case type_attribute:
                    //attrInfo = new attributeParsingInfo();
                    //currNode->__parsing_info->attributeParsingInfoList = &attrInfo;

                    // Get the attributeParsingInfo data
                    //....
                    //pAttrInfo = new attributeParsingInfo;
                    //node->__biggest_handle++;
                    pAttrInfo = new attributeParsingInfo;
                    //node->__parsing_info->attributeParsingInfoList[node->__biggest_handle] = pAttrInfo;
                    addAttributeInfo(node, pAttrInfo);
                    //node->__parsing_info->attributeParsingInfoList[node->__biggest_handle];
                    p = ParseAttributeNode(p, node, pAttrInfo);
                    break;
                case type_objectclass:

                    //std::cout <<"[" << node->__handle << "]" << "find a type_objectclass"<< std::endl;
                    currNode = new Node;
                    nodeCounter++;
                    currNode->__handle = nodeCounter;
		    currNode->rti_handle = new RTI_ObjectClassHandle(currNode->__handle);
                    nodeArray[nodeCounter] = currNode;
                    addNode(node, currNode);
                    addInheriteAttribute(currNode);
                    p = ParseObjectClass(p,currNode);
                    break;
                case type_error:
                    //std::cout <<<<<<<<<<<<<<<<<< " in OHT<T>::ReadValue==> type_error!!" << std::endl;
                    return 0;
                default:
                    p = SkipWhiteSpace(p);
                    break;
                }
            }
        }
        p = SkipWhiteSpace(p);
    }

    if ( !p )
    {
        return 0;
    }
    return p;
}

//�ݩ�ObjectClass Tag���ݩʭȡA�ثe���Ψ쪺�OName��Sharing
template<typename T>
const char* OHT<T>::ParseAttribute(const char* p, XML_STRING* attributeName, XML_STRING* attributeValue)
{

    p = SkipWhiteSpace(p);
    if ( !p || !*p ) return 0;

    // Read the name, the '=' and the value.f
    p = ReadName(p, attributeName);

    if ( !p || !*p )
    {
        return 0;
    }
    p = SkipWhiteSpace(p);
    if ( !p || !*p || *p != '=' )
    {
        return 0;
    }

    ++p;    // skip '='
    p = SkipWhiteSpace(p);
    if ( !p || !*p )
    {
        return 0;
    }

    const char* end;
    const char SINGLE_QUOTE = '\'';
    const char DOUBLE_QUOTE = '\"';

    if ( *p == SINGLE_QUOTE )
    {
        ++p;
        end = "\'";             // single quote in string
        p = ReadText(p, attributeValue, false, end, false);
    }
    else if ( *p == DOUBLE_QUOTE )
    {
        ++p;
        end = "\"";             // double quote in string

        p = ReadText(p, attributeValue, false, end, false);

    }
    else
    {
        // All attribute values should be in single or double quotes.
        // But this is such a common error that the parser will try
        // its best, even without them.

        *attributeValue = "";
        while (    p && *p                                                                                      // existence
                   && !IsWhiteSpace( *p ) && *p != '\n' && *p != '\r'      // whitespace
                   && *p != '/' && *p != '>' )                                                     // tag end
        {
            if ( *p == SINGLE_QUOTE || *p == DOUBLE_QUOTE )
            {
                // [ 1451649 ] Attribute values with trailing quotes not handled correctly
                // We did not have an opening quote but seem to have a
                // closing one. Give up and throw an error.
                return 0;
            }
            *attributeValue += *p;
            ++p;
        }

    }

    return p;
}

// �ѪR<ObjectClass���U�@�h��<attribute> Tag, <attribute> �ӼƤ��w�C
//attributeParseInfo�HattributeParseInfoList��_��
template<typename T>
const char* OHT<T>::ParseAttributeNode(const char* p, Node *currNode, attributeParsingInfo* attrInfo)
{
    XML_STRING value;

    if ( *p != '<' )
    {
        return 0;
    }

    p = SkipWhiteSpace(p+1);
    p = ReadName(p, &value);
    if ( !p || !*p )
    {
        return 0;
    }

    XML_STRING endTag ("</");
    endTag += value;
    endTag += ">";

    // Check for and read attributes. Also look for an empty
    // tag or an end tag.
    while ( p && *p )
    {
        p = SkipWhiteSpace(p);
        if ( !p || !*p )
            return 0;

        if ( *p == '/' )
        {
            ++p;
            // Empty tag.

            if ( *p  != '>' )
            {
                return 0;
            }
            return (p+1);
        }
        else if ( *p == '>' )
        {
            // Done with attributes (if there were any.)
            // Read the value -- which can include other
            // elements -- read the end tag, and return.
            ++p;
            p = ReadValue(p, currNode);
            //std::cout << "find a end Tag..." << std::endl;
            if ( !p || !*p )
                return 0;

            // We should find the end tag now
            if ( StringEqual(p, endTag.c_str(), false) )
            {
                //p += endTag.length();
                p += endTag.size();
                return p;
            }
            else
            {
                return 0;
            }

        }
        else
        {
            // Try to read an attribute:
            XML_STRING attrName, attrValue;

            p = ParseAttribute(p, &attrName, &attrValue);


            if ( !p || !*p )
            {
                return 0;
            }

            //td::cout << "ParseAttributeNode => " << std::endl;
            //std::cout << "attrName : " << attrName.c_str() << std::endl;
            //std::cout << "attrValue : " << attrValue.c_str() << std::endl;

            /*if(node->__parsing_Info->attCount == 0 )
            {
              //�|����Structure attributeParseInfo�A�s�W�@ attributeParseInfo
            attributeParsingInfo *pAttrParseInfo = new attributeParsingInfo;
            node->__parsing_Info->attCount++;
            node->__parsing_Info->attFirst = 1;
            node->__parsing_Info->attLast = 1;
            node->__parsing_Info->attCurrent = 1;
            }
            else
            {
            node->__parsing_info->attributeParsingInfoList[];
            }*/

            /////!!!! Need to re-write
            //�s�W�@structure attributeParsingInfo�A�Nparse����Ʃ�Jstructure
            char* n = attrName.c_str();

            // std::cout << "Attribute => " << std::endl;
            if ( StringEqual(n, "name", false) )
            {
                attrInfo->name = new char[attrValue.size()+1];
                strcpy(attrInfo->name , attrValue.c_str());
		
		attrInfo->rti_name = new RTI_wstring(StrtoWStr(attrInfo->name));
                //std::cout << "name :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "dataType", false) )
            {
                attrInfo->dataType = new char[attrValue.size()+1];
                strcpy(attrInfo->dataType , attrValue.c_str());
                //std::cout << "dataType :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "updateType", false) )
            {
                attrInfo->updateType = new char[attrValue.size()+1];
                strcpy(attrInfo->updateType , attrValue.c_str());
                //std::cout << "updateType :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "updateCondition", false) )
            {
                attrInfo->updateCondition = new char[attrValue.size()+1];
                strcpy(attrInfo->updateCondition , attrValue.c_str());
                //std::cout << "updateCondition :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "ownership", false) )
            {
                attrInfo->ownership = new char[attrValue.size()+1];
                strcpy(attrInfo->ownership , attrValue.c_str());
                //std::cout << "ownership :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "sharing", false) )
            {
                attrInfo->sharing = new char[attrValue.size()+1];
                strcpy(attrInfo->sharing , attrValue.c_str());
                //std::cout << "sharing :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "dimensions", false) )
            {
                attrInfo->dimensions = new char[attrValue.size()+1];
                strcpy(attrInfo->dimensions , attrValue.c_str());;
                //std::cout << "dimensions :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "transportation", false) )
            {
                attrInfo->transportation = new char[attrValue.size()+1];
                strcpy(attrInfo->transportation , attrValue.c_str());
                //std::cout << "transportation :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "order", false) )
            {
                attrInfo->order = new char[attrValue.size()+1];
                strcpy(attrInfo->order , attrValue.c_str());
                //std::cout << "order :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "updateConditionNotes", false) )
            {
                attrInfo->updateConditionNotes = new char[attrValue.size()+1];
                strcpy(attrInfo->updateConditionNotes , attrValue.c_str());
                //std::cout << "updateConditionNotes :" << attrValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "semantics", false) )
            {
                attrInfo->semantics = new char[attrValue.size()+1];
                strcpy(attrInfo->semantics , attrValue.c_str());
                //std::cout << "semantics :" << attrValue.c_str() << std::endl;
            }
        }
    }


    return p;
}


template<typename T>
const char* OHT<T>::ParseObjectClass(const char* p, Node *node)
{
    //std::cout << std::endl;
    //std::cout << "in OHT::ParseObjectClass()... =>" << *p << std::endl;
    XML_STRING value;

    //�H<�}�Y
    if ( *p != '<' )
    {
        return 0;
    }
    else
    {
        p = SkipWhiteSpace(p+1);

        //Ū�X<���᪺�W��
        p = ReadName(p, &value);

        if ( !p || !*p )
        {
            return 0;
        }

        Level++;
        node->__level = Level;
        //addNode(node);
        //std::cout <<"[" << node->__handle << "]"  << "node->__level = " << node->__level << std::endl;

        //std::cout << "Level = " << Level << std::endl;
    }

    //�@�]�wEnd Tag </value>
    XML_STRING endTag ("</");
    endTag += value;
    endTag += ">";


    while ( p && *p )
    {

        p = SkipWhiteSpace(p);
        if ( !p || !*p )
            return 0;

        // End Tag "/>"
        if ( *p == '/' )
        {
            ++p;
            // Empty tag.
            if ( *p  != '>' )
            {
                return 0;
            }


            node->__level = Level;
            Level--;
            //addNode(node);
            //std::cout <<"[" << node->__handle << "]" << "node->__level = " << node->__level << std::endl;


            //std::cout << "Level = " << Level << std::endl;


            //std::cout <<"[" << node->__handle << "]" << "find the end tag.1(<" << value << ".../>) p = "<< &p << ", node->__parsing_info->name = " << node->__parsing_info->name<< std::endl;

            return (p+1);
        }
        else if ( *p == '>' )
        {
            // Done with attributes (if there were any.)
            // Read the value -- which can include other
            // elements -- read the end tag, and return.
            ++p;


            p = ReadValue(p, node);
            if ( !p || !*p )
                return 0;

            // We should find the end tag now
            if ( StringEqual(p, endTag.c_str(), false) )
            {
                //p += endTag.length();
                p += endTag.size();


                node->__level = Level;
                //std::cout <<"[" << node->__handle << "]" << "node->__level = " << node->__level << std::endl;
                Level--;

                //std::cout << "Level = " << Level << std::endl;

                //std::cout <<"[" << node->__handle << "]"  << "find the end tag.2(<" << value << "/>) p = "<< &p << ", node->__parsing_info->name = " << node->__parsing_info->name << std::endl;

                return p;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            // ��XAttribute Name�� Value
            XML_STRING attrName, attrValue;

            p = ParseAttribute(p, &attrName, &attrValue);

            //�Yp���ũάOp���e���šA���}function
            if ( !p || !*p )
            {
                return 0;
            }

            char* n = attrName.c_str();

            //std::cout << "attributeValue.c_str() : " << attributeValue.c_str() << std:: endl;

            //�x�sObjectClass Name ��attribute�������
            if ( StringEqual(n, "name", false) )
            {
                node->__parsing_info->name = new char[attrValue.size()+1];
                strcpy(node->__parsing_info->name, attrValue.c_str());

		node->__parsing_info->rti_name = new RTI_wstring(StrtoWStr(node->__parsing_info->name));
		
                //test
                //std::cout <<"[" << node->__handle << "]" << "__parsing_info->name : " << node->__parsing_info->name << std:: endl;
                //std::cout << "attrValue.c_str() : " << attrValue.c_str() << std:: endl;
            }
            else if ( StringEqual(n, "sharing", false) )
            {
                node->__parsing_info->sharing = new char[attrValue.size()+1];
                strcpy(node->__parsing_info->sharing, attrValue.c_str());

                //test
                //std::cout << "node->__parsing_info->sharing : " << node->__parsing_info->sharing << std:: endl;
                //std::cout << "attrValue.c_str() : " << attrValue.c_str() << std:: endl;
            }
            else if ( StringEqual(n, "semantics", false) )
            {
                node->__parsing_info->semantics = new char[attrValue.size()+1];
                strcpy(node->__parsing_info->semantics, attrValue.c_str());

                //test
                //std::cout << "node->__parsing_info->semantics : " << node->__parsing_info->semantics << std:: endl;
                //std::cout << "attrValue.c_str() : " << attrValue.c_str() << std:: endl;
            }
        }
    }

    return p;
}

//*************************************************************************
//*
//* Method 1    : read_XMLfile
//* Description : ��xml��Ū�Jfile_content�ܼƸ̡A�䬰���Achar*���ܼơC�YŪ������
//*               �^��false
//* Input       : fullPathFileName
//* Output      : True  : Ū��xml file���\
//*               False : Ū��xml file����
//* Algorithm   :
//*************************************************************************
template<typename T>
bool OHT<T>::read_XMLfile(char* fullPathFileName)
{

    FILE *f = fopen(fullPathFileName, "rb");
    if (f == NULL)
    {
        // File does not exist
        //std::cout << "File Not Found!!" << std::endl;
        return false;
    }

    // File size == 0?? Yes, show the error message
    long fileLength = 0;
    fseek(f, 0, SEEK_END);
    fileLength = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fileLength == 0)
    {
        //File Lengthe is 0.
        //std::cout << "File Length is 0!!" << std::endl;
        return false;
    }

    this->file_content = new char[fileLength + 1];
    this->file_content[0] = 0;
    if (fread(this->file_content, fileLength, 1, f) == 0)
    {
        //std::cout << "File Read Error!!" << std::endl;
        return false;
    }

    fclose(f);
    //std::cout << this->file_content << std::endl;
    return true;
}


//*************************************************************************
//* Method 2    : parsing
//* Description : �w��file_contentdd��Interaction Class Tree�@parsing�A�Y
//*               parsing���~�hreturn false�A�YŪ�����ѫhreturn FALSE
//* Input       : �L
//* Output      : True  : ��R�ɮצ��\
//*               False : ��R�ɮץ���
//* Algorithm   :
//*************************************************************************

template<typename T>
bool OHT<T>::parsing()
{
    //std::cout << "OHT::parsing()" << std::endl;
    if (!this->file_content)
    {
        return false;
    }

    // Initialize var
    gLocation.Clear();
    this->biggest_handle = 0;
    nodeCounter = 0;

    // If we have a file, assume it is all one big XML file, and read it in.
    // The document parser may decide the document ends sooner than the entire file, however.
    // Remove all the CR-LF.
    XML_STRING data;
    data.reserve(strlen(this->file_content));

    const char* lastPos = this->file_content;
    const char* p = this->file_content;

    //read one line to data, and move cursor to next line.

    while( *p )
    {

        if ( *p == NEWLINE )
        {
            // Newline character. No special rules for this. Append all the characters
            // since the last string, and include the newline.
            data.append( lastPos, (p-lastPos+1) );  // append, include the newline
            ++p;                                                                    // move past the newline
            lastPos = p;                                                    // and point to the new buffer (may be 0)
            assert( p <= (this->file_content + strlen(this->file_content)) );
        }
        else if ( *p == CARRIAGERETURN )
        {
            // Carriage return. Append what we have so far, then
            // handle moving forward in the buffer.
            if ( (p - lastPos) > 0 )
            {
                data.append( lastPos, p-lastPos );      // do not add the CR
            }
            data += (char)CARRIAGERETURN;                                              // a proper newline

            if ( *(p+1) == CARRIAGERETURN )
            {
                // Carriage return - new line sequence
                p += 2;
                lastPos = p;
                assert( p <= (this->file_content + strlen(this->file_content)) );
            }
            else
            {
                // it was followed by something else...that is presumably characters again.
                ++p;
                lastPos = p;
                assert( p <= (this->file_content + strlen(this->file_content)) );
            }
        }
        else
        {
            ++p;
        }
    }

    // Handle any left over characters.
    // std::cout << "p:" << p << "lastPos:" << lastPos << ", p-lastPos :" << p-lastPos << std::endl;
    if ( p - lastPos )
    {
        data.append( lastPos, p-lastPos );
    }

    // Parsing ObjectClass
    char* newStr = data.c_str();
    Node *node;

    // std::cout << newStr << std::endl;

    //XmlParsingData parsingData( newStr, 4, gLocation.row, gLocation.col );
    p = SkipWhiteSpace( newStr );
    if ( !p )
    {
        // Show the error;
        return false;
    }

    while ( p && *p )
    {
        int id = IdentifyNode(p);
        switch (id)
        {
        case type_xmlHeader:
        case type_dtdHeader:
        case type_commentHeader:
        case type_objectModel:
            //std::cout << "type_xmlHeader or type_dtdHeader or type_commentHeader or type_objectModel ..." << id << std::endl;
            //skip all these content
            while ( p && *p && *p != '>' )
            {
                ++p;
            }
            ++p; //skip ">"
            break;
        case type_objectCollection:
            //skip all these content
            //std::cout << "type_objectCollection..." << id << std::endl;
            while ( p && *p && *p != '>' )
            {
                ++p;
            }
            ++p; //skip ">"

            //gLocation.col = 0;
            //gLocation.row = 0;

            //root
            //root = new Node();
            //current=root;
            break;
        case type_endObjectCollection:
            return true;
        case type_attribute:
            //std::cout << "type_attribute..." << std::endl;
            p = SkipTag(p);
            break;
        case type_objectclass:
            node = new Node();
            nodeCounter++;
            node->__handle = nodeCounter;
	    node->rti_handle = new RTI_ObjectClassHandle(node->__handle);
            nodeArray[nodeCounter] = node;

            //for root
            //if(root == NULL)
            //{
            root = node;
            //}
            //else
            //{
            //  addNode(root,node);
            //}
            p = ParseObjectClass(p, node);
            break;
            //case  type_interactions:
            //case  type_dimensions:
            //case  type_time:
            //case  type_tags:
            //case  type_synchronizations:
            //case  type_transportations:
            //case  type_switches:
            //case  type_dataTypes:
            //case  type_notes:
        case  type_error:
            //std::cout << "type_error" << std::endl;
            p = SkipTag(p);
            return false;
        default:
            //std::cout << "default..." << std::endl;
            p = SkipTag(p);
            break;
        }

        p = SkipWhiteSpace(p);
    }

    assignTailValue();

    treeReAllocation();

    delete [] this->file_content;

    return true;
}

template<typename T>
void OHT<T>::addAttributeInfo(Node * node, attributeParsingInfo* attrInfo)
{

    //printf("__parsing_info->name = %s\n" , node->__parsing_info->name);
    //printf("addAttributeInfo : \n" );

    //std::cout << "attrInfo = " << attrInfo << std::endl;
    node->__biggest_handle++;
    node->__parsing_info->attributeParsingInfoList[node->__biggest_handle] = attrInfo;
    //std::cout << "add an attributeInfo = " << node->__parsing_info->attributeParsingInfoList[node->__biggest_handle]->name << std::endl;
}

template<typename T>
void OHT<T>::addNode(Node * parentNode, Node * childNode)
{
    //std::cout << "addNode a node for :" << childNode->__handle << std::endl;
    //int findLevel = (childNode->__level);
    //std::cout << "parentNode = " << parentNode->__handle << std::endl;
    //std::cout << "parentNode = " << childNode->__handle << std::endl;

    Node * prevNode;
    unsigned long  childNo;
    unsigned long  handle;


    childNo = parentNode->__child_no;

    childNo++;
    //�ˬdchildNo�O�_�W�L�w�]��
    if(childNo <= MAX_NODE_NO )
    {
        //�D�Ĥ@�Ӥl�`�I��
        if(childNo != 1)
        {
            //���o���l�`�I
            prevNode = parentNode->__child_List[parentNode->__child_no];
            //���k�l�`�I����
            childNode->__prev = prevNode;
            prevNode->__next = childNode;
        }
        //�W�[���`�I���l�`�I�Ӽ�
        parentNode->__child_no = childNo;
        //�l�`�I���V���`�I
        childNode->__parent = parentNode;
        //�l�`�I�[�J���`�I
        parentNode->__child_List[childNo] = childNode;

        //Node����DFS���X�����ǡA�H����iterator�i�H�ϥ�DFS�覡���X
        handle = childNode->__handle;

        if(handle > 0)
        {
            Node* forwardNode = nodeArray[handle-1];
            forwardNode->__forward = childNode;
            childNode->__backward = forwardNode;
        }
        //printf("parentNode->__child_List[%d] = %X\n",childNo,parentNode->__child_List[childNo]);
    }
    else
    {
        //std::cout << "---------!!MAX_NODE_NO is reached!!----------" << std::endl;
    }

}

template<typename T>
void  OHT<T>::assignTailValue()
{
    Node *fixNode;
    Node *travlNode;
    unsigned long tailValue;
    //unsigned long level;
    int childCounter;
    for(int i = 1; i <= nodeCounter; i++)
    {
        fixNode = nodeArray[i];
        childCounter = 0;
        for(int j = i+1; j <= nodeCounter; j++)
        {
            travlNode = nodeArray[j];
            if(travlNode->__parent == fixNode->__parent)
            {
                break;
            }
            else
            {
                if(travlNode->__parent == fixNode)
                {
                    childCounter++;
                }
                tailValue = travlNode->__handle;
            }
        }
        if(childCounter == 0)
        {
            fixNode->__tail_value = fixNode->__handle;
        }
        else
        {
            fixNode->__tail_value = tailValue;
        }
        //std::cout << "node[" << i << "].tailValue = " <<fixNode->__tail_value << std::endl;
    }
}

template<typename T>
void  OHT<T>::addInheriteAttribute(Node *node)
{
    Node *parentNode;

    attributeParsingInfo * pAttrInfo;
    unsigned long biggest_handle;

    //std::cout << "Add Inherite AttributeParsingInfo..." << std::endl;

    //���parent�`�I�A���oattrInfoNo, attributeParsingInfoList
    parentNode = node->__parent;
    biggest_handle = parentNode->__biggest_handle;
    node->__inherited_attr_no = biggest_handle;

    //std::cout << "biggest_handle = " << biggest_handle << std::endl;

    for (int i = 1; i <= biggest_handle; i++)
    {
        pAttrInfo = parentNode->__parsing_info->attributeParsingInfoList[i];

        //std::cout << "Attribute = " << pAttrInfo->name << std::endl;

        addAttributeInfo(node, pAttrInfo);
    }
}
template <typename T>
void OHT<T>::treeReAllocation()
{

    Node *pNode;
    Node **newNodeArray;
    Node **newChildList;
    attributeParsingInfo **newAttrInfoList;
    int childNo;
    int biggest_handle;

    //���s�إ�nodeArray���а}�C
    newNodeArray=new Node*[nodeCounter+1];

    for(int i = 1; i <= nodeCounter; i++)
    {
        //�ƻs��ƨ�s�}�C
        newNodeArray[i] = nodeArray[i];

        //���s�إ�childList���а}�C
        pNode = nodeArray[i];

        //���o�ثe�`�I���l�`�I��
        childNo = pNode->__child_no;
        //�s�W�A����а}�C�j�p
        newChildList = new Node* [childNo+1];

        for(int j = 1; j <= childNo; j++)
        {
            //�ƻs�«��а}�C���
            newChildList[j] = pNode->__child_List[j];

            //���s�إߥثe�`�I��attributeParsingInfoList���а}�C
            biggest_handle = pNode->__biggest_handle;

            newAttrInfoList = new attributeParsingInfo* [biggest_handle+1];

            for(int k = 1; k <= biggest_handle; k++)
            {
                //�ƻsarrtibuteParsingInfoList�ܷs�����а}�C
                newAttrInfoList[k] = pNode->__parsing_info->attributeParsingInfoList[k];
            }
            //arrtibuteParsingInfoList�ƻs�����AattributeParsingInfoList����O����
            delete [] pNode->__parsing_info->attributeParsingInfoList;
            pNode->__parsing_info->attributeParsingInfoList = newAttrInfoList;
        }

        delete  [] pNode->__child_List;
        pNode->__child_List = newChildList;

    }
    delete  [] nodeArray;
    nodeArray = newNodeArray;

}


}
#endif
