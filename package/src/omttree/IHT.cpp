#ifndef IHT_CPP
#define IHT_CPP
/******************************************************************************
 *            IHT.h
 *
 *  Tue Nov 21 12:55:23 2006
 *  Copyright  2006  Bruce Wang
 *  cwwang2@gmail.com
 *******************************************************************************/

#include <cstdio>
#include <malloc.h>
#include <assert.h>
#include <iostream>
#include "globle.h"
#include "IHT.h"


namespace omt{

template <typename T>
IHT<T>::IHT()
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
IHT<T>::~IHT()
{

    //std::cout <<"IHT deconstructor..."<<std::endl;

    Node *pNode;
    Node *pChildNode;
    parameterParsingInfo *pParamInfo;
    int childNo;
    int biggest_handle;
    int inherited_param_no;
    if(nodeCounter != 0)
    {
        for(int i = nodeCounter ; i >= 1; i--)
        {
            //std::cout << "delete Node..."<< i << std::endl;
            pNode = nodeArray[i];

            childNo = pNode->__child_no;

            biggest_handle = pNode->__biggest_handle;
            inherited_param_no =  pNode->__inherited_param_no;

            for(int k = inherited_param_no+1; k <= biggest_handle ; k++)
            {

                //�R��parameterParsingInfo�Ψ�r��
                pParamInfo = pNode->__parsing_info->parameterParsingInfoList[k];
                //std::cout << "delete parameterParsingInfo..."<< k << ",name = "<< pParamInfo->name <<std::endl;
                delete [] pParamInfo->dataType;
                delete [] pParamInfo->name;
		if (pParamInfo->rti_handle)
		    delete pParamInfo->rti_handle;
		delete pParamInfo->rti_name;
                delete [] pParamInfo->semantics;
                delete [] pParamInfo;
            }
            //std::cout << "delete ObjectClassParsignInfo..."<< i << std::endl;
            delete [] pNode->__parsing_info->parameterParsingInfoList;
            delete [] pNode->__parsing_info->name;
	    delete pNode->__parsing_info->rti_name;
            delete [] pNode->__parsing_info->semantics;
            delete [] pNode->__parsing_info->sharing;
            delete [] pNode->__parsing_info->dimensions;
            delete [] pNode->__parsing_info->transportation;
            delete [] pNode->__parsing_info->order;

            delete [] pNode->__child_List;

            delete pNode;
        }
        delete  [] nodeArray;
    }

}

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
//* Time complexity : O(1)
//**********************************************************************

template <typename T>
int IHT<T>::isAncestorOf(unsigned long IChandleA, unsigned long IChandleB)
{
    if(IChandleA <= 0 || IChandleB <= 0 || IChandleA > nodeCounter || IChandleB > nodeCounter)
        return 0;
    else
        // if(IChandleA != IChandleB)
    {
        //IChandleB��Haldle�bOC HandleA��Interval����
        if((IChandleA < IChandleB) && (nodeArray[IChandleA]->__tail_value >= IChandleB))
        {
            return 1;
        }//IChandleA��Haldle�bOC HandleB��Interval����
        else if( (IChandleB < IChandleA) && (nodeArray[IChandleB]->__tail_value >= IChandleA))
        {
            return -1;
        }
        else//�YIChandleA�PIChandleB���ۦPObjectClass�S���������Y
            return 0;
    }
}

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
template <typename T>
char* IHT<T>::getInteractionClassName(unsigned long IChandle)
{        //childCounter++;
    if(IChandle > 0 && IChandle <= nodeCounter)
        return nodeArray[IChandle]->__parsing_info->name;
    else
        return "\n";
}

template <typename T>
RTI_wstring* IHT<T>::getRTIInteractionClassName(unsigned long IChandle)
{        //childCounter++;
    if(IChandle > 0 && IChandle <= nodeCounter)
        return nodeArray[IChandle]->__parsing_info->rti_name;
    else
        return NULL;
}


//*************************************************************************
//* Method 5    : getInteractionClassHandle
//* Description : ���w����Interaction Class name��OHT�A�z�L��function�i�o�쪺�^�ǭ�
//*               ����name�ҹ�����Interaction class handle�C�խY��name���s�b�h�^�ǭ�0
//* Input       : IChandle
//* Output      : �^�Ǹ�class name�ҹ�����handle��
//*               0 = ���s�b
//* Algorithm   :
//*************************************************************************
template <typename T>
unsigned long IHT<T>::getInteractionClassHandle(char* ICname)
{
    if(ICname == NULL)
        return 0;
    for(int i = 1; i<= nodeCounter; i++)
    {
        if(!strcmp(nodeArray[i]->__parsing_info->name, ICname))
            return nodeArray[i]->__handle;
    }

    return 0;
}

template <typename T>
RTI_InteractionClassHandle *IHT<T>::getRTIInteractionClassHandle(char* ICname)
{
    if(ICname == NULL)
        return 0;
    for(int i = 1; i<= nodeCounter; i++)
    {
        if(!strcmp(nodeArray[i]->__parsing_info->name, ICname))
            return nodeArray[i]->rti_handle;
    }

    return 0;
}

//*************************************************************************
//* Method 6    : getParameterName
//* Description : ���wIChandle�i�o����handle�ҹ������S�wInteraction Class�A���禡�^
//*               �Ǧb�S�wInteraction Class���UParameter handle�ҹ�����Parameter name
//* Input       : IChandle
//*               parameter_handle
//* Output      : parameter name
//* Algorithm : ���wIChandle�i�o����handle�ҹ������S�wInteratction Class�A���禡�^
//*             �Ǧb�S�wInteratction Class���UParameter handle�ҹ�����Parameter
//*             name�C
//* Time complexity : O(1)
//**********************************************************************

template <typename T>
char* IHT<T>::getParameterName(unsigned long IChandle,unsigned long Parameter_handle)
{
    if(IChandle <=0 || Parameter_handle <= 0)
        return "\n";
    if(IChandle <= nodeCounter && (Parameter_handle <= nodeArray[IChandle]->__biggest_handle))
        return nodeArray[IChandle]->__parsing_info->parameterParsingInfoList[Parameter_handle]->name;
    else
        return "\n";
}

template <typename T>
RTI_wstring *IHT<T>::getRTIParameterName(unsigned long IChandle,unsigned long Parameter_handle)
{
    if(IChandle <=0 || Parameter_handle <= 0)
        return NULL;
    if(IChandle <= nodeCounter && (Parameter_handle <= nodeArray[IChandle]->__biggest_handle))
        return nodeArray[IChandle]->__parsing_info->parameterParsingInfoList[Parameter_handle]->rti_name;
    else
        return NULL;
}

//*************************************************************************
//* Method 7    : getParameterHandle
//* Description : ���wIChandle�i�o����handle�ҹ������S�wInteratction Class�A���禡�^
//*               �Ǧb�S�wInteratction Class���Uparameter name�ҹ�����parameter handle
//* Input       : IChandle
//*               parameter_name
//* Output      : parameter handle
//* Algorithm : ���wIChandle�i�o����handle�ҹ������S�wInteratction Class�A���禡�^
//*             �Ǧb�S�wInteratction Class���UParameter name�ҹ�����Parameter
//*             handle�C
//* Time complexity : O(n), n���ݩʼƶq
//**********************************************************************

template <typename T>
unsigned long IHT<T>::getParameterHandle(unsigned long IChandle,char* parameter_name)
{
    Node *pNode;
    parameterParsingInfo *pParamInfo;

    if((IChandle > 0 && IChandle <= nodeCounter) && parameter_name != NULL)
    {
        pNode = nodeArray[IChandle];
        for(int i = 1; i<= pNode->__biggest_handle; i++)
        {
            pParamInfo = pNode->__parsing_info->parameterParsingInfoList[i];
            //strcmp����ۦP��0
            if (strcmp(pParamInfo->name, parameter_name) == 0)
                return i;
        }

    }
    return 0;

}

template <typename T>
RTI_ParameterHandle *IHT<T>::getRTIParameterHandle(unsigned long IChandle,char* parameter_name)
{
    Node *pNode;
    parameterParsingInfo *pParamInfo;

    if((IChandle > 0 && IChandle <= nodeCounter) && parameter_name != NULL)
    {
        pNode = nodeArray[IChandle];
        for(int i = 1; i<= pNode->__biggest_handle; i++)
        {
            pParamInfo = pNode->__parsing_info->parameterParsingInfoList[i];
            //strcmp����ۦP��0
            if (strcmp(pParamInfo->name, parameter_name) == 0)
	    {
		if (pParamInfo->rti_handle == NULL)
		    pParamInfo->rti_handle = new RTI_ParameterHandle(i);;
		return pParamInfo->rti_handle;
	    }
        }

    }
    return 0;

}

//*************************************************************************
//* Method 8    : getInteractionClass
//* Description : �^�ǫ��Vobj T������ (�ҥH���w���� T��Interaction Class Handle����
//*               IChandle��)
//* Input       : IChandle
//* Output      : T������
//* Algorithm :
//* Time complexity : O(1)
//**********************************************************************
template <typename T>
typename IHT<T>::value_ptr IHT<T>::getInteractionClass(unsigned long IChandle)
{
    if(IChandle > 0 && IChandle <= nodeCounter)
    {
        return nodeArray[IChandle]->__data;
    }
    else
        return NULL;
}

//*************************************************************************
//* Method 9    : getInheritedNumber
//* Description : �^�Ǹ�Interaction Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node�]�t
//*               root�PIChandle)
//* Input       : IChandle
//* Output      : �^�Ǹ�Interaction Class��root�o��path�W�Ҧ���Node�ƶq
//* Algorithm :
//**********************************************************************
template <typename T>
unsigned long IHT<T>::getInheritedNumber(unsigned long IChandle)
{
    unsigned long tailValue;
    unsigned long counter = 0;

    if(IChandle > 0 && IChandle <= nodeCounter)
    {
        tailValue = nodeArray[IChandle]->__tail_value;
        //for(int i = IChandle; i <= tailValue; i++)
        //{
        //    counter++;
        //}
        if(tailValue > 0)
            counter = (tailValue - IChandle + 1);
    }
    return counter;
}

//*************************************************************************
//* Method 10   : isInteractionClass
//* Description : �P�_��IChandle�O�_��������Interaction Class Tree�����䤤�@��Interaction Class
//* Input       : IChandle
//* Output      : True�G������
//*               False�G�L����
//* Algorithm :
//* Time complexity : O(n)
//**********************************************************************
template <typename T>
bool IHT<T>::isInteractionClass (unsigned long IChandle)
{
    if(IChandle > 0 && IChandle <= nodeCounter)
        return true;
    else
        return false;

}

//*************************************************************************
//* Method 11   : isInteractionClass
//* Description : �P�_��ICname�O�_��������Interaction Class Tree�����䤤�@��Interatction Class
//* Input       : ICname
//* Output      : True�G������
//*               False�G�L����
//* Algorithm :
//* Time complexity : O(1)
//**********************************************************************
template <typename T>
bool IHT<T>::isInteractionClass ( char* ICname)
{
    if(ICname == NULL)
        return false;
    //��DFS�覡�j�M�W��
    for(int i = 1; i<= nodeCounter; i++)
    {
        if(!strcmp(nodeArray[i]->__parsing_info->name, ICname))
            return true;
    }
    return false;
}

//*************************************************************************
//* Method 12   : isInheritedClassParameter
//* Description : �bIChandle�ҹ�����Interatction Class�U�A�Y��parameter_handle������
//*               Parameter�O�~�ӦӨӪ�iff�^��TRUE
//* Input       : IChandle, parameter_handle
//* Output      : True,������
//*               False,�L����
//* Algorithm :
//**********************************************************************
template <typename T>
bool IHT<T>::isInheritedClassParameter(unsigned long IChandle, unsigned long parameter_handle)
{
    if((IChandle > 0 && IChandle <= nodeCounter))
    {
        if (parameter_handle > 0 && parameter_handle <= nodeArray[IChandle]->__biggest_handle)
        {
            if(nodeArray[IChandle]->__biggest_handle > 0)
            {
                //parameterHandle��inherite parameter handle�j
                //�B�p�󵥩�̤j��parameter handle
                if( parameter_handle <= nodeArray[IChandle]->__biggest_handle &&
                        parameter_handle > nodeArray[IChandle]->__inherited_param_no)
                {
                    return true;
                }
            }
        }
    }
    return false;
}


//*//*************************************************************************
//* Method 13   : getNumberOfParameter
//* Description : �bIChandle����Interaction Class���U�A�^�Ǹ�Interaction Class��
//*               parameter���ƶq
//* Input       : IChandle
//* Output      : unsigned long
//* Algorithm :
//**********************************************************************
template <typename T>
unsigned long IHT<T>::getNumberOfParameter(unsigned long IChandle)
{
    if(IChandle > 0 && IChandle <= nodeCounter)
    {
        return (nodeArray[IChandle]->__biggest_handle);
    }
    else
        return 0;
}
//*************************************************************************
//* Method 14   : getNumberOfInheritedParameter
//* Description : �^�Ǹ�Interatction Class�~�ӦӨӪ�Parameter���ƶq(���]�t�ۤvObject
//*               Class�s�W��Parameter)
//* Input       : IChandle
//* Output      : unsigned long
//* Algorithm   :
//*************************************************************************
template <typename T>
unsigned long IHT<T>::getNumberOfInheritedParameter(unsigned long IChandle)
{
    if(IChandle > 0 && IChandle <= nodeCounter)
        return (nodeArray[IChandle]->__inherited_param_no);
    else
        return 0;
}

//*************************************************************************
//* Method 15   : getNumberOfInheritedClass
//* Description : �^�Ǹ�Interatction Class��root�o��path�W�Ҧ���Node�ƶq (�o��Node���]�tIChandle)
//* Input       : IChandle
//* Output      : unsigned long
//* Algorithm   :
//*************************************************************************
template <typename T>
unsigned long IHT<T>::getNumberOfInheritedClass(unsigned long IChandle)
{
    //unsigned long tailValue;
    //unsigned long counter = 0;
    //tailValue = (nodeArray[IChandle]->__tail_value);

    //�q�l�`�I�}�l��l�𪺳̫�@�Ӹ`�I
    //for(int i = (IChandle + 1); i <= tailValue; i++)
    //{
    //  counter++;
    //}
    //return counter;
    if(IChandle > 0 && IChandle <= nodeCounter)
        return ((nodeArray[IChandle]->__level) - 1);
    else
        return 0;

}

template <typename T>
unsigned long IHT<T>::getClassNodeCount()
{
    return nodeCounter;
}

template <typename T>
unsigned long IHT<T>::getClassTailValue(unsigned long OChandle)
{
    return nodeArray[OChandle]->__tail_value;
}

template <typename T>
unsigned long IHT<T>::getClassParentNode(unsigned long OChandle)
{
   return nodeArray[OChandle]->__parent->__handle;
}


template<typename T>
int IHT<T>::IdentifyNode(const char* p)
//int IdentifyNode(const char* p)
{
    const char* xmlHeader = { "<?xml" };
    const char* commentHeader = { "<!--" };
    const char* dtdHeader = { "<!" };

    int nodeType = 0;

    p = SkipWhiteSpace( p );
    if( !p || !*p || *p != '<' )
    {
        //std::cout << "type error => " << *p << std::endl;
        return type_error;
    }

    if ( StringEqual(p, xmlHeader, true) )
    {
        // found XML Header
        p = SkipWhiteSpace(p);

        if ( !p || !*p || !StringEqual(p, "<?xml", true) )
        {
            // Not a valid XML format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_xmlHeader;
    }
    else if ( StringEqual(p, commentHeader, false) )
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
    else if ( StringEqual( p, "</objects", false) )
    {
        if ( !p || !*p || !StringEqual(p, "</objects", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_endObjectCollection;
    }

    else if ( StringEqual( p, "<objectClass", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<objectClass", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_objectclass;
    }
    else if ( StringEqual( p, "<attribute", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<attribute", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
    }
    else if ( StringEqual( p, "<interactions", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<interactions", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        //std::cout << "find => " << *p << ",type_interactionCollection."<< std::endl;
        nodeType = type_interactionCollection;
    }
    else if ( StringEqual( p, "</interactions", false) )
    {
        if ( !p || !*p || !StringEqual(p, "</interactions", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        //std::cout << "find => " << *p << ",type_interactionCollection."<< std::endl;
        nodeType = type_endInteractionCollection;
    }
    else if ( StringEqual( p, "<interactionClass", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<interactionClass", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        //std::cout << "find => " << *p << ",type_parameter."<< std::endl;
        nodeType = type_interactionclass;
    }
    else if ( StringEqual( p, "<parameter", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<parameter", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        //std::cout << "find => " << *p << ",type_parameter."<< std::endl;
        nodeType = type_parameter;
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
const char* IHT<T>::SkipTag(const char * p)
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
            // Done with parameters (if there were any.)
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
const char* IHT<T>::SkipObjects(const char * p)
{
    //std::cout << "in SkipObjects()..." << std::endl;
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
        if ( *p == '>' )
        {
            //std::cout << "p=" << p <<"*p == >" << std::endl;
            // Done with parameters (if there were any.)
            // Read the value -- which can include other
            // elements -- read the end tag, and return.
            //++p;
            p = SkipWhiteSpace(p+1);
            //std::cout << "p=" << p <<"*p == >" << std::endl;
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
                //std::cout << "StringEqual(p, endTag.c_str(), false) == fale" << std::endl;
                //return 0;
                ++p;
            }
        }
        else
        {
            //std::cout << "*p != '>'" << std::endl;
            ++p;
        }
    }
}

template<typename T>
const char* IHT<T>::ReadName(const char* p, XML_STRING * name)
{
    *name = "";
    assert( p );

    // Names start with letters or underscores.
    // Of course, in unicode, tinyxml has no idea what a letter *is*. The
    // algorithm is generous.
    //
    // After that, they can be letters, underscores, numbers,
    // hyphens, or colons. (Colons are valid ony for namespaces,
    // but tinyxml can't tell namespaces from names.)
    if ( p && *p && ( isalpha( (unsigned char) *p ) || *p == '_' ) )
    {
        const char* start = p;
        while( p && *p && (     IsAlphaNum( (unsigned char ) *p )
                                || *p == '_' || *p == '-' || *p == '.' || *p == ':' ) )
        {
            ++p;
        }
        if ( p-start > 0 )
        {
            name->assign( start, p-start );
        }
        return p;
    }
    return 0;
}

template<typename T>
const char* IHT<T>::ReadText(const char* p, XML_STRING * text,
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
const char* IHT<T>::ReadValue(const char* p, Node *node)
{
    // Read in text and elements in any order.
    p = SkipWhiteSpace(p);

    Node* currNode;
    parameterParsingInfo *pParamInfo;

    while ( p && *p )
    {
        if( *p != '<')
        {
            //std::cout <<"[" << node->__handle << "]"<< "ReadValue PraseObjectClass() in (p != '<')" << std::endl;
            p = ParseInteractionClass(p, node);
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
                //std::cout << " in IHT<T>::ReadValue==> IdentifyNode()" << std::endl;
                int id  = IdentifyNode(p);
                switch (id)
                {
                case type_interactionCollection:
                    //std::cout << "<Interatctions>" << std::endl;
                    while ( p && *p && *p != '>' )
                    {
                        ++p;
                    }
                    ++p;
                    break;

                case type_parameter:
                    //paramInfo = new parameterParsingInfo();
                    //currNode->__parsing_info->parameterParsingInfoList = &paramInfo;

                    // Get the parameterParsingInfo data
                    //....
                    //pParamInfo = new parameterParsingInfo;
                    //node->__biggest_handle++;
                    pParamInfo = new parameterParsingInfo;
                    //node->__parsing_info->parameterParsingInfoList[node->__biggest_handle] = pParamInfo;
                    addParameterInfo(node, pParamInfo);
                    //node->__parsing_info->parameterParsingInfoList[node->__biggest_handle];
                    p = ParseParameterNode(p, node, pParamInfo);
                    break;
                case type_interactionclass:

                    //std::cout <<"[" << node->__handle << "]" << "find a type_interactionclass"<< std::endl;
                    currNode = new Node;
                    nodeCounter++;
                    currNode->__handle = nodeCounter;
		    currNode->rti_handle = new RTI_InteractionClassHandle(currNode->__handle);
                    nodeArray[nodeCounter] = currNode;
                    addNode(node, currNode);
                    addInheriteParameter(currNode);
                    p = ParseInteractionClass(p,currNode);
                    break;
                case type_error:
                    //std::cout << " in IHT<T>::ReadValue==> type_error!!" << std::endl;
                    return 0;
                default:
                    //std::cout << " in IHT<T>::ReadValue==> default!!, id:"<< id << std::endl;
                    while ( p && *p && *p != '>' )
                    {
                        ++p;
                    }
                    ++p; //skip ">"

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
const char* IHT<T>::ParseParameter(const char* p, XML_STRING* parameterName, XML_STRING* parameterValue)
{

    p = SkipWhiteSpace(p);
    if ( !p || !*p ) return 0;

    // Read the name, the '=' and the value.f
    p = ReadName(p, parameterName);

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
        p = ReadText(p, parameterValue, false, end, false);
    }
    else if ( *p == DOUBLE_QUOTE )
    {
        ++p;
        end = "\"";             // double quote in string

        p = ReadText(p, parameterValue, false, end, false);

    }
    else
    {
        // All Parameter values should be in single or double quotes.
        // But this is such a common error that the parser will try
        // its best, even without them.

        *parameterValue = "";
        while (    p && *p                                                                                      // existence
                   && !IsWhiteSpace( *p ) && *p != '\n' && *p != '\r'      // whitespace
                   && *p != '/' && *p != '>' )                                                     // tag end
        {
            if ( *p == SINGLE_QUOTE || *p == DOUBLE_QUOTE )
            {
                // [ 1451649 ] Parameter values with trailing quotes not handled correctly
                // We did not have an opening quote but seem to have a
                // closing one. Give up and throw an error.
                return 0;
            }
            *parameterValue += *p;
            ++p;
        }

    }

    return p;
}

// �ѪR<IteractionClass���U�@�h��<Parameter> Tag, <Parameter> �ӼƤ��w�C
//ParameterParseInfo�HParameterParseInfoList��_��
template<typename T>
const char* IHT<T>::ParseParameterNode(const char* p, Node *currNode, parameterParsingInfo* paramInfo)
{
    //std::cout << "In ParseParameterNode())..." << std::endl;
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

    // Check for and read Parameters. Also look for an empty
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
            // Done with Parameters (if there were any.)
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
            // Try to read an Parameter:
            XML_STRING paramName, paramValue;

            p = ParseParameter(p, &paramName, &paramValue);


            if ( !p || !*p )
            {
                return 0;
            }


            //�s�W�@structure parameterParsingInfo�A�Nparse����Ʃ�Jstructure
            char* n = paramName.c_str();

            //std::cout << "Parameter => " << std::endl;
            if ( StringEqual(n, "name", false) )
            {
                paramInfo->name = new char[paramValue.size()+1];
                strcpy(paramInfo->name , paramValue.c_str());

		paramInfo->rti_name = new RTI_wstring(StrtoWStr(paramInfo->name));
                //std::cout << "name :" << paramValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "dataType", false) )
            {
                paramInfo->dataType = new char[paramValue.size()+1];
                strcpy(paramInfo->dataType , paramValue.c_str());
                //std::cout << "dataType :" << paramValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "semantics", false) )
            {
                paramInfo->semantics = new char[paramValue.size()+1];
                strcpy(paramInfo->semantics , paramValue.c_str());
                //std::cout << "semantics :" << paramValue.c_str() << std::endl;
            }
        }
    }


    return p;
}


template<typename T>
const char* IHT<T>::ParseInteractionClass(const char* p, Node *node)
{
    //std::cout << std::endl;
    //std::cout << "in IHT::ParseInteractionClass()... =>" << *p << std::endl;
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


            //std::cout <<"[" << node->__handle << "]" << "find the end tag.1(<" << endTag.c_str() << ".../>) p = "<< &p << ", node->__parsing_info->name = " << node->__parsing_info->name<< std::endl;

            return (p+1);
        }
        else if ( *p == '>' )
        {
            // Done with Parameters (if there were any.)
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

                //std::cout <<"[" << node->__handle << "]"  << "find the end tag.2(<" << endTag.c_str() << "/>) p = "<< &p << ", node->__parsing_info->name = " << node->__parsing_info->name << std::endl;

                return p;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            // ��XParameter Name�� Value
            XML_STRING paramName, paramValue;

            p = ParseParameter(p, &paramName, &paramValue);

            //�Yp���ũάOp���e���šA���}function
            if ( !p || !*p )
            {
                return 0;
            }

            char* n = paramName.c_str();

            //std::cout << "parameterValue.c_str() : " << paramValue.c_str() << std:: endl;

            //�x�sObjectClass Name ��Parameter�������
            if ( StringEqual(n, "name", false) )
            {
                node->__parsing_info->name = new char[paramValue.size()+1];
                strcpy(node->__parsing_info->name, paramValue.c_str());

		node->__parsing_info->rti_name = new RTI_wstring(StrtoWStr(node->__parsing_info->name));
                //std::cout << "Name : " << paramValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "sharing", false) )
            {
                node->__parsing_info->sharing = new char[paramValue.size()+1];
                strcpy(node->__parsing_info->sharing, paramValue.c_str());
                //std::cout << "sharing : " << paramValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "dimensions", false) )
            {
                node->__parsing_info->dimensions = new char[paramValue.size()+1];
                strcpy(node->__parsing_info->dimensions, paramValue.c_str());
                //std::cout << "dimensions : " << paramValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "transportation", false) )
            {
                node->__parsing_info->transportation = new char[paramValue.size()+1];
                strcpy(node->__parsing_info->transportation, paramValue.c_str());
                //std::cout << "transportation : " << paramValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "order", false) )
            {
                node->__parsing_info->order = new char[paramValue.size()+1];
                strcpy(node->__parsing_info->order, paramValue.c_str());
                //std::cout << "order : " << paramValue.c_str() << std::endl;
            }
            else if ( StringEqual(n, "semantics", false) )
            {
                node->__parsing_info->semantics = new char[paramValue.size()+1];
                strcpy(node->__parsing_info->semantics, paramValue.c_str());
                //std::cout << "semantics : " << paramValue.c_str() << std::endl;
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
bool IHT<T>::read_XMLfile(char* fullPathFileName)
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
bool IHT<T>::parsing()
{
    //std::cout << "IHT::parsing()" << std::endl;
    if (!this->file_content)
    {
        return false;
    }

    // Initialize var
    //gLocation.Clear();
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
    //std::cout << "p:" << p << "lastPos:" << lastPos << ", p-lastPos :" << p-lastPos << std::endl;
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
        //std::cout << "id = " << id << std::endl;
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
            //std::cout << "type_objectCollection..." << std::endl;
            p = SkipObjects(p);
            break;
        case type_endObjectCollection:
            while ( p && *p && *p != '>' )
            {
                ++p;
            }
            ++p; //skip ">"
            break;
        case type_endInteractionCollection:
            p = 0;
            break;
        case type_interactionCollection:
            //skip all these content
            //std::cout << "type_interactionCollection..." << id << std::endl;
            while ( p && *p && *p != '>' )
            {
                ++p;
            }
            ++p; //skip ">"

            break;
        case type_parameter:
            //std::cout << "type_Parameter..." << std::endl;
            p = SkipTag(p);
            break;
        case type_interactionclass:
            node = new Node();
            nodeCounter++;
            node->__handle = nodeCounter;
	    node->rti_handle = new RTI_InteractionClassHandle(node->__handle);
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
            p = ParseInteractionClass(p, node);
            break;

        case  type_error:
            //std::cout << "type_error" << std::endl;
            //p = SkipTag(p);
            return false;
        default:
            //std::cout << "default..." << std::endl;
            //p = SkipTag(p);
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
void IHT<T>::addParameterInfo(Node * node, parameterParsingInfo* paramInfo)
{

    //printf("__parsing_info->name = %s\n" , node->__parsing_info->name);
    //printf("addParameterInfo : \n" );

    //std::cout << "paramInfo = " << paramInfo << std::endl;
    node->__biggest_handle++;
    node->__parsing_info->parameterParsingInfoList[node->__biggest_handle] = paramInfo;
    //std::cout << "add an ParameterInfo = " << node->__parsing_info->parameterParsingInfoList[node->__biggest_handle]->name << std::endl;
}

template<typename T>
void IHT<T>::addNode(Node * parentNode, Node * childNode)
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
        exit(1);
    }

}


template<typename T>
void  IHT<T>::assignTailValue()
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
void  IHT<T>::addInheriteParameter(Node *node)
{
    Node *parentNode;

    parameterParsingInfo * pParamInfo;
    unsigned long biggest_handle;

    //std::cout << "Add Inherite parameterParsingInfo..." << std::endl;

    //���parent�`�I�A���oparamInfoNo, parameterParsingInfoList
    parentNode = node->__parent;
    biggest_handle = parentNode->__biggest_handle;
    node->__inherited_param_no = biggest_handle;

    //std::cout << "biggest_handle = " << biggest_handle << std::endl;

    for (int i = 1; i <= biggest_handle; i++)
    {
        pParamInfo = parentNode->__parsing_info->parameterParsingInfoList[i];

        //std::cout << "Parameter = " << pParamInfo->name << std::endl;

        addParameterInfo(node, pParamInfo);
    }
}
template <typename T>
void IHT<T>::treeReAllocation()
{

    Node *pNode;
    Node **newNodeArray;
    Node **newChildList;
    parameterParsingInfo **newparamInfoList;
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

            //���s�إߥثe�`�I��parameterParsingInfoList���а}�C
            biggest_handle = pNode->__biggest_handle;

            newparamInfoList = new parameterParsingInfo* [biggest_handle+1];

            for(int k = 1; k <= biggest_handle; k++)
            {
                //�ƻsarrtibuteParsingInfoList�ܷs�����а}�C
                newparamInfoList[k] = pNode->__parsing_info->parameterParsingInfoList[k];
            }
            //arrtibuteParsingInfoList�ƻs�����AparameterParsingInfoList����O����
            delete [] pNode->__parsing_info->parameterParsingInfoList;
            pNode->__parsing_info->parameterParsingInfoList = newparamInfoList;
        }

        delete  [] pNode->__child_List;
        pNode->__child_List = newChildList;

    }
    delete  [] nodeArray;
    nodeArray = newNodeArray;

}


}

#endif
