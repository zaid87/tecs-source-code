/* 
 * File:   main.cpp
 * Author: Zaid a
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <utility>
#include <string>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <deque>

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN


using namespace std;

std::ofstream debug1("debug1.txt",std::ios::out);
std::ofstream debug2("partitioning.txt",std::ios::out);
std::ofstream debug3("resourceAssignment.txt",std::ios::out);
std::ofstream debug4("resultsBreakdown.txt",std::ios::out);
std::ofstream debug5("results.txt",std::ios::out);
std::ofstream debug6("utils.txt",std::ios::out);
std::ofstream debug7("RTA.txt",std::ios::out);
std::ofstream debug8("tasks.txt",std::ios::out);
std::ofstream debug9("log1.txt",std::ios::out);
std::ofstream debug10("errorlog.txt",std::ios::out);
std::ifstream in1("exp3-10.txt",std::ios::in);
std::ofstream test("testing.txt",std::ios::out);

bool breakused=false;
bool errorFlag=false;

//#include"genertaion.h"
#include"generation2.h"
//#include"generation-ex.h"
#include"utilities.h"
#include"msrpanalysis.h"
#include"ilpanalysis.h"

#include"assignresources.h"
//#include"heuristic.h"
//#include"exhaustive.h"
#include"gs.h"


const int numSystems=100;
int startingSeed=0;


int main()
{
    seed=time(NULL);

    //seed=1377294464;
//    seed=1377295111;    
   //seed=1379691292;
   
 //  seed=1379692792;

   //seed=1379691352;
    //seed=1379691296;
   //seed=1383073146;
//  seed=1387092334;
	seed=1397364920;
    //seed=1397586528;
//seed=1412663385;
//seed=1385894485;
seed=1385895233;

 
   

   startingSeed=seed;
           
   int numSched=0;
   int numOptimal=0;
   int costOptimal=0;
   int algoCostSys=0;
   int algoCostAll=0;
   bool algoSucceed=false;
   cout<<clock();
   
   clock_t timer;
   clock_t timer2;
   timer = time(NULL);
   int rval=0;

   
    
  
   for(int numPoints=0;numPoints<1;numPoints++)
   {
    
    
    numSched=0;
    algoCostSys=0;
    algoCostAll=0;
  
    for(int counter=0;counter<numSystems;counter++)
    {

        cout<<seed<<endl;

        tasks.clear();
        graph.clear();
        node_counter=0;
        assignedTaskCount=0;
         resources.clear();
        generateResources();
        generateTasks(counter);
        
        sets.clear();
       
        processors.clear();
        assignedTaskCount=0;
        
        generateSets();
        generateProcessors();
        

        cout<<"Just before"<<endl;
//        if(seed==1397364938)
//        {
        rval=ilpSchedAnalysis();
        if(rval!=0)
        {
            cout<<"ret="<<rval<<endl;
            debug9<<seed<<" new succeeded"<<endl;
            printPartitioning();
            
               if(assignedTaskCount == tasks.size())
               {
                    if(schedAnalysis2())    
                        debug9<<seed<<" old succeeded"<<endl;
                    else
                    {
                        debug9<<seed<<"old failed"<<endl;
                        debug7<<seed<<" NOTE: OLD FAILED SO ITS NUMBERS SHOULD BE IGNORED"<<endl;
                    }
                    checkILPsolution();
                      
                    for(int i=0;i<tasks.size();i++)
                    {
                        
                        test<<i<<"\t"<<tasks[i].b.totRespTime<<"\t"<<tasks[i].b.rmsrp<<"\t"<<tasks[i].b.lmsrp<<endl; 
                    }
                     test<<"=========================================="<<endl;

                    numSched++;
                    algoCostSys=solutionCost();
                    algoCostAll+=algoCostSys;
                    algoSucceed=true;
                    if(rval==1)
                    {
                        numOptimal++;
                        costOptimal+=algoCostSys;
                    }
                    debug4<<seed<<"       "<<algoCostSys<<"      ";
                    if(rval==1)
                        debug4<<"Optimal"<<endl;
                    else
                        debug4<<"Feasible"<<endl;


                      debug7<<seed<<endl<<"old new T"<<endl;
                      for(int i=0;i<tasks.size();i++)
                      {
                          debug7<<i<<"\t"<<tasks[i].b.totRespTime<<"\t"<<tasks[i].ilpR<<"\t"<<tasks[i].t<<endl;
                          if(tasks[i].b.totRespTime - tasks[i].ilpR > 0.001 || tasks[i].b.totRespTime - tasks[i].ilpR < -0.001 )
                          {
                              debug1<<seed<<"   "<<i<<"\t"<<tasks[i].b.totRespTime<<"\t"<<tasks[i].ilpR<<endl;
                              //errorFlag=true;
                          }
                      }
               }
               else
               {
                   algoSucceed=false;
                   debug9<<seed<<" new failed"<<endl;
               }
        }
        else
        {
            algoSucceed=false;
            debug9<<seed<<" new failed"<<endl;
        }
        //}


       deAllocateTasks();
       
      
     
         


   }
    
    
    

           debug5<<"At number of tasks= "<<totalTaskCnt;
    timer2=time(NULL);;
    debug5<<" number sched= "<<numSched<<" cost= "<<algoCostAll;
    debug5<<"  time= "<<double( timer2 - timer )<<endl;
    debug5<<"number Optimal sched= "<<numOptimal<<" optimal only cost= "<<costOptimal<<endl;
            
                totalTaskCnt+=2;
//			if(rsf==0.1)
//				rsf=0.25;
//			else if(rsf==0.25)
//				rsf=0.5;
//			else
//				rsf=0.75;
}
   
   
   debug5<<"starting seed="<<startingSeed<<endl;
   debug5<<"Number of systems per util= "<<numSystems<<endl;
   debug5<<"Number of processors="<<procCnt<<endl;
   debug5<<"Number of resources"<<numRes<<endl;
   debug5<<"Period range: "<<minPeriod<<"-"<<maxPeriod<<endl;
   //debug5<<"Number of tasks="<<totalTaskCnt<<endl;
   debug5<<"rsf= "<<rsf<<endl;
   debug5<<"Average task utilization=0.1"<<avgU<<endl;
   debug5<<"CS length range: "<<minCsLength<<"-"<<maxCsLength<<endl;

   debug5<<"Using GS"<<endl<<endl;             
    

            debug5<<endl<<"    "<<tasks[0].u<<endl;
   cout<<"---------------------------------------------"<<endl;
   cout<<"Number sched= "<<numSched<<"      Cost= "<<algoCostAll;
   //cout<<"TTime= "<<double( clock() - timer ) / (double)CLOCKS_PER_SEC<<"s"<<endl;
    cout<<"  time= "<<double( timer2 - timer )<<endl;
    return 0;
   
}
