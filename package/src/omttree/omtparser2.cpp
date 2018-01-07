

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <time.h>

#include "OMT_Parser.h"
//#include "IHT.h"
//#include "String.h"


using namespace std;
using namespace omt;

bool IHTtest(IHT<int>* parser, char* filename)
{
    //*************************************************************************
    //* IHT parseing()���աA�ɮ� RestaurantSOM.xml
    //*************************************************************************
    //--------------RestaurantSOM.xml--------------
    std::cout << std::endl;
    std::cout << " Reading " << filename << "..." << std::endl;
    if (!parser->read_XMLfile(filename))
    {
        std::cout << "�ɮ�Ū�����~�G" << std::endl;
    }

    if (!parser->parsing())
    {
        std::cout << "Parseing Error!!" << std::endl;
    }
    else
    {
        std::cout << "Parseing Succeed!!" << std::endl;
    }

    //*************************************************************************
    //* IHT iterator()����...
    //*************************************************************************
    std::cout << "����IHT<int>::iterator..." << std::endl;
    IHT<int>::iterator IHT_iter(parser->root, parser->root);
    IHT<int>::iterator iter;

    for (iter=parser->begin(IHT_iter); iter != parser->end(IHT_iter); iter++)
    {

        if (*iter != 0)
        {

            //*************************************************************************
            //* IHT method 4 getInteractionClassName()����...
            //*************************************************************************
            std::cout << "----------IHT method No 4 getInteractionClassName()����----------" << std::endl;
            String objectClaassName = parser->getInteractionClassName((*iter)->__handle);
            std::cout << "Class ["<< setw(2)<< (*iter)->__handle << "]->name = " << objectClaassName <<endl;
            std::cout << "----------------------------------------------" << std::endl;


            std::cout << "Level : " << (*iter)->__level << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* IHT method 5 getInteractionClassHandle()����...
            //*************************************************************************
            std::cout << "----------IHT method No 5 getInteractionClassHandle()����----------" << std::endl;
            std::cout << "getInteractionClassHandle(" << objectClaassName.c_str() << ") = " << parser->getInteractionClassHandle(objectClaassName.c_str()) << std::endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* IHT method 11 isInteractionClass()����...
            //*************************************************************************
            std::cout << "----------IHT method No 11 isInteractionClass()����----------" << std::endl;
            std::cout << "isInteractionClass(" << parser->getInteractionClassName((*iter)->__handle) << ") = " << (parser->isInteractionClass(parser->getInteractionClassName((*iter)->__handle))?"True":"False") << std::endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* IHT method 9 getInheritedNumber()����...
            //*************************************************************************
            std::cout << "----------IHT method No 9 getInheritedNumber()����-----------" << std::endl;
            unsigned long OC_No;
            OC_No = parser->getInheritedNumber((*iter)->__handle);
            cout << "parser->getInheritedNumber(" << (*iter)->__handle << ") = "<< OC_No << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* IHT method No 8 getInteractionClass()����...
            //*************************************************************************
            std::cout << "----------IHT method No 8 getInteractionClass()����-----------" << std::endl;
            cout << "parser->getInteractionClass(" << (*iter)->__handle << ") = "<< parser->getInteractionClass((*iter)->__handle) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* IHT method No 10 isInteractionClass()����...
            //*************************************************************************
            std::cout << "----------IHT method No 10 isInteractionClass()����-----------" << std::endl;
            cout << "parser->getInteractionClass(" << objectClaassName.c_str() << ") = "<< (parser->isInteractionClass(objectClaassName.c_str())?"True":"False") << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* IHT method No 13 getNumberOfParameter()����...
            //*************************************************************************
            std::cout << "----------IHT method No 13 getNumberOfParameter()����-----------" << std::endl;

            unsigned long NoOfParameter = parser->getNumberOfParameter((*iter)->__handle);

            cout << "parser->getNumberOfParameter(" << (*iter)->__handle << ") = "<<  NoOfParameter << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            //*************************************************************************
            //* IHT method No 15 getNumberOfInheritedClass()����...
            //*************************************************************************
            std::cout << "----------IHT method No 15 getNumberOfInheritedClass()����-----------" << std::endl;
            cout << "parser->getNumberOfInheritedClass(" << (*iter)->__handle << ") = "<<  parser->getNumberOfInheritedClass((*iter)->__handle) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;


            for ( unsigned long i = 1; i <= NoOfParameter; i++)
            {
                //*************************************************************************
                //* IHT method No 12 isInheritedClassParameter()����...
                //*************************************************************************
                std::cout << "----------IHT method No 12 isInheritedClassParameter()����-----------" << std::endl;
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle << ", " << i << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle, i)?"True":"False") << endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;
                //*************************************************************************
                //* IHT method No 6 getParameterName()����...
                //*************************************************************************
                std::cout << "----------IHT method No 6 getParameterName()����-----------" << std::endl;
                String attrName = parser->getParameterName((*iter)->__handle, i);
                std::cout << "parser->getParameterName(" << (*iter)->__handle << ", " << i << ") = " << attrName.c_str() << endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;

                //*************************************************************************
                //* IHT method No 7 getParameterHandle()����...
                //*************************************************************************
                std::cout << "----------IHT method No 7 getParameterHandle()����-----------" << std::endl;
                cout << "parser->getParameterHandle(" << (*iter)->__handle << ", " << attrName << ") = "<< parser->getParameterHandle((*iter)->__handle, attrName.c_str()) << endl;
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << std::endl;
            }


            //*************************************************************************
            //* IHT method No 14 getNumberOfInheritedParameter()����...
            //*************************************************************************
            std::cout << "----------IHT method No 14 getNumberOfInheritedParameter()����-----------" << std::endl;
            cout << "parser->getNumberOfInheritedParameter(" << (*iter)->__handle << ") = "<< parser->getNumberOfInheritedParameter((*iter)->__handle) << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;



            //*************************************************************************
            //* IHT method No 16 getInteractionClassIterator()����...
            //*************************************************************************
            std::cout << "----------IHT method No 16 getInteractionClassIterator()����-----------" << std::endl;
            printf( "parser->getInteractionClassIterator(%d) = %X\n", (*iter)->__handle, *parser->getInteractionClassIterator((*iter)->__handle));
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;


            unsigned long OC_No1;
            OC_No1 = parser->getInheritedNumber((*iter)->__handle);
            srand(time(NULL));
            for (int j = 1; j <= 10; j++ )
            {
                int rand1 = (rand()%OC_No1) + 1;
                int rand2 = (rand()%OC_No1) + 1;
                //*************************************************************************
                //* IHT method No 3 isAncestorOf()����...
                //*************************************************************************
                std::cout << "----------IHT method No 3 isAncestorOf()����-----------" << std::endl;
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

    //*************************************************************************
    //* IHT ���աK
    //*************************************************************************
    IHT<int>* IHT1 = new IHT<int>();
    IHT<int>* IHT2 = new IHT<int>();
    IHT<int>* IHT3 = new IHT<int>();

    IHTtest(IHT1, "JTFpFOM.xml");
    IHTtest(IHT2, "JTFpFOM_MOM.xml");
    IHTtest(IHT3, "RestaurantSOM.xml");
    std::cout << "TESTING END...." << std::endl;
    return EXIT_SUCCESS;
}
