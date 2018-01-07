

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <time.h>

//#pragma implementation "UniqueStringCollection.h"
//#pragma implementation "OHT.h"

#include "OMT_Parser.h"
//#include "OHT.h" 
//#include "OHT.cpp" 
//#include "UniqueStringCollection.h"
//#include "StrictCounter.h"
//#include "String.h"


using namespace std;
using namespace omt;
class miloClass;
//*--------------------- miloDefine Class ------------------------------

class miloClass{
public:
    miloClass(const char* theString){
        int i=0;
        int j=0;
        while(theString[i++]!='\0');
        _msg=new char[i];
        for(j=0;j<i;j++)_msg[j]=theString[j];
    }

    ~miloClass(){
        delete _msg;
    }

    char* _msg;
};

//-------------------------------------------------------------------*/



bool ohttest(OHT<int>* parser, char* filename)
{


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
    OHT<int>::iterator iter2;
    //---------------------------START of Milo's Iterator Testing------------------------------------------------
    for(iter=parser->begin(OHT_iter);iter!=parser->end(OHT_iter);iter++){
        std::cout<<"\n\n\n\n\nDiscuss the Handle:"<<(*iter)->__handle<<std::endl<<std::endl;
        OHT<int>::iterator tempiter;
        tempiter=iter;
        tempiter.root_here();
        for(iter2=parser->begin(tempiter);iter2!=parser->end(tempiter);iter2++){
            std::cout<<"\thandle="<<(*iter2)->__handle<<std::endl;
        }
    }
    //----------------------------END of Milo's Iterator Testing------------------------------------------------*/




    /*---------------------------START of Milo's getObjectClassName Testing------------------------------------------------
     
     	unsigned long m4idx;
    for(m4idx=0;m4idx<=90;m4idx++){
    std::cout<<"getObjectClassName: "<<m4idx<<" |-> "<<parser->getObjectClassName(m4idx)<<std::endl;
    }
      	
    //----------------------------END of Milo's getObjectClassName Testing------------------------------------------------*/

    /*---------------------------START of Milo's setting something for getObjectClass of OHT<miloClass>------------------------------------------------

    for(iter=parser->begin(OHT_iter);iter!=parser->end(OHT_iter);iter++){
    //(*iter)->__data = new miloClass(parser->getObjectClassName((*iter)->__handle));
    //(*iter)->__data = new miloClass("abc");
    }
    //----------------------------END of Milo's setting something for getObjectClass of OHT<miloClass>------------------------------------------------*/


    //----------------------------START of Milo's getAttributeName Testing------------------------------------------------



    //if Attribut is not here then the AttributeName will return "\n" even if when OChandle is illegal, beside,
    //Segment Fault will happen whenever OChandle is legal and attributeHandle = 0
    cout<<"getAttributeName Testing \n almost testing"<<endl;
    for(iter=parser->begin(OHT_iter);iter!=parser->end(OHT_iter);iter++){

        OHT<int>::iterator tmpiterator=iter;
        tmpiterator.up();
        unsigned long attridx;
        for(attridx=1;attridx<=parser->getNumberOfAttribute((*iter)->__handle)+10;attridx++){
            if(parser->isInheritedClassAttribute((*iter)->__handle, attridx)){
                if(strcmp( parser->getAttributeName((*iter)->__handle, attridx)  , parser->getAttributeName((*tmpiterator)->__handle,attridx))){
                    std::cout<<"attributeHandle="<<attridx <<"|-> Wrong because difference between "<< (*iter)->__handle  <<" and "<< (*tmpiterator)->__handle  <<std::endl;
                }else{
                    std::cout<<"OutOfAttrRange("<< attridx<<") |->"<<parser->getAttributeName((*iter)->__handle, attridx)<<std::endl;
                }

            }else{

                std::cout<<"is Not Inherited("<<parser->getObjectClassName((*iter)->__handle)<<","<<(*iter)->__handle<<"," <<attridx <<") |-> "<<parser->getAttributeName((*iter)->__handle,attridx)<<endl;

            }
        }
    }

    cout<<"getAttributeName Testing\n OChandle Boundry Testing"<<endl;
    cout<<"getAttributeName(0,0) = "<<parser->getAttributeName(0,0)<<endl;	//return "\n"
    cout<<"getAttributeName(0,1) = "<<parser->getAttributeName(0,1)<<endl;	//return "\n"
    cout<<"getAttributeName(1,0) = "<<parser->getAttributeName(1,0)<<endl;//Segment Fault
    cout<<"getAttributeName(1,20) = "<<parser->getAttributeName(1,20)<<endl;//return "\n"
    cout<<"getAttributeName(1,1) = "<<parser->getAttributeName(1,1)<<endl;	//return HLAprivilegeToDeleteObject
    cout<<"getAttributeName(40,0) = "<<parser->getAttributeName(40,0)<<endl;//Segment Fault
    cout<<"getAttributeName(41,0) = "<<parser->getAttributeName(41,0)<<endl;//return "\n"
    cout<<"getAttributeName(41,1) = "<<parser->getAttributeName(41,1)<<endl;//return "\n"




    //---------------------------------END of Milo's getAttributeName Testing----------------------------------------*/

    //--------------------------------START of Milo's getAttributeHandle Testing----------------------------------------
    //inside domain variable testing
    cout<<"\n\n\n\n\n\ninside domain variable testing"<<endl;
    cout<<"getAttributeHandle Testing\n inside domain variable testing"<<endl;
    for(iter=parser->begin(OHT_iter);iter!=parser->end(OHT_iter);iter++){
        unsigned long attridx;
        for(attridx=1;attridx<=parser->getNumberOfAttribute((*iter)->__handle);attridx++){
            //when attridx is out of range; if attridx=0 then Segment Fault, else return false
            cout<<"{OC="<<(*iter)->__handle<<",attr="<<attridx<<"} |-> ";
            cout<< (( (attridx)==parser->getAttributeHandle((*iter)->__handle, parser->getAttributeName((*iter)->__handle, attridx) ))?"Correct":"Error");
            cout<<"   ....attridx="<<attridx<<",  getAttributeHandle(...)="<< parser->getAttributeHandle((*iter)->__handle, parser->getAttributeName((*iter)->__handle, attridx) )  <<endl;

            cout<<"{OC="<<0<<",attr="<<attridx<<"} |-> ";
            cout<< parser->getAttributeHandle(0, parser->getAttributeName((*iter)->__handle, attridx) )<<endl;

            cout<<"{OC="<<60<<",attr="<<attridx<<"} |-> ";
            cout<< parser->getAttributeHandle(60, parser->getAttributeName((*iter)->__handle, attridx) )<<endl;
        }
    }
    cout<<"\n\n\n\n\n\ninside and outside domain constant testing";
    //inside and outside domain constant testing
    //The wrong is happen when OChandle is correct and the attributeName is not the attribute of this OChandle
    //The warnning is that the type of return value of getAttributeHandle is 'int' but not 'unsinged long'
    //The Segment Fault will happen when OChandle is legal and  (char*)attributeName is given by NULL
    cout<<"getAttributeHandle Testing\n inside and outside domain constant testing"<<endl;
    unsigned long ochandleidx;
    for(ochandleidx=0;ochandleidx<=60;ochandleidx++){
        char* attrname="HLAobjectRoot";
        //attrname="HLAprivilegeToDeleteObject";//Correct
        attrname="HLAprivilegeToDeleteObject ";//return handle is random like this 33523234 Whenever OChandle is legal because never have the attribute name
        //attrname="YearsOfService";	//return handle is a random like this 312443155 Whenever OChandle is legal and the OChandle have no such attrname
        //attrname=""; 		// return handle is a random like this 3134123413515 Whenever OChandle is legal because never have the such attribute name
        //attrname="\n"; 	// return handle is a random like this 3342352345234 Whenever OChandle is legal because never have the such attribute name
        //attrname=NULL;	//Segment Fault whenever OChandle is legal
        cout<<"getAttributeHandle(OChandle="<<ochandleidx<<",attributeName="<<attrname<<")="<<parser->getAttributeHandle(ochandleidx,attrname)<<endl;
        //cout<<"getAttributeHandle(OChandle="<<ochandleidx<<",attributeName=NULL)="<<parser->getAttributeHandle(ochandleidx,attrname)<<endl;
    }




    //---------------------------------END of Milo's getAttributeHandle Testing---------------------------------------*/



    for(iter=parser->begin(OHT_iter); iter != parser->end(OHT_iter); iter++)
    {

        //    if(*iter != 0)
        //    {

        //*************************************************************************
        //* OHT method 4 getObjectClassName()測試...Ok ... and "\n" will mean fail
        //*************************************************************************
        //std::cout << "----------OHT method No 4 getObjectClassName()測試----------" << std::endl;
        String objectClassName = parser->getObjectClassName((*iter)->__handle);

        //   std::cout << "Class ["<< setw(2)<< (*iter)->__handle << "]->name = " << objectClaassName <<endl;
        //   std::cout<< "Class ["<<setw(2)<<(*iter)->__handle<<"]->name = " << parser->getObjectClassName((*iter)->__handle)<< std::endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //std::cout<<"the ObjectClassName of this illegal class with ID= "<<0<<" is "<<parser->getObjectClassName(0)<<std::endl;
        //std::cout<<"the ObjectClassName of this illegal class with ID= "<<60<<" is "<<parser->getObjectClassName(60)<<std::endl;
        //*************************************************************************
        //* OHT method 5 getObjectClassHandle()測試.Not Ok  ...getObjectClass(NULL) will cause Segment Fault
        //                                                       if the Class is not exist ,then it's will return 0
        //*************************************************************************
        //std::cout << "----------OHT method No 5 getObjectClassHandle()測試-----" << std::endl;
        //std::cout << "getObjectClassHandle(" << objectClassName.c_str() << ") = " << parser->getObjectClassHandle(objectClassName.c_str()) << std::endl;
        //char* tmpStr = parser->getObjectClassName((*iter)->__handle);

        //std::cout<<"getObjectClassHandle( " << objectClassName.c_str() <<") = " << parser->getObjectClassHandle( parser->getObjectClassName((*iter)->__handle) ) <<std::endl;

        //std::cout<<"getObjectClassHandle( " << "constStr" <<") = " << parser->getObjectClassHandle("Cook" ) <<std::endl;
        //std::cout<<parser->getObjectClassHandle( tmpStr)<<std::endl;

        //std::cout<<parser->getObjectClassHandle( NULL)<<std::endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //*************************************************************************
        //* OHT method 11 isObjectClass(OCname)測討ot ok because Segment Fault will happen when OCname=NULL
        //************************************************************************
        //std::cout << (*iter)->__handle <<" th"<<std::endl;
        //std::cout << "----------OHT method No 11 isObjectClass()測試----------" << std::endl;
        //std::cout << "isObjectClass(" << parser->getObjectClassName((*iter)->__handle-1) << ") = " << (parser->isObjectClass(parser->getObjectClassName((*iter)->__handle-1))?"True":"False") << std::endl;

        //std::cout << "isObjectClass(" << parser->getObjectClassName((*iter)->__handle) << ") = " << (parser->isObjectClass(parser->getObjectClassName((*iter)->__handle))?"True":"False") << std::endl;
        //std::cout << "isObjectClass(" <<"Funny" << ") = " << (parser->isObjectClass("Funny")?"True":"False") << std::endl;
        //std::cout << "isObjectClass(" <<"\"\"" << ") = " << (parser->isObjectClass("")?"True":"False") << std::endl;
        //std::cout << "isObjectClass(" <<"NULL" << ") = " << (parser->isObjectClass(NULL)?"True":"False") << std::endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //*************************************************************************
        //* OHT method 9 getInheritedNumber()測?.Not Important here
        //*************************************************************************
        //std::cout << "----------OHT method No 9 getInheritedNumber()測試-----------" << std::endl;
        unsigned long OC_No;
        OC_No = parser->getInheritedNumber((*iter)->__handle);
        std::cout << "parser->getInheritedNumber(" << (*iter)->__handle << ") = "<< OC_No << endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;

        //*************************************************************************
        //* OHT method No 8 getObjectClass(OChandle)測試. .... o... return NULL whenever OChandlis illegal or zero
        //*************************************************************************
        //std::cout << "----------OHT method No 8 getObjectClass()測試-----------" << std::endl;
        //cout << "parser->getObjectClass(" << (*iter)->__handle << ") = "<< parser->getObjectClass((*iter)->__handle) << endl;

        //cout << "parser->getObjectClass(" << 0 << ") = "<< (int)parser->getObjectClass(0) << endl;
        //cout << "parser->getObjectClass(" <<60 << ") = "<< (int)parser->getObjectClass(60) << endl;
        //unsigned long M8_handle;
        //M8_handle=(*iter)->__handle+10;
        //if(parser->getObjectClass(M8_handle)){
        //      std::cout << "parser->getObjectClass(" <<M8_handle << ") = "<< parser->getObjectClass(M8_handle)->_msg << std::endl;
        //}
        //else{
        //	std::cout<<"parser->getObjectClass("<<M8_handle<<") return NULL "<<std::endl;
        //}


        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //*************************************************************************
        //* OHT method No 10 isObjectClass(OChandle)測試...Ok
        //*************************************************************************
        //std::cout << "----------OHT method No 10 isObjectClass()測試-----------" << std::endl;
        //cout << "parser->getObjectClass(" << (*iter)->__handle << ") = "<< (parser->isObjectClass((*iter)->__handle)?"True":"False") << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;
        //cout<<"\n\nisObjectClass(0)="<<parser->isObjectClass((unsigned long)0)<<endl;
        //cout<<"isObjectClass(60)="<<parser->isObjectClass((unsigned long)60)<<"\n\n\n"<<endl;

        //*************************************************************************
        //* OHT method No 13 getNumberOfAttribute()測試..Ok.
        //*************************************************************************
        //std::cout << "----------OHT method No 13 getNumberOfAttribute()測試-----------" << std::endl;

        unsigned long NoOfAttribute = parser->getNumberOfAttribute((*iter)->__handle);
        //cout<<"the number of attrbute for object class "<<parser->getObjectClassName((*iter)->__handle)<<"with ID="<<(*iter)->__handle<<" is "<<parser->getNumberOfAttribute((*iter)->__handle)<<std::endl;
        //cout << "parser->getNumberOfAttribute(" << (*iter)->__handle << ") = "<<  NoOfAttribute << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //std::cout<<"the number of attribute of illegal class with ID= "<<0<<" is "<<parser->getNumberOfAttribute(0)<<std::endl;
        //std::cout<<"the number of attribute of illegal class with ID= "<<60<<" is "<<parser->getNumberOfAttribute(60)<<std::endl;

        //*************************************************************************
        //* OHT method No 15 getNumberOfInheritedClass()測試.Not Ok when OChandle is illegal
        //*************************************************************************
        /*std::cout << "----------OHT method No 15 getNumberOfInheritedClass()測試-----------" << std::endl;
        std::cout << "the Number of Inherited Class " <<parser->getObjectClassName( (*iter)->__handle) << " with ID="<<(*iter)->__handle<<" is "<<  parser->getNumberOfInheritedClass((*iter)->__handle) <<std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout<<"the number of inherited class of illegal class with ID= "<<0<<" is "<<parser->getNumberOfInheritedClass(0)<<std::endl;
        std::cout<<"the number of inherited class of illegal class with ID= "<<60<<" is "<<parser->getNumberOfInheritedClass(60)<<std::endl;
        */
        for( unsigned long i = 1; i <= NoOfAttribute; i++)

        {
            //No 12 is the only Method just be check by me. Other are never be checked.
            //*************************************************************************
            //* OHT method No 12 isInheritedClassAttribute()測試...Nookt ,because  Segment Fault will happened when the arg of attrHandle =0
            //                                                     well-define on correct AttributeHandle
            //                                                     My wish is that If the the attribute handle is illegal, then return False
            //*************************************************************************
            //Milo Comment: This function is just well-defined for [1 to NumberOfAttribute(OChandle)],
            //              If you give the isInheritedClassAttribute(OChandle, 0),then System will 'Segment Fault'
            /*    std::cout << "----------OHT method No 12 isInheritedClassAttribute()測試-----------" << std::endl;
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle << ", " << i << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle, i)?"True":"False") << endl;
                //Correct with legal OChandle and legal attributeHandle
                
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle-1 << ", " << 0 << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle-1, 0)?"True":"False") << endl;
                //isInheritedClassAttribute(0, 0) == TRUE
                
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle << ", " << 0 << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle, 0)?"True":"False") << endl;
                //isInhertedClassAttribute(legal OChandle, 0) == TRUE
                
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle+10 << ", " << 0 << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle+10, 0)?"True":"False") << endl;
                //isInheritedClassAttribute(illegal OChandle , 0) == Segmentation fault


                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle-1 << ", " << i << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle-1, i)?"True":"False") << endl;
                //isInheritedClassAttribute(0, legal attributeHandle) == TRUE
                
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle << ", " << i << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle, i)?"True":"False") << endl;
                //isInheritedClassAttribute(legal OChandle , legal attributeHandle) == Correct
                
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle+10 << ", " << i << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle+10, i)?"True":"False") << endl;
                //iInheritedClassAttribute(illegal OChandle, legal attributeHandle) == Segmentation fault

               
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle-1 << ", " << i+10 << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle-1, i+10)?"True":"False") << endl;
                //isInheritedClassAttribute(0, illegal attributeHandle) == TRUE
                
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle << ", " << i+10 << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle, i+10)?"True":"False") << endl;
                //isInheritedClassAttribute(legal OChandle, illegal attributeHandle) == TRUE
                
                cout << "parser->isInheritedClassAttribute(" << (*iter)->__handle+10 << ", " << i+10 << ") = "<< (parser->isInheritedClassAttribute((*iter)->__handle+10, i+10)?"True":"False") << endl;
                //isInheritedClassAttribute(illegal OChandle, illegal attributeHandle) == Segmentation fault


            //std::cout << "----------------------------------------------" << std::endl; //*/
            //std::cout << std::endl;
            //*************************************************************************
            //* OHT method No 6 getAttributeName()測試...Not ok, because Segment Fault will happened when the arg of attrHandle
            //                                                   To pay attention that the func return "\n" whenever the attribute is illegal
            //                                                   This testing is coding above the loop
            //*************************************************************************
            //std::cout << "----------OHT method No 6 getAttributeName()測試-----------" << std::endl;
            String attrName = parser->getAttributeName((*iter)->__handle, i);
            std::cout << "parser->getAttributeName(" << (*iter)->__handle << ", " << i << ") = " << attrName.c_str() << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            cout << "parser->getAttributeName(" << 0 << ", " << 0 << ") = "<< (parser->getAttributeName(0, 0)) << endl;//getAttributeName(0, 0) == "\n"
            cout << "parser->getAttributeName(" << (*iter)->__handle << ", " << 0 << ") = "<< (parser->getAttributeName((*iter)->__handle, 0)) << endl;//getAttributeName(legal OChandle, 0) == Segmentation fault
            cout << "parser->getAttributeName(" << (*iter)->__handle+10 << ", " << 0 << ") = "<< (parser->getAttributeName((*iter)->__handle+10, 0)) << endl;//getAttributeName(illegal OChandle , 0) == Segmentation fault


            cout << "parser->getAttributeName(" << 0 << ", " << i << ") = "<< (parser->getAttributeName(0, i)) << endl;//getAttributeName(0, legal attributeHandle) == "\n"
            cout << "parser->getAttributeName(" << (*iter)->__handle << ", " << i << ") = "<< (parser->getAttributeName((*iter)->__handle, i)) << endl;//getAttributeName(legal OChandle , legal attributeHandle) == Correct
            cout << "parser->getAttributeName(" << (*iter)->__handle+10 << ", " << i << ") = "<< (parser->getAttributeName((*iter)->__handle+10, i)) << endl;//getAttributeName(illegal OChandle, legal attributeHandle) == "\n"


            cout << "parser->getAttributeName(" << 0 << ", " << i+10 << ") = "<< (parser->getAttributeName(0, i+10)) << endl;//getAttributeName(0, illegal attributeHandle) == "\n"
            cout << "parser->getAttributeName(" << (*iter)->__handle << ", " << i+10 << ") = "<< (parser->getAttributeName((*iter)->__handle, i+10)) << endl;//getAttributeName(legal OChandle, illegal attributeHandle) == "\n"
            cout << "parser->getAttributeName(" << (*iter)->__handle+10 << ", " << i+10 << ") = "<< (parser->getAttributeName((*iter)->__handle+10, i+10)) << endl;//getAttributeName(illegal OChandle, illegal attributeHandle) == "\n"
            //*/

            //*************************************************************************
            //* OHT method No 7 getAttributeHandle()測試...Not ok, (1) beacause the return is int but not unsigned long
            //                                                     (2)
            //                                                     This testing is coding above the loop
            //*************************************************************************
            //std::cout << "----------OHT method No 7 getAttributeHandle()測試-----------" << std::endl;
            //cout << "parser->getAttributeHandle(" << (*iter)->__handle << ", " << attrName << ") = "<< parser->getAttributeHandle((*iter)->__handle, attrName.c_str()) << endl;
            //std::cout << "----------------------------------------------" << std::endl;
            //std::cout << std::endl;
            //

            //cout << "parser->getAttributeHandle(" << 0 << ", " << "NULL" << ") = "<< (parser->getAttributeHandle(0, NULL)) << endl;//getAttributeHandle(0, NULL) == 0
            //cout << "parser->getAttributeHandle(" << (*iter)->__handle << ", " << "NULL" << ") = "<< (parser->getAttributeHandle((*iter)->__handle, NULL)) << endl;//getAttributeHandle(legal OChandle, NULL) == Segmentation fault
            //cout << "parser->getAttributeHandle(" << (*iter)->__handle+10 << ", " << "NULL" << ") = "<< (parser->getAttributeHandle((*iter)->__handle+10, NULL)) << endl;//getAttributeHandle(illegal OChandle , NULL) == Segmentation fault


            //cout << "parser->getAttributeHandle(" << 0 << ", " << "\"\"" << ") = "<< (parser->getAttributeHandle(0, "")) << endl;//getAttributeHandle(0, "") == 0
            //cout << "parser->getAttributeHandle(" << (*iter)->__handle << ", " << "\"\"" << ") = "<< (parser->getAttributeHandle((*iter)->__handle, "")) << endl;//getAttributeHandle(legal OChandle, "") == Large Random
            //cout << "parser->getAttributeHandle(" << (*iter)->__handle+10 << ", " << "\"\"" << ") = "<< (parser->getAttributeHandle((*iter)->__handle+10, "")) << endl;//getAttributeHandle(illegal OChandle , "") == 0
            //--------------------------------------------testing is here
            /*
                   cout << "parser->getAttributeHandle(" << 0 << ", " << i << ") = "<< (parser->getAttributeHandle(0, i)) << endl;//getAttributeHandle(0, legal attributeHandle) == "\n"
                   cout << "parser->getAttributeHandle(" << (*iter)->__handle << ", " << i << ") = "<< (parser->getAttributeHandle((*iter)->__handle, i)) << endl;//getAttributeHandle(legal OChandle , legal attributeHandle) == Correct
                   cout << "parser->getAttributeHandle(" << (*iter)->__handle+10 << ", " << i << ") = "<< (parser->getAttributeHandle((*iter)->__handle+10, i)) << endl;//getAttributeHandle(illegal OChandle, legal attributeHandle) == "\n"

                  
                   cout << "parser->getAttributeHandle(" << 0 << ", " << i+10 << ") = "<< (parser->getAttributeHandle(0, i+10)) << endl;//getAttributeHandle(0, illegal attributeHandle) == "\n"
                   cout << "parser->getAttributeHandle(" << (*iter)->__handle << ", " << i+10 << ") = "<< (parser->getAttributeHandle((*iter)->__handle, i+10)) << endl;//getAttributeHandle(legal OChandle, illegal attributeHandle) == "\n"
                   cout << "parser->getAttributeHandle(" << (*iter)->__handle+10 << ", " << i+10 << ") = "<< (parser->getAttributeHandle((*iter)->__handle+10, i+10)) << endl;//getAttributeHandle(illegal OChandle, illegal attributeHandle) == "\n"
            */
        }



        //*************************************************************************
        //* OHT method No 14 getNumberOfInheritedAttribute()測試.ok..
        //*************************************************************************
        //std::cout << "----------OHT method No 14 getNumberOfInheritedAttribute()測試-----------" << std::endl;
        //cout << "parser->getNumberOfInheritedAttribute(" << (*iter)->__handle << ") = "<< parser->getNumberOfInheritedAttribute((*iter)->__handle) << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;
        //	std::cout<<"the number of inherited attribute of class "<< parser->getObjectClassName((*iter)->__handle)<<" with ID="<<(*iter)->__handle<<" is "<<parser->getNumberOfInheritedAttribute((*iter)->__handle)<<std::endl;
        //std::cout<<"the number of inherited attribute of illegal class with ID= "<<0<<" is "<<parser->getNumberOfInheritedAttribute(0)<<std::endl;
        //std::cout<<"the number of inherited attribute of illegal class with ID= "<<60<<" is "<<parser->getNumberOfInheritedAttribute(60)<<std::endl;
        //*************************************************************************
        //* OHT method No 15 getNumberOfInheritedClass()測試...ok
        //*************************************************************************
        //std::cout << "----------OHT method No 15 getNumberOfInheritedClass()測試-----------" << std::endl;
        //cout << "parser->getNumberOfInheritedClass(" << (*iter)->__handle << ") = "<<  parser->getNumberOfInheritedClass((*iter)->__handle) << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //*************************************************************************
        //* OHT method No 16 getObjectClassIteratorClass(OChandle)測試...o expect to return iterator(NULL,NULL) whenever OChandlis illegal
        //*************************************************************************
        //
        //This code is Wating for Milo to test
        //
        //std::cout << "----------OHT method No 16 getNumberOfInheritedClass()測試-----------" << std::endl;
        //printf( "parser->getObjectClassIterator(%d) = %X\n", (*iter)->__handle, *parser->getObjectClassIterator((*iter)->__handle));
        //std::cout<<"parser->getObjectClassIterator("<<(*iter)->__handle<<") = "<<(int)(   ( parser->getObjectClassIterator((*iter)->__handle) )._current  ) <<std::endl;
        //std::cout<<"parser->getObjectClassIterator("<<(*iter)->__handle<<") = "<<  parser->getObjectClassIterator((*iter)->__handle)._current->__data->_msg<<std::endl;
        //std::cout<<"parser->getObjectClassIterator("<<(*iter)->__handle<<") = "<< (*(parser->getObjectClassIterator((*iter)->__handle)))->__handle<<std::endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;
        //cout<<"parser->getObjectClassIterator(0)is iterator("<<(int)parser->getObjectClassIterator(0)._current<<","<<(int)parser->getObjectClassIterator(0)._current<<")\n";
        //cout<<"parser->getObjectClassIterator(60)is iterator("<<(int)parser->getObjectClassIterator(60)._current<<","<<(int)parser->getObjectClassIterator(60)._current<<")\n";

        //*************************************************************************
        //* OHT method No 3 isAncestorOf()測試...ok
        //*************************************************************************
        //unsigned long OC_No1;
        //OC_No1 = parser->getInheritedNumber((*iter)->__handle);
        //srand(time(NULL));
        //for(int j = 1; j <= 10; j++ )
        //{
        // int rand1 = (rand()%OC_No1) + 1;
        // int rand2 = (rand()%OC_No1) + 1;
        //std::cout << "----------OHT method No 3 isAncestorOf()測試-----------" << std::endl;
        //printf( "parser->isAncestorOf(%d, %d) = %d\n", rand1, rand2, parser->isAncestorOf(rand1, rand2));
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;
        //}

        //unsigned long m3idx;
        //std::cout<<"\n\n\n"<<std::endl;
        //for(m3idx=0;m3idx<=50;m3idx++){
        //	cout<<"isAncestorOf("<<(*iter)->__handle<<","<<m3idx<<") = "<< parser->isAncestorOf((*iter)->__handle,m3idx)<<std::endl;
        //	//cout<<"isAncestorOf("<<(*iter)->__handle<<","<<m3idx<<") = "<< parser->isAncestorOf(  m3idx,(*iter)->__handle)<<std::endl;
        //}
        //cout<<"parser->isAncestorOf(0,60)="<<parser->isAncestorOf(0,60)<<endl;
        //cout<<"parser->isAncestorOf(60,0)="<<parser->isAncestorOf(60,0)<<endl;
        //cout<<"parser->isAncestorOf(0,0)="<<parser->isAncestorOf(0,0)<<endl;
        //cout<<"parser->isAncestorOf(60,60)="<<parser->isAncestorOf(60,60)<<endl;
        //cout<<"parser->isAncestorOf(60,30)="<<parser->isAncestorOf(60,30)<<endl;
        //cout<<"parser->isAncestorOf(30,60)="<<parser->isAncestorOf(30,60)<<endl;
        //cout<<"parser->isAncestorOf(0,30)="<<parser->isAncestorOf(0,30)<<endl;
        //cout<<"parser->isAncestorOf(30,0)="<<parser->isAncestorOf(30,0)<<endl;

        //std::cout << std::endl << std::endl << std::endl << std::endl;
        //}//end-if(*iter!=0)
    }

}


/*--------------------- miloDefine Class ------------------------------

class miloClass{
public:
	miloClass(const char* theString){
		int i=0;
		int j=0;
		while(theString[i++]!='\0');
		_msg=new char[i];
		for(j=0;j<i;j++)_msg[j]=theString[j];
	}

	~miloClass(){
		delete _msg;
	}

	char* _msg;
};

//-------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    /*
    int no;
    String str1("OHT TEST 1");
    String str2("OHT TEST 2");
    //*************************************************************************
    //* StrictCounter 測試…
    //*************************************************************************
    /*
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
    */
    //*************************************************************************
    //* UniqueStringCollection 測試…
    //*************************************************************************
    /*----------------------The test code which is written by Milochen --------------------
     
    std::cout<<"To Test the class:miloClass "<<std::endl;
    miloClass* miloClassTest=new miloClass("This is Test");
    std::cout<<miloClassTest->_msg<<std::endl;
    std::cout<<"The Test of class miloClass is Final"<<std::endl;
     UniqueStringCollection<miloClass> miloSet(100);
     miloClass *S1=new miloClass("This is S1 ");
     miloClass *S2=new miloClass("This is S2 ");
     miloClass *S3=new miloClass("This is S3 ");

     miloSet.insert(S1->_msg,S1);
     miloSet.insert(S2->_msg,S2);
     miloSet.insert(S3->_msg,S3);
     std::cout<<"Query the result->_msg with the key = \""<<S1->_msg<<"\" ,and we get \""<<miloSet.query(S1->_msg)->_msg<<"\""<<std::endl;
     std::cout<<"Query the result->_msg with the key = \""<<S2->_msg<<"\" ,and we get \""<<miloSet.query(S2->_msg)->_msg<<"\""<<std::endl;
     std::cout<<"Query the result->_msg with the key = \""<<S3->_msg<<"\" ,and we get \""<<miloSet.query(S3->_msg)->_msg<<"\""<<std::endl;

     //miloSet.insert("S1",new miloClass("S1")
     

    //    ULongSet.insert("S2",U2[2]);
    //    ULongSet.insert("S3",U2[3]);
     

     
     //------------------------------------------------------------------------------------*/

    /*
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
        
        std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;
        
        std::cout << "UniqueStringCollection.isExist(" << str2.c_str() << ") = "<< (usc.isExist(str1.c_str())?"True":"False") << std::endl;

        std::cout << "UniqueStringCollection.query(str2.c_str()) = " << usc.query(str2.c_str()) << std::endl;

        
        std::cout << "測試UniqueStringCollection.cancel(" << str1.c_str() << ")..." << std::endl;
        usc.cancel(str1.c_str());
        std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< (usc.isExist(str1.c_str())?"True":"False") << std::endl;
        
        std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;

        
        std::cout << "測試UniqueStringCollection.cancel(" << str1.c_str() << ")..." << std::endl;
        usc.cancel(str2.c_str());
        
        std::cout << "UniqueStringCollection.isExist(" << str1.c_str() << ") = "<< (usc.isExist(str1.c_str())?"True":"False") << std::endl;
        
        std::cout << "UniqueStringCollection.size() = "<< usc.size() << std::endl;
        
        std::cout << "測試UniqueStringCollection.query(\"aaa\")..." << std::endl;
        std::cout << "usc.query(\"aaa\") = " << usc.query("aaa") << std::endl;
    */


    //*************************************************************************
    //* OHT 測試…
    //*************************************************************************
    OHT<int>* oht1 = new OHT<int>();
    OHT<int>* oht2 = new OHT<int>();
    OHT<int>* oht3 = new OHT<int>();

    //ohttest(oht1, "JTFpFOM.xml");
    //ohttest(oht2, "JTFpFOM_MOM.xml");
    ohttest(oht3, "RestaurantSOM.xml");
    std::cout << "TESTING END...." << std::endl;



    return EXIT_SUCCESS;
}
