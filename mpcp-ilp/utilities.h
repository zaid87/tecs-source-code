/* 
 * File:   utilities.h
 * Author: Zaid
 *
 * Created on October 2, 2013, 11:59 PM
 */

#ifndef UTILITIES_H

#include "generation2.h"


//#include "generation2.h"



//int maxPossCost;

void checkILPsolution()
{
    debug10<<seed<<endl;
    if(assignedTaskCount!=tasks.size())
    {
        debug10<<"error: Not all assigned"<<endl;
    }
    
    for(int i=0;i<tasks.size();i++)
        for(int j=i+1;j<tasks.size();j++)
            if(tasks[i].priority==tasks[j].priority)
                debug10<<"priority error"<<endl;
        
    for(int i=0;i<tasks.size();i++)
    {
        if(tasks[i].ilpR > tasks[i].b.totRespTime && tasks[i].ilpR-tasks[i].b.totRespTime>0.01)
            debug10<<"response time error "<<tasks[i].ilpR <<"-"<<tasks[i].b.totRespTime <<endl;
        if(tasks[i].ilpR < tasks[i].b.totRespTime && tasks[i].ilpR-tasks[i].b.totRespTime<0.01)

            debug10<<"response time error "<<tasks[i].ilpR <<"-"<<tasks[i].b.totRespTime <<endl;
    }
        
        
}

float csl(int resid)
{
    set s0;
    for(int i=0;i<resources[resid].tasks.size();i++)
        s0.setTasks.push_back(resources[resid].tasks[i]);
    
    if(s0.setTasks.size()>1)
    {
        float total=0.0;
        for(int i=0;i<s0.setTasks.size();i++)
            for(int j=0;j<tasks[s0.setTasks[i]].critSec.size();j++)
                if(tasks[s0.setTasks[i]].critSec[j].resID==resid)
                    total+=tasks[s0.setTasks[i]].critSec[j].length;
            
      
		return total;
    }
    else
        return 0.0;
}

//bool readersort(pair <int,int> p1, pair <int,int> p2)
//{
//    if(p1.second < p2.second)
//        return true;
//    return false;
//}


int calcMaxMem(set s)
{
    int writer=s.setTasks[0];
    int tw=tasks[writer].t;
    int minNumBuff=0;
    if(s.setTasks.size()-1 > 0)
    {
        int trmax=0;
        for(int i=1;i<s.setTasks.size();i++)
            trmax=max(trmax,tasks[s.setTasks[i]].t);
        minNumBuff=1+max(2,int(1+ceil(float(trmax)/float(tw))));
        
    }
     return minNumBuff*tasks[writer].succSize;
}

//int calcMaxMem(set s)
//{
//   
//   
//    int writer=s.setTasks[0];
//    int tw=tasks[writer].t;
//    int slow=0,fast=0;
//    int minNumBuff=0,splitPoint=-1;
//
//    if(s.setTasks.size()-1 > 0)
//    {
//        
//        pair <int,int> temppair;
//        vector < pair<int,int> > readers;
//        int nmax=0;
//        
//        for(int i=1;i<s.setTasks.size();i++)
//        {
//            nmax=max(2,int(1+ceil(float( tasks[s.setTasks[i]].t )/float(tw) )));
//            readers.push_back(make_pair(s.setTasks[i],nmax));
//        }
//        
//        sort(readers.begin(),readers.end(),readersort);
//        slow=readers.size()+2;
//        minNumBuff=slow;
//        
//        for(int i=0;i<readers.size();i++)
//        {
//            fast=readers[i].second+1;
//            slow=1*readers.size()-i-1;
//            
//            if(slow+fast <= minNumBuff)
//            {
//                splitPoint=i;// index in readers of first fast task
//                minNumBuff=slow+fast;
//            }
//        }
//        
//        
//         
//    }
//   return minNumBuff*tasks[writer].succSize;
//}


int solutionCost()
{
    int cost=0;
    set s;
    
    
    for(int i=0;i<resources.size();i++)
        if(resources[i].active==true && resources[i].protocol==2)
        {        
           
            s.setTasks.clear();
            for(int j=0;j<resources[i].tasks.size();j++)
                    s.setTasks.push_back(resources[i].tasks[j]);
            if(s.setTasks.size() > 1)
                cost+=calcMaxMem(s);
            //cout<<"RES"<<i<<" "<<calcMaxMem(s)<<endl;
            
            
        }   

return cost;    
}


int calcMemCostPS(int processorID,int setpos)
{
    int cost=0;
    set s;
    for(int i=0;i<sets[setpos].setTasks.size();i++)
    if(tasks[sets[setpos].setTasks[i]].procAlloc == -1)
	{
		
        s.setTasks.clear();
        int swriterID=sets[setpos].setTasks[i];
        s.setTasks.push_back(swriterID);
        for(int j=0;j<tasks[swriterID].succ.size();j++)
            if( (tasks[tasks[swriterID].succ[j]].procAlloc != processorID) &&
                (tasks[tasks[swriterID].succ[j]].procAlloc != -1) )
                s.setTasks.push_back(tasks[swriterID].succ[j]);
        if(s.setTasks.size() >1)
            cost+=calcMaxMem(s);
        

    }

    vector <int> preds;
    set s2;
 
    for(int i=0;i<sets[setpos].setTasks.size();i++)
		if(tasks[sets[setpos].setTasks[i]].procAlloc == -1)
        for(int j=0;j<tasks[sets[setpos].setTasks[i]].pred.size();j++)
            if( (find(preds.begin(),preds.begin()+preds.size(),tasks[sets[setpos].setTasks[i]].pred[j]) == preds.end())
                 && ( tasks[tasks[sets[setpos].setTasks[i]].pred[j]].procAlloc != -1) 
                 && ( tasks[tasks[sets[setpos].setTasks[i]].pred[j]].procAlloc != processorID) )
                preds.push_back(tasks[sets[setpos].setTasks[i]].pred[j]);
        

        
        for(int i=0;i<preds.size();i++)
        {
           s2.setTasks.clear();
           int s2writerID=preds[i];
           s2.setTasks.push_back(s2writerID);
    
            for(int j=0;j<tasks[s2writerID].succ.size();j++)
                if( (tasks[tasks[s2writerID].succ[j]].procAlloc != -1) && 
                    (tasks[tasks[s2writerID].succ[j]].procAlloc != tasks[s2writerID].procAlloc) )
                    s2.setTasks.push_back(tasks[s2writerID].succ[j]);
            if(s2.setTasks.size() >1)
                cost-=calcMaxMem(s2);
           
            
                   
            for(int j=0;j<sets[setpos].setTasks.size();j++)
                if(graph[s2writerID][sets[setpos].setTasks[j]]==true)         
                        s2.setTasks.push_back(sets[setpos].setTasks[j]);
            
            if(s2.setTasks.size() >1)
                cost+=calcMaxMem(s2);
            
            
                
        }
        
    return cost;
}


void generateSets()
{
    //maxPossCost=1;
      for(int i=0;i<tasks.size();i++)
      {
          set s2;
          s2.type=0;
          s2.setTasks.clear();
         
          s2.util=tasks[i].u;
          s2.setTasks.push_back(i);
          s2.maxMemory=0;/////////////////////////////////////////////////
          s2.bestp=-1;
          s2.lastChance=0;
          //s2.csl=csl(s2);
          s2.csl=0;
          sets.push_back(s2);
          
          
      }  
}


void assignSet(int setID, int procID) 
{
    
    assignTask(sets[setID].setTasks[0],procID);
    sets.erase(sets.begin()+setID);
   
}

void wfall()
{
    for(int i=0;i<resources.size();i++)
        if(resources[i].protocol==0 || resources[i].protocol==1)
            resources[i].protocol=2;
}

void resetPriorities()
{
    vector <int> tasks2;
    for(int i=0;i<tasks.size();i++)
        tasks2.push_back(i);
    
    sort(tasks2.begin(),tasks2.end(),prioritysort);
    
    for(int i=0;i<tasks2.size();i++)
        tasks[tasks2[i]].priority = i;
    
       for(int i=0;i<tasks.size();i++)
        for(int j=0;j<tasks[i].critSec.size();j++)
            tasks[i].critSec[j].csPriority=tasks[i].priority;
    
}

void deAllocateTasks()
{
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc!=-1)
        unassignTask(i,tasks[i].procAlloc);
    
    processors.clear();
    generateProcessors();
    assignedTaskCount=0;
    
    for(int i=0;i<resources.size();i++)
        resources[i].tasks.clear();
   
    sets.clear();
    generateSets();
    
    resetPriorities();
    
}


bool candprocsortx(pair <int,int> c1, pair <int,int> c2)
{
    if(c1.first < c2.first)
        return true;
    if(c1.first > c2.first)
        return false;
    if(processors[c1.second].u > processors[c2.second].u)
        return true;
    return false;
    
}



bool systemsort(sys s1, sys s2)
{
    if(s1.cost < s2.cost)
        return true;
    return false;
}


bool setsortx(set s1, set s2)
{
    
    if (s1.lastChance >  s2.lastChance)
        return true;

	if (s1.lastChance <  s2.lastChance)
        return false;
	
    if(s1.util > s2.util)
        return true;
    
    return false;
}

bool setsort(set s1, set s2)
{
    if (s1.maxMemory >  s2.maxMemory)
        return true;

	if (s1.maxMemory <  s2.maxMemory)
        return false;
	
    if(s1.util > s2.util)
        return true;
    
    return false;
}


bool utilsortLH(int t1,int t2)
{
    if(tasks[t1].u < tasks[t2].u)
        return true;
    return false;
}

bool utilsortHL(int t1,int t2)
{
    if(tasks[t1].u > tasks[t2].u)
        return true;
    return false;
}

bool periodsortLH(int t1,int t2)
{
    if(tasks[t1].t < tasks[t2].t)
        return true;
    return false;
}


bool cpusortHL(int p1,int p2)
{
    if(processors[p1].u > processors[p2].u)
        return true;
    return false;
}


bool cpusortLH(int p1,int p2)
{
    if(processors[p1].u < processors[p2].u)
        return true;
    return false;
}


bool bdftasksort(int p1, int p2)
{
    if(tasks[p1].u > tasks[p2].u)
        return true;
    return false;
}


void printCost()
{
	set s;
	 for(int i=0;i<resources.size();i++)
        if(resources[i].active==true && resources[i].protocol==2)
        {        
      
            for(int j=0;j<resources[i].tasks.size();j++)
                    s.setTasks.push_back(resources[i].tasks[j]);

            s.setTasks.clear();
        } 		
}


void printPartitioning()
{
    debug2<<seed<<endl;
         debug3<<seed<<endl;
        for(int i=0;i<tasks.size();i++)
            debug2<<i<<"    "<<tasks[i].procAlloc<<"    "<<tasks[i].priority<<endl;
        for(int i=0;i<resources.size();i++)
        if(resources[i].active==true)
        {
            debug3<<i<<'\t'<<resources[i].protocol<<'\t'<<resources[i].tasks[0];
            if(resources[i].protocol==2)
            {
                set sss;
                sss.setTasks.clear();
                for(int j=0;j<resources[i].tasks.size();j++)
                    sss.setTasks.push_back(resources[i].tasks[j]);
                debug3<<'\t'<<calcMaxMem(sss);
            }
            debug3<<endl;
                    
            for(int j=0;j<resources[i].tasks.size();j++)
               debug3<<resources[i].tasks[j]<<" ";
                    debug3<<endl;
        }
        
        debug2<<"------------------------------------"<<endl;
        debug3<<"------------------------------------"<<endl;
}

//void printRT()
//{
//    debug7<<seed<<endl;
//                 for(int i=0;i<tasks.size();i++)
//                     debug7<<i<<"     "<<tasks[i].b.totRespTime<<"    "<<tasks[i].t<<endl;
//                 debug7<<"---------------------------------------------"<<endl;
//}



#define	UTILITIES_H



#endif	/* UTILITIES_H */

//  tasks.clear();
//        
//        task r0,r1,r2,r3,r4,r5,r6,w0;
//        w0.succSize=1;
//        w0.t=10;
//        r0.t=5;
//        r1.t=125;
//        r2.t=14;
//        r3.t=475;
//        r4.t=20;
//        r5.t=4;
//        r6.t=9;
//        
//        tasks.push_back(w0);
//        tasks.push_back(r0);
//        tasks.push_back(r1);
//        tasks.push_back(r2);
//        tasks.push_back(r3);
//        tasks.push_back(r4);
//        tasks.push_back(r5);
//        tasks.push_back(r6);
//       
//        set ss;
//        for(int i=0;i<tasks.size();i++)
//            ss.setTasks.push_back(i);
//        
//        cout<<calcMaxMem(ss)<<endl;
//        