/* 
 * File:   mpcpAnalysis.h
 * Author: admin
 *
 * Created on April 21, 2015, 12:56 AM
 */

#ifndef MPCPANALYSIS_H

#include "generation2.h"


float maxRespTimeInc;

void resetCsPara()
{
    for(int i=0;i<tasks.size();i++)
    {
        tasks[i].b.lmpcp=0.0;
        tasks[i].b.lmsrp=0.0;
        tasks[i].b.rmpcp=0.0;
        tasks[i].b.rmsrp=0.0;
        tasks[i].b.numArr=1+tasks[i].critSec.size();
        tasks[i].b.totRespTime=0.0;
        tasks[i].b.rmpcpW.clear();
        tasks[i].b.rmsrpCS.clear();
        for(int j=0;j<tasks[i].critSec.size();j++)
        {
            tasks[i].b.rmpcpW.push_back(0.0);
            tasks[i].b.rmsrpCS.push_back(0.0);
        }
 
       
    }
    
}

void calcMpcpCsPara()
{
    
    resetCsPara();
    
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc !=-1)
            for(int j=0;j<tasks[i].critSec.size();j++)
            if(resources[tasks[i].critSec[j].resID].active==true)    
            {
                int procID=tasks[i].procAlloc;
                //resID=resources[tasks[i].critSec[j].resID].resID;
                float lmpcpSum=0.0;
                for(int k=0;k<processors[procID].tasks.size();k++)
                    if(i!=processors[procID].tasks[k])
                    {
                        
                       float btemp=0.0;
                        for(int m=0;m<tasks[processors[procID].tasks[k]].critSec.size();m++)
                           if(resources[tasks[processors[procID].tasks[k]].critSec[m].resID].protocol==0
                                   && resources[tasks[processors[procID].tasks[k]].critSec[m].resID].active==true
                                   && tasks[processors[procID].tasks[k]].critSec[m].csPriority < tasks[i].critSec[j].csPriority )             
                               btemp=max(btemp,tasks[processors[procID].tasks[k]].critSec[m].length);
                        
                        lmpcpSum+=btemp;
                        
                    }

                tasks[i].b.rmpcpW[j]=tasks[i].critSec[j].length+lmpcpSum;
            }
    
}


void calcMpcpRemoteBlocking()
{
    
    
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc !=-1)
        {
            vector <float> csRemB(tasks[i].critSec.size(),0.0);
            
            for(int j=0;j<tasks[i].critSec.size();j++)
            {
                int resID=tasks[i].critSec[j].resID;
                if(resources[resID].active==true && resources[resID].protocol==0)
                {
                    float bijr1=0.0,bijr2=0.0;
                    float wkm=0.0;
                    for(int k=0;k<resources[resID].tasks.size();k++)
                        if(tasks[resources[resID].tasks[k]].procAlloc != -1 && tasks[resources[resID].tasks[k]].priority > tasks[i].priority)
                            for(int m=0;m<tasks[resources[resID].tasks[k]].critSec.size();m++)
                                if(tasks[resources[resID].tasks[k]].critSec[m].resID == resID)
                                    wkm=max(wkm,tasks[resources[resID].tasks[k]].b.rmpcpW[m]);
                
                    bijr2=wkm;
                    do
                    {
                        bijr1=bijr2;
                        bijr2=wkm;
                        for(int k=0;k<resources[resID].tasks.size();k++)
                            if(tasks[resources[resID].tasks[k]].procAlloc != -1 && tasks[resources[resID].tasks[k]].priority < tasks[i].priority)
                                for(int m=0;m<tasks[resources[resID].tasks[k]].critSec.size();m++)
                                    if(tasks[resources[resID].tasks[k]].critSec[m].resID == resID)
                                        bijr2+=tasks[resources[resID].tasks[k]].b.rmpcpW[m]*( 1+ceil(bijr1/float(tasks[k].t)) );
                    
                    }
                    while(bijr2 > bijr1);
                    csRemB[j]=bijr2;
                }
            }
            for(int j=0;j<csRemB.size();j++)
                tasks[i].b.rmpcp+=csRemB[j];
        }
}

void calcMpcpLocalBlocking()
{
    int procID,resID;
      
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc !=-1)
    {
            procID=tasks[i].procAlloc;
       
        float lmpcpSum=0.0;
        float btemp=0.0;
                            
        for(int k=0;k<processors[procID].tasks.size();k++)          
            if( tasks[processors[procID].tasks[k]].priority > tasks[i].priority)
            {
                btemp=0.0;
  
                 for(int m=0;m<tasks[processors[procID].tasks[k]].critSec.size();m++)
                {    

                    if(resources[tasks[processors[procID].tasks[k]].critSec[m].resID].protocol==3 
                            && resources[tasks[processors[procID].tasks[k]].critSec[m].resID].active==true)
                            //&& tasks[processors[procID].tasks[k]].critSec[m].csPriority <= tasks[i].priority )
                        btemp=max(btemp,tasks[processors[procID].tasks[k]].critSec[m].length);

                    if(resources[tasks[processors[procID].tasks[k]].critSec[m].resID].protocol==0
                            && resources[tasks[processors[procID].tasks[k]].critSec[m].resID].active==true)             
                        btemp=max(btemp,tasks[processors[procID].tasks[k]].critSec[m].length);
                }
                 lmpcpSum+=btemp;
            }

            tasks[i].b.lmpcp=tasks[i].b.numArr*lmpcpSum;
        
    }
}

#define	MPCPANALYSIS_H

void calcBlocking2(){


    calcMpcpCsPara();
    calcMpcpRemoteBlocking();
    calcMpcpLocalBlocking();

}

bool schedAnalysis2()
{
    float diff=0.0;
    float ri1,ri2; 
    int procID;
    int h;
    float sums1[procCnt]={0.0};
    bool sched=true;
   
    calcBlocking2();
    
     vector <int> tasks2;
    for(int i=0;i<tasks.size();i++)
        tasks2.push_back(i);
    sort(tasks2.begin(),tasks2.end(),prioritysort);
    
    
    for(int i=0;i<tasks2.size();i++)
    if(tasks[tasks2[i]].procAlloc != -1)
    {
        ri2=tasks[tasks2[i]].c+tasks[tasks2[i]].b.lmpcp+tasks[tasks2[i]].b.rmpcp;
        procID=tasks[tasks2[i]].procAlloc;
                      
        do
        {
            ri1=ri2;
            ri2=tasks[tasks2[i]].c+tasks[tasks2[i]].b.lmpcp+tasks[tasks2[i]].b.rmpcp;
            
            for(int j=0;j<processors[procID].tasks.size();j++)
            {
                h=processors[procID].tasks[j];
                if(tasks[tasks2[i]].priority > tasks[h].priority)
                        ri2+=tasks[h].c*ceil((ri1+(tasks[h].b.totRespTime - tasks[h].c))/float(tasks[h].t));
            }

            if(ri2>tasks[tasks2[i]].t)
            {
                
                sched=false;
                
                diff=max(diff,ri2-tasks[tasks2[i]].t);
//				if(flag==true)
//				cout<<"The unsched is"<<i<<" on processor "<<tasks[tasks2[i]].procAlloc<<endl;
                break;
            }   
        }
        while(ri2>ri1);
        tasks[tasks2[i]].b.totRespTime=ri2;
       
    } 

    if(sched==false)
        maxRespTimeInc=diff;

    return sched;
    
}

bool schedAnalysisProc(int procToCheck)
{

    float diff=0.0;
    float ri1,ri2; 
    int procID;
    int h;
    float sums1[procCnt]={0.0};
    bool sched=true;
   
   calcBlocking2();
    vector <int> tasks2;
    for(int i=0;i<tasks.size();i++)
        tasks2.push_back(i);
    sort(tasks2.begin(),tasks2.end(),prioritysort);
       
        
    for(int i=0;i<tasks2.size();i++)
    if(tasks[tasks2[i]].procAlloc == procToCheck)
    {
                ri2=tasks[tasks2[i]].c+tasks[tasks2[i]].b.lmpcp+tasks[tasks2[i]].b.rmpcp;
        procID=tasks[tasks2[i]].procAlloc;
                      
        do
        {
            ri1=ri2;
            ri2=tasks[tasks2[i]].c+tasks[tasks2[i]].b.lmpcp+tasks[tasks2[i]].b.rmpcp;
            
            for(int j=0;j<processors[procID].tasks.size();j++)
            {
                h=processors[procID].tasks[j];
                if(tasks[tasks2[i]].priority > tasks[h].priority)
                        ri2+=tasks[h].c*ceil((ri1+(tasks[h].b.totRespTime - tasks[h].c))/float(tasks[h].t));
            }
            if(ri2>tasks[tasks2[i]].t)
            {
                sched=false;
                diff=max(diff,ri2-tasks[tasks2[i]].t);
//				if(flag==true)
//				cout<<"The unsched is"<<i<<" on processor "<<tasks[tasks2[i]].procAlloc<<endl;
                break;
            }   
        }
        while(ri2>ri1);
        tasks[tasks2[i]].b.totRespTime=ri2;
       
    } 

    if(sched==false)
        maxRespTimeInc=diff;
    
    return sched;          
}


float checkassignment(int ptc)
{
    float diff=0.0;
    float ri1,ri2; 
    int procID;
    int h;
    float sums1[procCnt]={0.0};
    bool sched=true;
    float s=99999999;
   
    
    calcBlocking2();
    
     vector <int> tasks2;
    for(int i=0;i<tasks.size();i++)
        tasks2.push_back(i);
    sort(tasks2.begin(),tasks2.end(),prioritysort);
    
    for(int i=0;i<tasks2.size();i++)
    if( (tasks[tasks2[i]].procAlloc != -1) && ( tasks[tasks2[i]].procAlloc != ptc  || (tasks[tasks2[i]].procAlloc == ptc && tasks[tasks2[i]].priority!=0) ))
    {
        ri2=tasks[tasks2[i]].c+tasks[tasks2[i]].b.lmpcp+tasks[tasks2[i]].b.rmpcp;
        procID=tasks[tasks2[i]].procAlloc;
                      
        do
        {
            ri1=ri2;
            ri2=tasks[tasks2[i]].c+tasks[tasks2[i]].b.lmpcp+tasks[tasks2[i]].b.rmpcp;
            
            for(int j=0;j<processors[procID].tasks.size();j++)
            {
                h=processors[procID].tasks[j];
                if(tasks[tasks2[i]].priority > tasks[h].priority)
                        ri2+=tasks[h].c*ceil((ri1+(tasks[h].b.totRespTime - tasks[h].c))/float(tasks[h].t));
            }

            if(ri2>tasks[tasks2[i]].t)
            {
                
                sched=false;
                
                diff=max(diff,ri2-tasks[tasks2[i]].t);
//				if(flag==true)
//				cout<<"The unsched is"<<i<<" on processor "<<tasks[tasks2[i]].procAlloc<<endl;
                break;
            }   
        }
        while(ri2>ri1);
        tasks[tasks2[i]].b.totRespTime=ri2;
       
    } 

    if(sched==false)
    {
        
        return -1;
    }
    else
    {
        for(int j=0;j<processors[ptc].tasks.size();j++)
            if(tasks[processors[ptc].tasks[j]].priority !=0)
            {
                int ta=processors[ptc].tasks[j];
                s=min(s,(float(tasks[ta].t)-tasks[ta].b.totRespTime)/float(tasks[ta].t));
                
            }
        return s;
    }

    
}

#endif	/* MPCPANALYSIS_H */

