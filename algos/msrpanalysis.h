/* 
 * File:   msrpanalysis.h
 * Author: Zaid
 *
 * Created on October 8, 2013, 12:49 AM
 */

#ifndef MSRPANALYSIS_H
float maxRespTimeInc;

void resetCsPara()
{
    for(int i=0;i<tasks.size();i++)
    {
        tasks[i].b.lmpcp=0.0;
        tasks[i].b.lmsrp=0.0;
        tasks[i].b.rmpcp=0.0;
        tasks[i].b.rmsrp=0.0;
        tasks[i].b.numArr=1;
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

void calcMsrpCsPara()
{
    
    resetCsPara();
    int procID,resID;
    int writerID;


    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc !=-1)
        for(int j=0;j<tasks[i].critSec.size();j++)
        {
           
            procID=tasks[i].procAlloc;
            resID=resources[tasks[i].critSec[j].resID].resID;
             
            
            
            if(resources[resID].protocol==1 && resources[resID].active==true)
            {
                 
                float sums1[procCnt]={0.0};
                for(int k=0;k<tasks.size();k++)
                {
                    if(tasks[k].procAlloc != procID && tasks[k].procAlloc != -1)
                        for(int m=0;m<tasks[k].critSec.size();m++)
                            if(tasks[k].critSec[m].resID == resID)
                                sums1[tasks[k].procAlloc]=max(sums1[tasks[k].procAlloc],tasks[k].critSec[m].length);
                }
                            
                for(int n=0;n<procCnt;n++)
                     tasks[i].b.rmsrpCS[j]+=sums1[n];
            }
        }  
}


void calcMsrpRemoteBlocking()
{

//    for(int i=0;i<tasks[3].b.rmsrpCS.size();i++)
//        cout<<tasks[3].b.rmsrpCS[i]<<" ";
//    cout<<endl;
    
    int procID,resID;     
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc !=-1)
            for(int j=0;j<tasks[i].critSec.size();j++)
                if(resources[tasks[i].critSec[j].resID].active==true && resources[tasks[i].critSec[j].resID].protocol==1)
                    tasks[i].b.rmsrp+=tasks[i].b.rmsrpCS[j];
    
}


void calcMsrpLocalBlocking()
{
    int procID,resID;
      
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc !=-1)
    {
            procID=tasks[i].procAlloc;
       
        float lmpcpSum=0.0,bLocRes=0.0,bGloRes=0.0;
                            
        for(int k=0;k<processors[procID].tasks.size();k++)          
            if( tasks[processors[procID].tasks[k]].priority > tasks[i].priority)
            {
               
                
                 for(int m=0;m<tasks[processors[procID].tasks[k]].critSec.size();m++)
                {    

                    if(resources[tasks[processors[procID].tasks[k]].critSec[m].resID].protocol==3 
                            && resources[tasks[processors[procID].tasks[k]].critSec[m].resID].active==true &&
                            tasks[processors[procID].tasks[k]].critSec[m].csPriority <= tasks[i].priority )
                        bLocRes=max(bLocRes,tasks[processors[procID].tasks[k]].critSec[m].length);

                          
                    if(resources[tasks[processors[procID].tasks[k]].critSec[m].resID].protocol==1
                            && resources[tasks[processors[procID].tasks[k]].critSec[m].resID].active==true)             
                        bGloRes=max(bGloRes,tasks[processors[procID].tasks[k]].critSec[m].length+tasks[processors[procID].tasks[k]].b.rmsrpCS[m]);
                }
            }    
    

            tasks[i].b.lmsrp=tasks[i].b.numArr*max(bLocRes,bGloRes);
        
    }
}



void calcBlocking2(){


    calcMsrpCsPara();
    calcMsrpRemoteBlocking();
    calcMsrpLocalBlocking();

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
    
    for(int i=0;i<tasks.size();i++)
    if(tasks[i].procAlloc != -1)
    {
        ri2=tasks[i].c+tasks[i].b.lmsrp+tasks[i].b.rmsrp;
        procID=tasks[i].procAlloc;
                      
        do
        {
            ri1=ri2;
            ri2=tasks[i].c+tasks[i].b.lmsrp+tasks[i].b.rmsrp;
            
            for(int j=0;j<processors[procID].tasks.size();j++)
            {
                h=processors[procID].tasks[j];
                if(tasks[i].priority > tasks[h].priority)
                        ri2+=(tasks[h].c+tasks[h].b.rmsrp)*ceil(ri1/float(tasks[h].t));
            }

            if(ri2>tasks[i].t)
            {
                
                sched=false;
                
                diff=max(diff,ri2-tasks[i].t);
//				if(flag==true)
//				cout<<"The unsched is"<<i<<" on processor "<<tasks[i].procAlloc<<endl;
                break;
            }   
        }
        while(ri2>ri1);
        tasks[i].b.totRespTime=ri2;
       
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
       
        
    for(int i=0;i<tasks.size();i++)
    if(tasks[i].procAlloc == procToCheck)
    {
                ri2=tasks[i].c+tasks[i].b.lmsrp+tasks[i].b.rmsrp;
        procID=tasks[i].procAlloc;
                      
        do
        {
            ri1=ri2;
            ri2=tasks[i].c+tasks[i].b.lmsrp+tasks[i].b.rmsrp;
            
            for(int j=0;j<processors[procID].tasks.size();j++)
            {
                h=processors[procID].tasks[j];
                if(tasks[i].priority > tasks[h].priority)
                        ri2+=(tasks[h].c+tasks[h].b.rmsrp)*ceil(ri1/float(tasks[h].t));
            }
            if(ri2>tasks[i].t)
            {
                sched=false;
                diff=max(diff,ri2-tasks[i].t);
//				if(flag==true)
//				cout<<"The unsched is"<<i<<" on processor "<<tasks[i].procAlloc<<endl;
                break;
            }   
        }
        while(ri2>ri1);
        tasks[i].b.totRespTime=ri2;
       
    } 

    if(sched==false)
        maxRespTimeInc=diff;
    
    return sched;          
}

#define	MSRPANALYSIS_H



#endif	/* MSRPANALYSIS_H */

