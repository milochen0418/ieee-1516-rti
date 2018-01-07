#include <iostream>
#include <cstdlib>
#include <time.h>

#include "OMT_Parser.h"
//#include "OHT.h"
//#include "UniqueStringCollection.h"
//#include "StrictCounter.h"
//#include "String.h"


using namespace std;
using namespace omt;

bool ohttest(OHT<int>* parser, char* filename)
{
    //*************************************************************************
    //* OHT parseing()���աA�ɮ� RestaurantSOM.xml
    //*************************************************************************
    //--------------RestaurantSOM.xml--------------
    std::cout << std::endl;
    std::cout << " Reading " << filename << "..." << std::endl;
    if(!parser->read_XMLfile(filename))
    {
        std::cout << "�ɮ�Ū�����~�G" << std::endl;
    }

    if(!parser->parsing())
    {
        std::cout << "Parseing Error!!" << std::endl;
    }
    else
    {
        std::cout << "Parseing Succeed!!" << std::endl;
    }

    //*************************************************************************
    //* OHT iterator()����...
    //*************************************************************************
    std::cout << "����OHT<int>::iterator..." << std::endl;
    OHT<int>::iterator OHT_iter(parser->root, parser->root);
    OHT<int>::iterator iter;

    for(iter=parser->begin(OHT_iter); iter != parser->end(OHT_iter); iter++)
    {

        if(*iter != 0)
        {



            //*************************************************************************
            //* OHT method 4 getObjectClassName()����...
            //*************************************************************************
            std::cout << "----------OHT method No 4 getObjectClassName()����----------" << std::endl;
            String objectClaassName = parser->getObjectClassName((*iter)->__handle);
            std::cout << "Class ["<< setw(2)<< (*iter)->__handle << "]->name = " << objectClaassName <<endl;
            std::cout << "----------------------------------------------" << std::endl;


            std::cout << "Level : " << (*iter)->__level << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method 5 getObjectClassHandle()����...
            //*************************************************************************
            std::cout << "----------OHT method No 5 getObjectClassHandle()����----------" << std::endl;
            std::cout << "getObjectClassHandle(" << objectClaassName.c_str() << ") = " << parser->getObjectClassHandle(objectClaassName.c_str()) << std::endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method 11 isObjectClass()����...
            //*************************************************************************
            std::cout << "----------OHT method No 11 isObjectClass()����----------" << std::endl;
            std::cout << "isObjectClass(" << parser->getObjectClassName((*iter)->__handle) << ") = " << (parser->isObjectClass(parser->getObjectClassName((*iter)->__handle))?"True":"False") << std::endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method 9 getInheritedNumber()����...
            //*************************************************************************
            std::cout << "----------OHT method No 9 getInheritedNumber()����-----------" << std::endl;
            unsigned long OC_No;
            OC_No = parser->getInheritedNumber((*iter)->__handle);
            cout << "parser->getInheritedNumber(" << (*iter)->__handle << ") = "<< OC_No << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 8 getObjectClass()����...
            //*************************************************************************
            std::cout << "----------OHT method No 8 getObjectClass()����-----------" << std::endl;
            cout << "parser->getObjectClass(" << (*iter)->__handle << ") = "<< parser->getObjectClass((*iter)->__handle) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 10 isObjectClass()����...
            //*************************************************************************
            std::cout << "----------OHT method No 10 isObjectClass()����-----------" << std::endl;
            cout << "parser->getObjectClass(" << objectClaassName.c_str() << ") = "<< (parser->isObjectClass(objectClaassName.c_str())?"True":"False") << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 13 getNumberOfAttribute()����...
            //*************************************************************************
            std::cout << "----------OHT method No 13 getNumberOfAttribute()����-----------" << std::endl;

            unsigned long NoOfAttribute = parser->getNumberOfAttribute((*iter)->__handle);

            cout << "parser->getNumberOfAttribute(" << (*iter)->__handle << ") = "<<  NoOfAttribute << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 15 getNumberOfInheritedClass()����...
            //*************************************************************************
            std::cout << "----------OHT method No 15 getNumberOfInheritedClass()����-----------" << std::endl;
            cout << "parser->getNumberOfInheritedClass(" << (*iter)->__handle << ") = "<<  parser->getNumberOfInheritedClass((*iter)->__handle) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;


            for( unsigned long i = 1; i <= NoOfAttribute; i++)
            {
                //*************************************************************************
                //* OHT method No 12 isInheritedClassAttribute()����...
                //*************************************************************************
                std::cout << "----------OHT method No 12 isInheritedClassAttribute()����-----------" << std::endl;
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle << ", " << i << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle, i)?"True":"False") << endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;
                //*************************************************************************
                //* OHT method No 6 getAttributeName()����...
                //*************************************************************************
                std::cout << "----------OHT method No 6 getAttributeName()����-----------" << std::endl;
                String attrName = parser->getAttributeName((*iter)->__handle, i);
                std::cout << "parser->getAttributeName(" << (*iter)->__handle << ", " << i << ") = " << attrName.c_str() << endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;

                //*************************************************************************
                //* OHT method No 7 getAttributeHandle()����...
                //*************************************************************************
                std::cout << "----------OHT method No 7 getAttributeHandle()����-----------" << std::endl;
                cout << "parser->getAttributeHandle(" << (*iter)->__handle << ", " << attrName << ") = "<< parser->getAttributeHandle((*iter)->__handle, attrName.c_str()) << endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;
            }


            //*************************************************************************
            //* OHT method No 14 getNumberOfInheritedAttribute()����...
            //*************************************************************************
            std::cout << "----------OHT method No 14 getNumberOfInheritedAttribute()����-----------" << std::endl;
            cout << "parser->getNumberOfInheritedAttribute(" << (*iter)->__handle << ") = "<< parser->getNumberOfInheritedAttribute((*iter)->__handle) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;



            //*************************************************************************
            //* OHT method No 16 getObjectClassIterator()����...
            //*************************************************************************
            std::cout << "----------OHT method No 16 getObjectClassIterator()����-----------" << std::endl;
            printf( "parser->getObjectClassIterator(%d) = %X\n", (*iter)->__handle, *parser->getObjectClassIterator((*iter)->__handle));
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;


            unsigned long OC_No1;
            OC_No1 = parser->getInheritedNumber((*iter)->__handle);
            srand(time(NULL));
            for(int j = 1; j <= 10; j++ )
            {
                int rand1 = (rand()%OC_No1) + 1;
                int rand2 = (rand()%OC_No1) + 1;
                //*************************************************************************
                //* OHT method No 3 isAncestorOf()����...
                //*************************************************************************
                std::cout << "----------OHT method No 3 isAncestorOf()����-----------" << std::endl;
                printf( "parser->isAncestorOf(%d, %d) = %d\n", rand1, rand2, parser->isAncestorOf(rand1, rand2));
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;

            }

            std::cout << std::endl << std::endl << std::endl << std::endl;

        }
    }
}


int main(int argc, char *argv[])
{

    int no;
    String str1("OHT TEST 1");
    String str2("OHT TEST 2");
    //*************************************************************************
    //* StrictCounter ���աK
    //*************************************************************************
    std::cout << "StrictCounter ����..." << std::endl;

    std::cout << "�إߤ@ StrictCounter  SC" << std::endl;
    StrictCounter SC;
    std::cout << "�п�JSC.requestID���榸��:";
    // std::cin >> no;
    no=5;
    //std::cout << "����" << no <<"��SC.requestID() = " << std::endl;
    for(int i = 1; i <= no; i++)
    {
        std::cout << "��" << i << "���ASC.requestID���G=" << SC.requestID() << std::endl;
    }
    no=5;
    std::cout << "�п�JSC.releaseID���榸��:";
    //std::cin >> no;
    //std::cout << "����" << no <<"��SC.requestID() = " << std::endl;
    for(int i = 1; i <= no; i++)
    {
        std::cout << "��" << i << "���ASC.releaseID" << std::endl;

        SC.releaseID(i);

    }
    //*************************************************************************
    //* StrictCounter ���աK
    //*************************************************************************
    //���ק�uniqueStringcollection
    std::cout << "����UniqueStringCollection..." << std::endl;
    UniqueStringCollection<String> usc(100);
    int i = 1;

    String* pStr1 = &str1;
    String* pStr2 = &str2;

    std::cout << "����UniqueStringCollection.insert("<< str1.c_str() << ")..." << std::endl;
    usc.insert(str1.c_str(), &str2);

    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;

    std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< usc.isExist(str1.c_str()) << std::endl;

    std::cout << "UniqueStringCollection.query(str1.c_str()) = " << usc.query(str1.c_str()) << std::endl;


    std::cout << "����UniqueStringCollection.insert("<< str2.c_str() << ")..." << std::endl;
    usc.insert(str2.c_str(), pStr2);

    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;

    std::cout << "UniqueStringCollection.isExist(" << str2.c_str() << ") = "<< (usc.isExist(str2.c_str())?"True":"False") << std::endl;

    std::cout << "UniqueStringCollection.query(str2.c_str()) = " << usc.query(str2.c_str()) << std::endl;


    std::cout << "����UniqueStringCollection.cancel(" << str1.c_str() << ")..." << std::endl;
    usc.cancel(str1.c_str());
    std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< (usc.isExist(str1.c_str())?"True":"False") << std::endl;

    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;


    std::cout << "����UniqueStringCollection.cancel(" << str1.c_str() << ")..." << std::endl;
    usc.cancel(str2.c_str());

    std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< (usc.isExist(str1.c_str())?"True":"False") << std::endl;

    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;

    std::cout << "����UniqueStringCollection.query(\"aaa\")..." << std::endl;
    std::cout << "usc.query(\"aaa\") = " << usc.query("aaa") << std::endl;

    //*************************************************************************
    //* OHT ���աK
    //*************************************************************************
    OHT<int>* oht1 = new OHT<int>();
    OHT<int>* oht2 = new OHT<int>();
    OHT<int>* oht3 = new OHT<int>();

    ohttest(oht1, "JTFpFOM.xml");
    ohttest(oht2, "JTFpFOM_MOM.xml");
    ohttest(oht3, "RestaurantSOM.xml");
    std::cout << "TESTING END...." << std::endl;



    return EXIT_SUCCESS;
}
