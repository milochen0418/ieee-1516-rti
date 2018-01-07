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
    String myStr;
    //*************************************************************************
    //* OHT parseing()測試，檔案 RestaurantSOM.xml
    //*************************************************************************
    //--------------RestaurantSOM.xml--------------
    std::cout << std::endl;
    std::cout << " Reading " << filename << "..." << std::endl;
    if(!parser->read_XMLfile(filename))
    {
        std::cout << "檔案讀取錯誤：" << std::endl;
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
    //* OHT iterator()測試...
    //*************************************************************************
    std::cout << "測試OHT<int>::iterator..." << std::endl;
    OHT<int>::iterator OHT_iter(parser->root, parser->root);
    OHT<int>::iterator iter;

    for(iter=parser->begin(OHT_iter); iter != parser->end(OHT_iter); iter++)
    {

        if(*iter != 0)
        {
            //*************************************************************************
            //* OHT method 4 getObjectClassName()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 4 getObjectClassName()測試----------" << std::endl;
            String objectClaassName = parser->getObjectClassName((*iter)->__handle);
            std::cout << "Class ["<< setw(2)<< (*iter)->__handle << "]->name = " << objectClaassName <<endl;
            //std::cout << "getObjectClassName(-1) = " << parser->getObjectClassName(-1) << std::endl;
            std::cout << "getObjectClassName(0) = " << parser->getObjectClassName(0) << std::endl;
            std::cout << "getObjectClassName(__handle + 100) = " << parser->getObjectClassName(((*iter)->__handle) + 100) << std::endl;
            std::cout << "parser->getObjectClassName(0xFFFFFFFF) = "<< parser->getObjectClassName(0xFFFFFFFF) << std::endl;
            //std::cout << "getObjectClassName(-1) = " << parser->getObjectClassName(NULL) << std::endl;//不能用
            std::cout << "----------------------------------------------" << std::endl;


            std::cout << "Level : " << (*iter)->__level << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method 5 getObjectClassHandle()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 5 getObjectClassHandle()測試----------" << std::endl;
            std::cout << "getObjectClassHandle(" << objectClaassName.c_str() << ") = " << parser->getObjectClassHandle(objectClaassName.c_str()) << std::endl;
            std::cout << "getObjectClassHandle(\"NULL\") = " << parser->getObjectClassHandle(NULL) << std::endl; 
            std::cout << "getObjectClassHandle(\"\") = " << parser->getObjectClassHandle("") << std::endl;
            std::cout << "getObjectClassHandle(\"\\n\") = " << parser->getObjectClassHandle("\n") << std::endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //****************************cout << "parser->getInheritedNumber(" << (*iter)->__handle << ") = "<< OC_No << endl;*********************************************
            //* OHT method 11 isObjectClass()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 11 isObjectClass()測試----------" << std::endl;
            std::cout << "isObjectClass(" << parser->getObjectClassName((*iter)->__handle) << ") = " << (parser->isObjectClass(parser->getObjectClassName((*iter)->__handle))) << std::endl;
            std::cout << "isObjectClass(\"\\n\") = " << parser->isObjectClass("\n") << std::endl;
            std::cout << "isObjectClass(\"\") = " << parser->isObjectClass("") << std::endl;
            //std::cout << "isObjectClass(NULL) = " << parser->isObjectClass(NULL) << std::endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method 9 getInheritedNumber()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 9 getInheritedNumber()測試-----------" << std::endl;
            unsigned long OC_No;
            OC_No = parser->getInheritedNumber((*iter)->__handle);
            std::cout << "parser->getInheritedNumber(" << (*iter)->__handle << ") = "<< OC_No << std::endl;
                       std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 8 getObjectClass()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 8 getObjectClass()測試-----------" << std::endl;
            std::cout << "parser->getObjectClass(" << (*iter)->__handle << ") = "<< parser->getObjectClass((*iter)->__handle) << endl; 
            //std::cout << "parser->getObjectClass(-1) = "<< parser->getObjectClass(-1) << endl;
            std::cout << "parser->getObjectClass(0) = "<< parser->getObjectClass(0) << endl;
            std::cout << "parser->getObjectClass(" << ((*iter)->__handle) + 10 << ") = "<< parser->getObjectClass(((*iter)->__handle) + 10) << endl;
            std::cout << "parser->getObjectClass(0xFFFFFFFF) = "<< parser->getObjectClass(0xFFFFFFFF) << std::endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 10 isObjectClass()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 10 isObjectClass()測試-----------" << std::endl;
            cout << "parser->getObjectClass(" << objectClaassName.c_str() << ") = "<< (parser->isObjectClass(objectClaassName.c_str())?"True":"False") << endl;
            cout << "parser->getObjectClass(\"\\n\") = "<< (parser->isObjectClass("\n")?"True":"False") << endl;
            cout << "parser->getObjectClass(\"\") = "<< (parser->isObjectClass("")?"True":"False") << endl;
            cout << "parser->getObjectClass(myStr.c_str()) = "<< (parser->isObjectClass(myStr.c_str())?"True":"False") << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 13 getNumberOfAttribute()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 13 getNumberOfAttribute()測試-----------" << std::endl;

            unsigned long NoOfAttribute = parser->getNumberOfAttribute((*iter)->__handle);

            cout << "parser->getNumberOfAttribute(" << (*iter)->__handle << ") = "<<  NoOfAttribute << endl;
            cout << "parser->getNumberOfAttribute(0) = "<<  parser->getNumberOfAttribute(0) << endl;
            cout << "parser->getNumberOfAttribute(200) = "<<  parser->getNumberOfAttribute(200) << endl;
            cout << "parser->getNumberOfAttribute(0xFFFFFFFF) = "<<  parser->getNumberOfAttribute(0xFFFFFFFF) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* OHT method No 15 getNumberOfInheritedClass()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 15 getNumberOfInheritedClass()測試-----------" << std::endl;
            cout << "parser->getNumberOfInheritedClass(" << (*iter)->__handle << ") = "<<  parser->getNumberOfInheritedClass((*iter)->__handle) << endl;
            cout << "parser->getNumberOfInheritedClass(0) = "<<  parser->getNumberOfInheritedClass(0) << endl;
            cout << "parser->getNumberOfInheritedClass(200)"<<  parser->getNumberOfInheritedClass(200) << endl;
            cout << "parser->getNumberOfInheritedClass(0xFFFFFFFF) = "<<  parser->getNumberOfInheritedClass(0xFFFFFFFF) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;


            for( unsigned long i = 0; i <= NoOfAttribute+2; i++)
            {
                //*************************************************************************
                //* OHT method No 12 isInheritedClassAttribute()測試...
                //*************************************************************************
                std::cout << "----------OHT method No 12 isInheritedClassAttribute()測試-----------" << std::endl;
                std::cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle << ", " << i << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle, i)?"True":"False") << std::endl;
                std::cout << "parser->isInheritedClassAttribute(0xFFFFFFFF, " << i << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle, i)?"True":"False") << std::endl;
                std::cout << "parser->isInheritedClassAttribute(0xFFFFFFFF, 0xFFFFFFFF) = "<< (parser->isInheritedClassAttribute(0xFFFFFFFF, 0xFFFFFFFF)?"True":"False") << std::endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;
                //*************************************************************************
                //* OHT method No 6 getAttributeName()測試...
                //*************************************************************************
                std::cout << "----------OHT method No 6 getAttributeName()測試-----------" << std::endl;
                String attrName = parser->getAttributeName((*iter)->__handle, i);
                std::cout << "parser->getAttributeName(0xFFFFFFFF, " << i << ") = " << parser->getAttributeName(0xFFFFFFFF, i)<< endl;
                std::cout << "parser->getAttributeName(0xFFFFFFFF, 0xFFFFFFFF) = " << parser->getAttributeName(0xFFFFFFFF, 0xFFFFFFFF)<< endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;

                //*************************************************************************
                //* OHT method No 7 getAttributeHandle()測試...
                //*************************************************************************
                std::cout << "----------OHT method No 7 getAttributeHandle()測試-----------" << std::endl;
                cout << "parser->getAttributeHandle(" << (*iter)->__handle << ", " << attrName << ") = "<< parser->getAttributeHandle((*iter)->__handle, attrName.c_str()) << endl;
                cout << "parser->getAttributeHandle(0, " << attrName << ") = "<< parser->getAttributeHandle(0, attrName.c_str()) << endl;
                cout << "parser->getAttributeHandle(0, \"\") = "<< parser->getAttributeHandle(0, "") << endl;
                cout << "parser->getAttributeHandle(0, \"\\n\") = "<< parser->getAttributeHandle(0, "\n") << endl;
                cout << "parser->getAttributeHandle(0, NULL) = "<< parser->getAttributeHandle(0, NULL) << endl;
                cout << "parser->getAttributeHandle(xFFFFFFFF, \"\") = "<< parser->getAttributeHandle(0xFFFFFFFF, "") << endl;
                cout << "parser->getAttributeHandle(xFFFFFFFF, \"\\n\") = "<< parser->getAttributeHandle(0xFFFFFFFF, "\n") << endl;
                cout << "parser->getAttributeHandle(xFFFFFFFF, NULL) = "<< parser->getAttributeHandle(0xFFFFFFFF, NULL) << endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;
            }


            //*************************************************************************
            //* OHT method No 14 getNumberOfInheritedAttribute()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 14 getNumberOfInheritedAttribute()測試-----------" << std::endl;
            std::cout << "parser->getNumberOfInheritedAttribute(" << (*iter)->__handle << ") = "<< parser->getNumberOfInheritedAttribute((*iter)->__handle) << endl;
            std::cout << "parser->getNumberOfInheritedAttribute(0) = "<< parser->getNumberOfInheritedAttribute(0) << endl;
            std::cout << "parser->getNumberOfInheritedAttribute(0xFFFFFFFF) = "<< parser->getNumberOfInheritedAttribute(0xFFFFFFFF) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;



            //*************************************************************************
            //* OHT method No 16 getObjectClassIterator()測試...
            //*************************************************************************
            std::cout << "----------OHT method No 16 getObjectClassIterator()測試-----------" << std::endl;

            printf( "parser->getObjectClassIterator(%d) = %X\n", (*iter)->__handle, *parser->getObjectClassIterator((*iter)->__handle));
            printf( "parser->getObjectClassIterator(%d) = %X\n", 0, *parser->getObjectClassIterator((*iter)->__handle));
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
                //* OHT method No 3 isAncestorOf()測試...
                //*************************************************************************
                std::cout << "----------OHT method No 3 isAncestorOf()測試-----------" << std::endl;
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
    String str3("OMT TEST 3");
    String str4("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    String str5("OMT TEST 4");
    String str6("\n");
    String str7("");
    //*************************************************************************
    //* StrictCounter 測試…
    //*************************************************************************
    std::cout << "StrictCounter 測試..." << std::endl;

    std::cout << "建立一 StrictCounter  SC" << std::endl;
    StrictCounter SC;
    std::cout << "請輸入SC.requestID執行次數:";
    // std::cin >> no;
    no=5;
    //std::cout << "執行" << no <<"次SC.requestID() = " << std::endl;
    for(int i = 1; i <= no; i++)
    {
        std::cout << "第" << i << "次，SC.requestID結果=" << SC.requestID() << std::endl;
    }
    no=5;
    std::cout << "請輸入SC.releaseID執行次數:";
    //std::cin >> no;
    //std::cout << "執行" << no <<"次SC.requestID() = " << std::endl;
    for(int i = 1; i <= no; i++)
    {
        std::cout << "第" << i << "次，SC.releaseID" << std::endl;

        SC.releaseID(i);

    }
    //*************************************************************************
    //* StrictCounter 測試…
    //*************************************************************************
    //須修改uniqueStringcollection
    std::cout << "測試UniqueStringCollection..." << std::endl;
    UniqueStringCollection<String> usc(100);
    int i = 1;

    String* pStr1 = &str1;
    String* pStr2 = &str2;

    std::cout << "測試UniqueStringCollection.insert("<< str1.c_str() << ")..." << std::endl;
    usc.insert(str1.c_str(), &str2);

    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;

    std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< usc.isExist(str1.c_str()) << std::endl;

    std::cout << "UniqueStringCollection.query(str1.c_str()) = " << usc.query(str1.c_str()) << std::endl;


    std::cout << "測試UniqueStringCollection.insert("<< str2.c_str() << ")..." << std::endl;

    usc.insert(str2.c_str(), pStr2);
    std::cout << "UniqueStringCollection.isExist(" << str2.c_str() << ") = "<< usc.isExist(str2.c_str()) << std::endl;

    usc.insert(str3.c_str(), &str3);
    std::cout << "UniqueStringCollection.isExist(" << str3.c_str() << ") = "<< usc.isExist(str3.c_str()) << std::endl;

    usc.insert(str4.c_str(), &str4);
    std::cout << "UniqueStringCollection.isExist(" << str4.c_str() << ") = "<< usc.isExist(str4.c_str()) << std::endl;

    usc.insert(str5.c_str(), &str5);
    std::cout << "UniqueStringCollection.isExist(" << str5.c_str() << ") = "<< usc.isExist(str5.c_str()) << std::endl;

    usc.insert(str6.c_str(), &str6);
    std::cout << "UniqueStringCollection.isExist(" << str6.c_str() << ") = "<< usc.isExist(str6.c_str()) << std::endl;

    usc.insert(str7.c_str(), &str7);
    std::cout << "UniqueStringCollection.isExist(" << str7.c_str() << ") = "<< usc.isExist(str7.c_str()) << std::endl;


    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;

    std::cout << "UniqueStringCollection.isExist(" << str2.c_str() << ") = "<< (usc.isExist(str2.c_str())?"True":"False") << std::endl;

    std::cout << "UniqueStringCollection.query(str2.c_str()) = " << usc.query(str2.c_str()) << std::endl;


    std::cout << "測試UniqueStringCollection.cancel(" << str1.c_str() << ")..." << std::endl;
    usc.cancel(str1.c_str());
    std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< (usc.isExist(str1.c_str())?"True":"False") << std::endl;

    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;


    std::cout << "測試UniqueStringCollection.cancel(" << str2.c_str() << ")..." << std::endl;
    usc.cancel(str2.c_str());

    std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< (usc.isExist(str1.c_str())?"True":"False") << std::endl;

    std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;

    std::cout << "測試UniqueStringCollection.query(\"aaa\")..." << std::endl;
    std::cout << "usc.query(\"aaa\") = " << usc.query("aaa") << std::endl;

    //*************************************************************************
    //* OHT 測試…
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
