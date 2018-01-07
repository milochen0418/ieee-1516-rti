

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <time.h>

//#pragma implementation "UniqueStringCollection.h"
//#pragma implementation "IHT.h"

#include "OMT_Parser.h"
//#include "IHT.h" 
//#include "IHT.cpp" 
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



bool IHTtest(IHT<int>* parser, char* filename)
{


    //*************************************************************************
    //* IHT parseing()測試，檔案 RestaurantSOM.xml
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
    //* IHT iterator()測試...
    //*************************************************************************

    std::cout << "測試IHT<int>::iterator..." << std::endl;
    IHT<int>::iterator IHT_iter(parser->root, parser->root);
    IHT<int>::iterator iter;
    IHT<int>::iterator iter2;
    /*//---------------------------START of Milo's Iterator Testing------------------------------------------------
    for(iter=parser->begin(IHT_iter);iter!=parser->end(IHT_iter);iter++){
        std::cout<<"\n\n\n\n\nDiscuss the Handle:"<<(*iter)->__handle<<std::endl<<std::endl;
        IHT<int>::iterator tempiter;
        tempiter=iter;
        tempiter.root_here();
        for(iter2=parser->begin(tempiter);iter2!=parser->end(tempiter);iter2++){
            std::cout<<"\thandle="<<(*iter2)->__handle<<std::endl;
        }
    }
    //----------------------------END of Milo's Iterator Testing------------------------------------------------*/




    /*---------------------------START of Milo's getInteractionClassName Testing------------------------------------------------
     
    unsigned long m4idx;
    for(m4idx=0;m4idx<=200;m4idx++){
    std::cout<<"getInteractionClassName: "<<m4idx<<" |-> "<<parser->getInteractionClassName(m4idx)<<std::endl;
    }
      	
    //----------------------------END of Milo's getInteractionClassName Testing------------------------------------------------*/

    /*---------------------------START of Milo's setting something for getInteractionClass of IHT<miloClass>------------------------------------------------

    for(iter=parser->begin(IHT_iter);iter!=parser->end(IHT_iter);iter++){
    //(*iter)->__data = new miloClass(parser->getInteractionClassName((*iter)->__handle));
    //(*iter)->__data = new miloClass("abc");
    }
    //----------------------------END of Milo's setting something for getInteractionClass of IHT<miloClass>------------------------------------------------*/


    /*----------------------------START of Milo's getParameterName Testing------------------------------------------------



    //if Attribut is not here then the ParameterName will return "\n" even if when OChandle is illegal, beside,
    //Segment Fault will happen whenever OChandle is legal and ParameterHandle = 0
    cout<<"getParameterName Testing \n almost testing"<<endl;
    for(iter=parser->begin(IHT_iter);iter!=parser->end(IHT_iter);iter++){

        IHT<int>::iterator tmpiterator=iter;
        tmpiterator.up();
        unsigned long attridx;
        for(attridx=1;attridx<=parser->getNumberOfParameter((*iter)->__handle)+10;attridx++){
            if(parser->isInheritedClassParameter((*iter)->__handle, attridx)){
                if(strcmp( parser->getParameterName((*iter)->__handle, attridx)  , parser->getParameterName((*tmpiterator)->__handle,attridx))){
                    std::cout<<"ParameterHandle="<<attridx <<"|-> Wrong because difference between "<< (*iter)->__handle  <<" and "<< (*tmpiterator)->__handle  <<std::endl;
                }else{
                    std::cout<<"OutOfAttrRange("<< attridx<<") |->"<<parser->getParameterName((*iter)->__handle, attridx)<<std::endl;
                }

            }else{

                std::cout<<"is Not Inherited("<<parser->getInteractionClassName((*iter)->__handle)<<","<<(*iter)->__handle<<"," <<attridx <<") |-> "<<parser->getParameterName((*iter)->__handle,attridx)<<endl;

            }
        }
    }

    cout<<"getParameterName Testing\n OChandle Boundry Testing"<<endl;
    cout<<"getParameterName(0,0) = "<<parser->getParameterName(0,0)<<endl;	//return "\n"
    cout<<"getParameterName(0,1) = "<<parser->getParameterName(0,1)<<endl;	//return "\n"
    //cout<<"getParameterName(1,0) = "<<parser->getParameterName(1,0)<<endl;//Segment Fault
    cout<<"getParameterName(1,20) = "<<parser->getParameterName(1,20)<<endl;//return "\n"
    cout<<"getParameterName(1,1) = "<<parser->getParameterName(1,1)<<endl;	//return HLAprivilegeToDeleteObject
    //cout<<"getParameterName(40,0) = "<<parser->getParameterName(40,0)<<endl;//Segment Fault
    cout<<"getParameterName(41,0) = "<<parser->getParameterName(41,0)<<endl;//return "\n"
    cout<<"getParameterName(41,1) = "<<parser->getParameterName(41,1)<<endl;//return "\n"




    //---------------------------------END of Milo's getParameterName Testing----------------------------------------*/

    /*--------------------------------START of Milo's getParameterHandle Testing----------------------------------------
    //inside domain variable testing
    cout<<"\n\n\n\n\n\ninside domain variable testing"<<endl;
    cout<<"getParameterHandle Testing\n inside domain variable testing"<<endl;
    for(iter=parser->begin(IHT_iter);iter!=parser->end(IHT_iter);iter++){
    	unsigned long attridx;
    	for(attridx=1;attridx<=parser->getNumberOfParameter((*iter)->__handle);attridx++){
    		//when attridx is out of range; if attridx=0 then Segment Fault, else return false
    		cout<<"{OC="<<(*iter)->__handle<<",attr="<<attridx<<"} |-> ";
    		cout<< (( (attridx)==parser->getParameterHandle((*iter)->__handle, parser->getParameterName((*iter)->__handle, attridx) ))?"Correct":"Error");
    		cout<<"   ....attridx="<<attridx<<",  getParameterHandle(...)="<< parser->getParameterHandle((*iter)->__handle, parser->getParameterName((*iter)->__handle, attridx) )  <<endl;
    		
    		cout<<"{OC="<<0<<",attr="<<attridx<<"} |-> ";
    		cout<< parser->getParameterHandle(0, parser->getParameterName((*iter)->__handle, attridx) )<<endl;
    		
    		cout<<"{OC="<<60<<",attr="<<attridx<<"} |-> ";
    		cout<< parser->getParameterHandle(60, parser->getParameterName((*iter)->__handle, attridx) )<<endl;
    	}
    }
    cout<<"\n\n\n\n\n\ninside and outside domain constant testing";
    //inside and outside domain constant testing
    //The wrong is happen when OChandle is correct and the ParameterName is not the Parameter of this OChandle
    //The warnning is that the type of return value of getParameterHandle is 'int' but not 'unsinged long'
    //The Segment Fault will happen when OChandle is legal and  (char*)ParameterName is given by NULL
    cout<<"getParameterHandle Testing\n inside and outside domain constant testing"<<endl;
    unsigned long ochandleidx;
    for(ochandleidx=0;ochandleidx<=60;ochandleidx++){
    	char* attrname="HLAobjectRoot";
    	attrname="send_time";//Correct
        cout<<"getParameterHandle(OChandle="<<ochandleidx<<",ParameterName="<<attrname<<")="<<parser->getParameterHandle(ochandleidx,attrname)<<endl;
    	attrname="HLAprivilegeToDeleteObject ";//return handle is random like this 33523234 Whenever OChandle is legal because never have the Parameter name
        cout<<"getParameterHandle(OChandle="<<ochandleidx<<",ParameterName="<<attrname<<")="<<parser->getParameterHandle(ochandleidx,attrname)<<endl;
    	attrname="YearsOfService";	//return handle is a random like this 312443155 Whenever OChandle is legal and the OChandle have no such attrname
        cout<<"getParameterHandle(OChandle="<<ochandleidx<<",ParameterName="<<attrname<<")="<<parser->getParameterHandle(ochandleidx,attrname)<<endl;
    	attrname=""; 		// return handle is a random like this 3134123413515 Whenever OChandle is legal because never have the such Parameter name
        cout<<"getParameterHandle(OChandle="<<ochandleidx<<",ParameterName="<<attrname<<")="<<parser->getParameterHandle(ochandleidx,attrname)<<endl;
    	attrname="\n"; 	// return handle is a random like this 3342352345234 Whenever OChandle is legal because never have the such Parameter name
        cout<<"getParameterHandle(OChandle="<<ochandleidx<<",ParameterName="<<attrname<<")="<<parser->getParameterHandle(ochandleidx,attrname)<<endl;
    	attrname=NULL;	//Segment Fault whenever OChandle is legal
    	//cout<<"getParameterHandle(OChandle="<<ochandleidx<<",ParameterName="<<attrname<<")="<<parser->getParameterHandle(ochandleidx,attrname)<<endl;
        printf("getParameterHandle(OChandle= %d, ,ParameterName=%s)=%d\n", ochandleidx, attrname,parser->getParameterHandle(ochandleidx,attrname));
    	//cout<<"getParameterHandle(OChandle="<<ochandleidx<<",ParameterName=NULL)="<<parser->getParameterHandle(ochandleidx,attrname)<<endl;
    }




     //---------------------------------END of Milo's getParameterHandle Testing---------------------------------------*/



    for(iter=parser->begin(IHT_iter); iter != parser->end(IHT_iter); iter++)
    {

        //    if(*iter != 0)
        //    {

        //*************************************************************************
        //* IHT method 4 getInteractionClassName()測試...Ok ... and "\n" will mean fail
        //*************************************************************************
        //std::cout << "----------IHT method No 4 getInteractionClassName()測試----------" << std::endl;
        String InteractionClassName = parser->getInteractionClassName((*iter)->__handle);

        //std::cout << "Class ["<< setw(2)<< (*iter)->__handle << "]->name = " << objectClaassName <<endl;
        //std::cout<< "Class ["<<setw(2)<<(*iter)->__handle<<"]->name = " << parser->getInteractionClassName((*iter)->__handle)<< std::endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //std::cout<<"the InteractionClassName of this illegal class with ID= "<<0<<" is "<<parser->getInteractionClassName(0)<<std::endl;
        //std::cout<<"the InteractionClassName of this illegal class with ID= "<<60<<" is "<<parser->getInteractionClassName(60)<<std::endl;
        //*************************************************************************
        //* IHT method 5 getInteractionClassHandle()測試.Not Ok  ...getInteractionClass(NULL) will cause Segment Fault
        //                                                       if the Class is not exist ,then it's will return 0
        //*************************************************************************
        /*std::cout << "----------IHT method No 5 getInteractionClassHandle()測試-----" << std::endl;
        std::cout << "getInteractionClassHandle(" << InteractionClassName.c_str() << ") = " << parser->getInteractionClassHandle(InteractionClassName.c_str()) << std::endl;
        char* tmpStr = parser->getInteractionClassName((*iter)->__handle);

        std::cout<<"getInteractionClassHandle( " << InteractionClassName.c_str() <<") = " << parser->getInteractionClassHandle( parser->getInteractionClassName((*iter)->__handle) ) <<std::endl;

        std::cout<<"getInteractionClassHandle( " << "constStr" <<") = " << parser->getInteractionClassHandle("Cook" ) <<std::endl;
        std::cout<<parser->getInteractionClassHandle( tmpStr)<<std::endl;

        std::cout<<parser->getInteractionClassHandle( NULL)<<std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;
        //*/

        //*************************************************************************
        //* IHT method 11 isInteractionClass(OCname)測討ot ok because Segment Fault will happen when OCname=NULL
        //************************************************************************
        /*std::cout << (*iter)->__handle <<" th"<<std::endl;
        std::cout << "----------IHT method No 11 isInteractionClass()測試----------" << std::endl;
        std::cout << "isInteractionClass(" << parser->getInteractionClassName((*iter)->__handle-1) << ") = " << (parser->isInteractionClass(parser->getInteractionClassName((*iter)->__handle-1))?"True":"False") << std::endl;

        std::cout << "isInteractionClass(" << parser->getInteractionClassName((*iter)->__handle) << ") = " << (parser->isInteractionClass(parser->getInteractionClassName((*iter)->__handle))?"True":"False") << std::endl;
        std::cout << "isInteractionClass(" <<"Funny" << ") = " << (parser->isInteractionClass("Funny")?"True":"False") << std::endl;
        std::cout << "isInteractionClass(" <<"\"\"" << ") = " << (parser->isInteractionClass("")?"True":"False") << std::endl;
        //Method 11 can't using NULL as a parameter. Note by Bruce
        //std::cout << "isInteractionClass(" <<"NULL" << ") = " << (parser->isInteractionClass(NULL)?"True":"False") << std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;
        //*/

        //*************************************************************************
        //* IHT method 9 getInheritedNumber()測?.Not Important here
        //*************************************************************************
        /*std::cout << "----------IHT method No 9 getInheritedNumber()測試-----------" << std::endl;
        unsigned long OC_No;
        OC_No = parser->getInheritedNumber((*iter)->__handle);
        cout << "parser->getInheritedNumber(" << (*iter)->__handle << ") = "<< OC_No << endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;

        //*************************************************************************
        //* IHT method No 8 getInteractionClass(OChandle)測試. .... o... return NULL whenever OChandlis illegal or zero
        //*************************************************************************
        std::cout << "----------IHT method No 8 getInteractionClass()測試-----------" << std::endl;
        cout << "parser->getInteractionClass(" << (*iter)->__handle << ") = "<< parser->getInteractionClass((*iter)->__handle) << endl;

        cout << "parser->getInteractionClass(" << 0 << ") = "<< (int)parser->getInteractionClass(0) << endl;
        cout << "parser->getInteractionClass(" <<60 << ") = "<< (int)parser->getInteractionClass(60) << endl;
        unsigned long M8_handle;
        M8_handle=((*iter)->__handle)+10;
        if(parser->getInteractionClass(M8_handle)){
              std::cout << "parser->getInteractionClass(" <<M8_handle << ") = "<< parser->getInteractionClass(M8_handle) << std::endl;
        }
        else{
        	std::cout<<"parser->getInteractionClass("<<M8_handle<<") return NULL "<<std::endl;
        }


        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;
        //*/

        //*************************************************************************
        //* IHT method No 10 isInteractionClass(OChandle)測試...Ok
        //*************************************************************************
        //std::cout << "----------IHT method No 10 isInteractionClass()測試-----------" << std::endl;
        //cout << "parser->getInteractionClass(" << (*iter)->__handle << ") = "<< (parser->isInteractionClass((*iter)->__handle)?"True":"False") << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;
        //cout<<"\n\nisInteractionClass(0)="<<parser->isInteractionClass((unsigned long)0)<<endl;
        //cout<<"isInteractionClass(60)="<<parser->isInteractionClass((unsigned long)60)<<"\n\n\n"<<endl;

        //*************************************************************************
        //* IHT method No 13 getNumberOfParameter()測試..Ok.
        //*************************************************************************
        //std::cout << "----------IHT method No 13 getNumberOfParameter()測試-----------" << std::endl;

        unsigned long NoOfParameter = parser->getNumberOfParameter((*iter)->__handle);
        //cout<<"the number of attrbute for object class "<<parser->getInteractionClassName((*iter)->__handle)<<"with ID="<<(*iter)->__handle<<" is "<<parser->getNumberOfParameter((*iter)->__handle)<<std::endl;
        //cout << "parser->getNumberOfParameter(" << (*iter)->__handle << ") = "<<  NoOfParameter << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //std::cout<<"the number of Parameter of illegal class with ID= "<<0<<" is "<<parser->getNumberOfParameter(0)<<std::endl;
        //std::cout<<"the number of Parameter of illegal class with ID= "<<60<<" is "<<parser->getNumberOfParameter(60)<<std::endl;

        //*************************************************************************
        //* IHT method No 15 getNumberOfInheritedClass()測試.Not Ok when OChandle is illegal
        //*************************************************************************
        std::cout << "----------IHT method No 15 getNumberOfInheritedClass()測試-----------" << std::endl;
        std::cout << "the Number of Inherited Class " <<parser->getInteractionClassName( (*iter)->__handle) << " with ID="<<(*iter)->__handle<<" is "<<  parser->getNumberOfInheritedClass((*iter)->__handle) <<std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;
        //std::cout<<"the number of inherited class of illegal class with ID= "<<0<<" is "<<parser->getNumberOfInheritedClass(0)<<std::endl;
        //std::cout<<"the number of inherited class of illegal class with ID= "<<60<<" is "<<parser->getNumberOfInheritedClass(60)<<std::endl;

        for( unsigned long i = 1; i <= NoOfParameter; i++)

        {
            //No 12 is the only Method just be check by me. Other are never be checked.
            //*************************************************************************
            //* IHT method No 12 isInheritedClassParameter()測試...Nookt ,because  Segment Fault will happened when the arg of attrHandle =0
            //                                                     well-define on correct ParameterHandle
            //                                                     My wish is that If the the Parameter handle is illegal, then return False
            //*************************************************************************
            //Milo Comment: This function is just well-defined for [1 to NumberOfParameter(OChandle)],
            //              If you give the isInheritedClassParameter(OChandle, 0),then System will 'Segment Fault'
            /*    std::cout << "----------IHT method No 12 isInheritedClassParameter()測試-----------" << std::endl;
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle << ", " << i << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle, i)?"True":"False") << endl;
                //Correct with legal OChandle and legal ParameterHandle
                
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle-1 << ", " << 0 << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle-1, 0)?"True":"False") << endl;
                //isInheritedClassParameter(0, 0) == TRUE
                
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle << ", " << 0 << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle, 0)?"True":"False") << endl;
                //isInhertedClassParameter(legal OChandle, 0) == TRUE
                
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle+10 << ", " << 0 << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle+10, 0)?"True":"False") << endl;
                //isInheritedClassParameter(illegal OChandle , 0) == Segmentation fault


                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle-1 << ", " << i << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle-1, i)?"True":"False") << endl;
                //isInheritedClassParameter(0, legal ParameterHandle) == TRUE
                
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle << ", " << i << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle, i)?"True":"False") << endl;
                //isInheritedClassParameter(legal OChandle , legal ParameterHandle) == Correct
                
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle+10 << ", " << i << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle+10, i)?"True":"False") << endl;
                //iInheritedClassParameter(illegal OChandle, legal ParameterHandle) == Segmentation fault

               
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle-1 << ", " << i+10 << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle-1, i+10)?"True":"False") << endl;
                //isInheritedClassParameter(0, illegal ParameterHandle) == TRUE
                
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle << ", " << i+10 << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle, i+10)?"True":"False") << endl;
                //isInheritedClassParameter(legal OChandle, illegal ParameterHandle) == TRUE
                
                cout << "parser->isInheritedClassParameter(" << (*iter)->__handle+10 << ", " << i+10 << ") = "<< (parser->isInheritedClassParameter((*iter)->__handle+10, i+10)?"True":"False") << endl;
                //isInheritedClassParameter(illegal OChandle, illegal ParameterHandle) == Segmentation fault


            //std::cout << "----------------------------------------------" << std::endl; //*/
            //std::cout << std::endl;
            //*/

            //*************************************************************************
            //* IHT method No 6 getParameterName()測試...Not ok, because Segment Fault will happened when the arg of attrHandle
            //                                                   To pay attention that the func return "\n" whenever the Parameter is illegal
            //                                                   This testing is coding above the loop
            //*************************************************************************
            /*std::cout << "----------IHT method No 6 getParameterName()測試-----------" << std::endl;
            String attrName = parser->getParameterName((*iter)->__handle, i);
            std::cout << "parser->getParameterName(" << (*iter)->__handle << ", " << i << ") = " << attrName.c_str() << endl;
            std::cout << "----------------------------------------------" << std::endl;
            std::cout << std::endl;

            cout << "parser->getParameterName(" << 0 << ", " << 0 << ") = "<< (parser->getParameterName(0, 0)) << endl;//getParameterName(0, 0) == "\n"
            cout << "parser->getParameterName(" << (*iter)->__handle << ", " << 0 << ") = "<< (parser->getParameterName((*iter)->__handle, 0)) << endl;//getParameterName(legal OChandle, 0) == Segmentation fault
            cout << "parser->getParameterName(" << (*iter)->__handle+10 << ", " << 0 << ") = "<< (parser->getParameterName((*iter)->__handle+10, 0)) << endl;//getParameterName(illegal OChandle , 0) == Segmentation fault


            cout << "parser->getParameterName(" << 0 << ", " << i << ") = "<< (parser->getParameterName(0, i)) << endl;//getParameterName(0, legal ParameterHandle) == "\n"
            cout << "parser->getParameterName(" << (*iter)->__handle << ", " << i << ") = "<< (parser->getParameterName((*iter)->__handle, i)) << endl;//getParameterName(legal OChandle , legal ParameterHandle) == Correct
            cout << "parser->getParameterName(" << (*iter)->__handle+10 << ", " << i << ") = "<< (parser->getParameterName((*iter)->__handle+10, i)) << endl;//getParameterName(illegal OChandle, legal ParameterHandle) == "\n"


            cout << "parser->getParameterName(" << 0 << ", " << i+10 << ") = "<< (parser->getParameterName(0, i+10)) << endl;//getParameterName(0, illegal ParameterHandle) == "\n"
            cout << "parser->getParameterName(" << (*iter)->__handle << ", " << i+10 << ") = "<< (parser->getParameterName((*iter)->__handle, i+10)) << endl;//getParameterName(legal OChandle, illegal ParameterHandle) == "\n"
            cout << "parser->getParameterName(" << (*iter)->__handle+10 << ", " << i+10 << ") = "<< (parser->getParameterName((*iter)->__handle+10, i+10)) << endl;//getParameterName(illegal OChandle, illegal ParameterHandle) == "\n"
            //*/

            //*************************************************************************
            //* IHT method No 7 getParameterHandle()測試...Not ok, (1) beacause the return is int but not unsigned long
            //                                                     (2)
            //                                                     This testing is coding above the loop
            //*************************************************************************
            //std::cout << "----------IHT method No 7 getParameterHandle()測試-----------" << std::endl;
            //cout << "parser->getParameterHandle(" << (*iter)->__handle << ", " << attrName << ") = "<< parser->getParameterHandle((*iter)->__handle, attrName.c_str()) << endl;
            //std::cout << "----------------------------------------------" << std::endl;
            //std::cout << std::endl;
            //

            //cout << "parser->getParameterHandle(" << 0 << ", " << "NULL" << ") = "<< (parser->getParameterHandle(0, NULL)) << endl;//getParameterHandle(0, NULL) == 0
            //cout << "parser->getParameterHandle(" << (*iter)->__handle << ", " << "NULL" << ") = "<< (parser->getParameterHandle((*iter)->__handle, NULL)) << endl;//getParameterHandle(legal OChandle, NULL) == Segmentation fault
            //cout << "parser->getParameterHandle(" << (*iter)->__handle+10 << ", " << "NULL" << ") = "<< (parser->getParameterHandle((*iter)->__handle+10, NULL)) << endl;//getParameterHandle(illegal OChandle , NULL) == Segmentation fault


            //cout << "parser->getParameterHandle(" << 0 << ", " << "\"\"" << ") = "<< (parser->getParameterHandle(0, "")) << endl;//getParameterHandle(0, "") == 0
            //cout << "parser->getParameterHandle(" << (*iter)->__handle << ", " << "\"\"" << ") = "<< (parser->getParameterHandle((*iter)->__handle, "")) << endl;//getParameterHandle(legal OChandle, "") == Large Random
            //cout << "parser->getParameterHandle(" << (*iter)->__handle+10 << ", " << "\"\"" << ") = "<< (parser->getParameterHandle((*iter)->__handle+10, "")) << endl;//getParameterHandle(illegal OChandle , "") == 0
            //--------------------------------------------testing is here
            /*
                   cout << "parser->getParameterHandle(" << 0 << ", " << i << ") = "<< (parser->getParameterHandle(0, i)) << endl;//getParameterHandle(0, legal ParameterHandle) == "\n"
                   cout << "parser->getParameterHandle(" << (*iter)->__handle << ", " << i << ") = "<< (parser->getParameterHandle((*iter)->__handle, i)) << endl;//getParameterHandle(legal OChandle , legal ParameterHandle) == Correct
                   cout << "parser->getParameterHandle(" << (*iter)->__handle+10 << ", " << i << ") = "<< (parser->getParameterHandle((*iter)->__handle+10, i)) << endl;//getParameterHandle(illegal OChandle, legal ParameterHandle) == "\n"

                  
                   cout << "parser->getParameterHandle(" << 0 << ", " << i+10 << ") = "<< (parser->getParameterHandle(0, i+10)) << endl;//getParameterHandle(0, illegal ParameterHandle) == "\n"
                   cout << "parser->getParameterHandle(" << (*iter)->__handle << ", " << i+10 << ") = "<< (parser->getParameterHandle((*iter)->__handle, i+10)) << endl;//getParameterHandle(legal OChandle, illegal ParameterHandle) == "\n"
                   cout << "parser->getParameterHandle(" << (*iter)->__handle+10 << ", " << i+10 << ") = "<< (parser->getParameterHandle((*iter)->__handle+10, i+10)) << endl;//getParameterHandle(illegal OChandle, illegal ParameterHandle) == "\n"
            */
        }



        //*************************************************************************
        //* IHT method No 14 getNumberOfInheritedParameter()測試.ok..
        //*************************************************************************
        //std::cout << "----------IHT method No 14 getNumberOfInheritedParameter()測試-----------" << std::endl;
        //cout << "parser->getNumberOfInheritedParameter(" << (*iter)->__handle << ") = "<< parser->getNumberOfInheritedParameter((*iter)->__handle) << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;
        //	std::cout<<"the number of inherited Parameter of class "<< parser->getInteractionClassName((*iter)->__handle)<<" with ID="<<(*iter)->__handle<<" is "<<parser->getNumberOfInheritedParameter((*iter)->__handle)<<std::endl;
        //std::cout<<"the number of inherited Parameter of illegal class with ID= "<<0<<" is "<<parser->getNumberOfInheritedParameter(0)<<std::endl;
        //std::cout<<"the number of inherited Parameter of illegal class with ID= "<<60<<" is "<<parser->getNumberOfInheritedParameter(60)<<std::endl;
        //*************************************************************************
        //* IHT method No 15 getNumberOfInheritedClass()測試...ok
        //*************************************************************************
        //std::cout << "----------IHT method No 15 getNumberOfInheritedClass()測試-----------" << std::endl;
        //cout << "parser->getNumberOfInheritedClass(" << (*iter)->__handle << ") = "<<  parser->getNumberOfInheritedClass((*iter)->__handle) << endl;
        //std::cout << "----------------------------------------------" << std::endl;
        //std::cout << std::endl;

        //*************************************************************************
        //* IHT method No 16 getInteractionClassIteratorClass(OChandle)測試...o expect to return iterator(NULL,NULL) whenever OChandlis illegal
        //*************************************************************************
        //
        //This code is Wating for Milo to test
        //
        std::cout << "----------IHT method No 16 getNumberOfInheritedClass()測試-----------" << std::endl;
        printf( "parser->getInteractionClassIterator(%d) = %X\n", (*iter)->__handle, *parser->getInteractionClassIterator((*iter)->__handle));
        std::cout<<"parser->getInteractionClassIterator("<<(*iter)->__handle<<") = "<<(int)(   ( parser->getInteractionClassIterator((*iter)->__handle) )._current  ) <<std::endl;
        std::cout<<"parser->getInteractionClassIterator(" << (*iter)->__handle << ") = " <<  parser->getInteractionClassIterator((*iter)->__handle)._current->__data <<std::endl;
        std::cout<<"parser->getInteractionClassIterator("<<(*iter)->__handle<<") = "<< (*(parser->getInteractionClassIterator((*iter)->__handle)))->__handle<<std::endl;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << std::endl;
        cout<<"parser->getInteractionClassIterator(0)is iterator("<<(int)parser->getInteractionClassIterator(0)._current<<","<<(int)parser->getInteractionClassIterator(0)._current<<")\n";
        cout<<"parser->getInteractionClassIterator(60)is iterator("<<(int)parser->getInteractionClassIterator(60)._current<<","<<(int)parser->getInteractionClassIterator(60)._current<<")\n";

        //*************************************************************************
        //* IHT method No 3 isAncestorOf()測試...ok
        //*************************************************************************
        //unsigned long OC_No1;
        //OC_No1 = parser->getInheritedNumber((*iter)->__handle);
        //srand(time(NULL));
        //for(int j = 1; j <= 10; j++ )
        //{
        // int rand1 = (rand()%OC_No1) + 1;
        // int rand2 = (rand()%OC_No1) + 1;
        //std::cout << "----------IHT method No 3 isAncestorOf()測試-----------" << std::endl;
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
    String str1("IHT TEST 1");
    String str2("IHT TEST 2");
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
    //* IHT 測試…
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
