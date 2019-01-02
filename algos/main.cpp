/* 
 * File:   main.cpp
 * Author: Zaid
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

using namespace std;

std::ofstream debug1("debug1.txt",std::ios::out);
std::ofstream debug2("partitioning.txt",std::ios::out);
std::ofstream debug3("resourceAssignment.txt",std::ios::out);
std::ofstream debug4("resultsBreakdown.txt",std::ios::out);
std::ofstream debug5("results.txt",std::ios::out);
std::ofstream debug6("utils.txt",std::ios::out);
//std::ofstream debug7("response.txt",std::ios::out);
std::ofstream debug8("tasks.txt",std::ios::out);
std::ofstream debug9("test.txt",std::ios::out);
std::ifstream in1("exp3d-2030.txt",std::ios::in);

bool breakused=false;

//#include"genertaion.h"
#include"generation2.h"
#include"msrpanalysis.h"
#include"utilities.h"
#include"assignresources.h"
#include"heuristic.h"
//#include"exhaustive.h"
//#include"gs.h"


const int numSystems=100;
int startingSeed=0;


int main()
{
    seed=time(NULL);

    //seed=1377294464;
//    seed=1377295111;    
   //seed=1379691292;
   
  // seed=1379692792;

   //seed=1379691352;
    //seed=1379691296;
//   seed=1383073146;
 //  seed=1387092334;
	//seed=1397364017;
    //seed=1397586528;
 
   

   startingSeed=seed;
           
   int numSched=0;
   int algoCostSys=0;
   int algoCostAll=0;
   bool algoSucceed=false;
   clock_t timer,timer2;

   
    
  
   for(int numPoints=0;numPoints<6;numPoints++)
   {
    
    timer=clock();
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
        

        
        
        runAlgo();
      
        
   //   runExhaustive();
//        greedyslacker();
        
   //     greedyslackerext();
        
        int tcheck=0;
        for(int i=0;i<processors.size();i++)
            for(int j=0;j<processors[i].tasks.size();j++)
                tcheck++;
        if(tcheck!=assignedTaskCount)
        {
            cout<<"assignedTaskCoint error"<<endl;
            exit(1);
        }
//        
       
        
        cout<<"Algo assigned "<<assignedTaskCount<<" of "<<node_counter<<endl;
         if(assignedTaskCount == node_counter && schedAnalysis2())
         {
                printCost();
                printPartitioning();
                //printRT();
                
                numSched++;
                algoCostSys=solutionCost();
                algoCostAll+=algoCostSys;
                algoSucceed=true;
                debug4<<seed<<"       "<<algoCostSys<<endl;
              
                
        }
        else
        {
            algoSucceed=false;
            debug4<<seed<<"       "<<"NOT"<<endl;
        }
                         
        
         deAllocateTasks();


   }
    
    
    

           debug5<<"At number of tasks= "<<totalTaskCnt;
    
    debug5<<" number sched= "<<numSched<<" cost= "<<algoCostAll;
            debug5<<"  time= "<<double( clock() - timer ) / (double)CLOCKS_PER_SEC<<"s"<<endl;
     
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

   debug5<<"Using MPA"<<endl<<endl;             
    

            debug5<<endl<<"    "<<tasks[0].u<<endl;
   cout<<"---------------------------------------------"<<endl;
   cout<<"Number sched= "<<numSched<<"      Cost= "<<algoCostAll;
   cout<<"       Time= "<<double( clock() - timer ) / (double)CLOCKS_PER_SEC<<"s"<<endl;
   
    return 0;
   
}
