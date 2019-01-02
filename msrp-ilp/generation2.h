/* 
 * File:   generation2.h
 * Author: Zaid
 *
 * Created on October 6, 2013, 3:25 AM
 */

#ifndef GENERATION2_H

const int procCnt=3;
const int numRes=4;
const float  minPeriod=10;
const float maxPeriod=100;
const float rsf=0.4;
const float minCsLength=0.001;
const float maxCsLength=0.1;


float avgU;        
int totalTaskCnt=10;
const int maxWritersPerTask=3;//CHANGE/////////////////////////////////////////////////////
int maxCsPerTask=totalTaskCnt;


struct resource
{
    int resID;
    bool active;
    int writer;
    int protocol;
    vector <int> tasks;
};

struct cs
{
    int resID;
    int writerID;
    float length;
    float csPriority;
};

struct blockingParameters
{
    int numArr;
    float rmpcp;
    float rmsrp;
    float lmpcp;
    float lmsrp;
    float totRespTime;
    vector <float> rmpcpW;
    vector <float> rmsrpCS;
};

struct task
{
        int tID;
        int num_succ;
	vector<int> succ;
	vector<int> pred;
	int succSize;
        float c;
        int t;
        float u;
        int priority;
        int procAlloc;                           //-1 unalocated
        vector <cs> critSec;
        vector <float> normalSec;
        blockingParameters b;
        deque <int> resids;
	float ilpB;
        float ilpR;

};

struct set
{
    vector <int> setTasks;
    int maxMemory;
    float util;
    int bestp;
    float csl;
    int type;
    int lastChance;
};

struct processor
{
    int pID;
    vector <int> tasks;
    float u;
};

struct sys
{
   vector <task> btasks;
   vector <set> bsets;
   vector <processor> bprocessors;
   vector <resource> bresources;
   set nextset;
   int nextproc;
   int cost;
};

vector <task> tasks;
vector <set> sets;
vector <processor> processors;
vector <resource> resources;
vector < vector<bool> > graph;



time_t seed;
int node_counter=0;
int assignedTaskCount=0;
int resMode=1; 

bool prioritysort(int t1, int t2){
    if(tasks[t1].t < tasks[t2].t)
        return true;
    if( (tasks[t1].t == tasks[t2].t) && (t1<t2) )
            return true;
    return false;
}

void generateTasks(int counter){
   
   
    pair <int,int> temppair;
    node_counter=0;
    int randomNum=0;
    float ftemp1=0.0,ftemp2=0.0;
    vector <bool> row;
     
    float util=avgU*totalTaskCnt;
    float taskPerRes=(totalTaskCnt*rsf)-1;
    cs cstemp;
    float nonCs;
    bool alternate=false;
    if(taskPerRes > floor(taskPerRes))
        alternate=true;
    
    seed++;
    srand(seed);
    debug1<<seed<<endl;
    
    node_counter=totalTaskCnt;
    tasks.resize(node_counter);
    
    for(int i=0;i<node_counter;i++)
    {
         row.clear();
        for(int j=0;j<node_counter;j++)
            row.push_back(false);
        graph.push_back(row);   
    }
   
    for(int i=0;i<node_counter;i++)
    {
        tasks[i].num_succ=0;
        tasks[i].tID=i;
        tasks[i].pred.clear();
        tasks[i].succ.clear();
        tasks[i].procAlloc=-1;
        tasks[i].critSec.clear();
        tasks[i].normalSec.clear();
        tasks[i].b.lmpcp=0.0;
        tasks[i].b.lmsrp=0.0;
        tasks[i].b.rmpcp=0.0;
        tasks[i].b.rmsrp=0.0;
        tasks[i].b.numArr=1;
        tasks[i].b.totRespTime=0;
        tasks[i].b.rmpcpW.clear();
        tasks[i].b.rmsrpCS.clear();
        tasks[i].resids.clear();
    }
   
    float tg=1.0;
    float ra;
    float LA = log(minPeriod); 
    float LB = log(maxPeriod+tg);
     for(int n=0;n<tasks.size();n++)
     {
        ra=LA + (LB-LA) * (float(rand())/float(RAND_MAX));
        tasks[n].t=tg*floor(float(exp(ra))/tg);
     } 
     
    
    vector <int> tasks2;
    for(int i=0;i<tasks.size();i++)
        tasks2.push_back(i);
    sort(tasks2.begin(),tasks2.end(),prioritysort);
    for(int i=0;i<tasks2.size();i++)
        tasks[tasks2[i]].priority = i;

    
 float c1[100];
    for(int i=0;i<node_counter;i++)
    {
        in1>>c1[i];
       
    }
    for (int n=0;n<node_counter;n++)
    {
        tasks[n].u=c1[n];
        tasks[n].c=tasks[n].u*tasks[n].t;
    }

 
    for(int n=0;n<node_counter;n++)
    {
        randomNum=rand()%10;
		switch(randomNum)
		{
		case 0:
			tasks[n].succSize=1;
			break;
                case 1:
                case 2:           
			tasks[n].succSize=4;
			break;
		case 3:
                case 4:
			tasks[n].succSize=24;
			break;
		case 5:
                        tasks[n].succSize=48;
			break;
                case 6:
                case 7:
			tasks[n].succSize=128;
			break;
		case 8:
			tasks[n].succSize=256;
			break;
		case 9:
			tasks[n].succSize=512;
			break;
		
        }
    }


 
   vector <int> readers;
   vector <int> writers;
   writers.resize(tasks.size());
   
   vector<int>::iterator it;
   int randWriter;
    for(int i=0;i<numRes;i++)
    {
        if(alternate && i%2==0)
            taskPerRes-=1;
        if(alternate && i%2==1)
            taskPerRes+=1;
         int rid=resources.size();

        do
        {

           randWriter=rand()%totalTaskCnt; 
          
           
           if(writers[randWriter] < maxWritersPerTask)
            {
            
                writers[randWriter]++;
                tasks[randWriter].resids.push_back(rid);
                tasks[randWriter].resids.push_back(rid+1);

                resource r1,r2; 
            
                r1.active=false;
                r1.protocol=9;
                r1.resID=resources.size();
                r1.tasks.clear();
                r1.writer=randWriter;
                resources.push_back(r1);
                r2.active=false;
                r2.protocol=9;
                r2.resID=resources.size();
                r2.tasks.clear();
                r2.writer=randWriter;

                resources.push_back(r2);
               
                    break;
            }
        }
        while(1);   

        int randReader;
        readers.clear();
        readers.push_back(randWriter);
        for(int j=0;j<taskPerRes;j++)
        {

            do
            {
                randReader=rand()%tasks.size();
                it = find (readers.begin(), readers.end(), randReader);
                if(it==readers.end() && tasks[randReader].pred.size()<maxCsPerTask)
                {
                    readers.push_back(randReader);
                    it = find (tasks[randWriter].succ.begin(), tasks[randWriter].succ.end(), randReader);
                     if(it==tasks[randWriter].succ.end())
                     {
                    tasks[randWriter].num_succ++;
                    tasks[randWriter].succ.push_back(randReader);
                    tasks[randReader].pred.push_back(randWriter);
                     }
                    tasks[randReader].resids.push_front(rid);
                    graph[randWriter][randReader]=true;
                    break;
                }
            }
            while(1);
        }
    }

   
for(int n=0;n<tasks.size();n++)
{
//    int bound=tasks[n].pred.size();
//    if(tasks[n].succ.size()>0)
//        bound+=2*tasks[n].resids.size();
        
       for(int i=0;i<tasks[n].resids.size();i++)
       {
        
            ftemp2 = (minCsLength + (float)rand()/((float)RAND_MAX/(maxCsLength-minCsLength)));
            
            cstemp.writerID=resources[tasks[n].resids[i]].writer;
//            if(i<tasks[n].pred.size())
//            {
//                cstemp.writerID=tasks[n].pred[i];
//                
//            }
//            else
//            {
//                cstemp.writerID=n;
//            }
            cstemp.resID=tasks[n].resids[i];

            cstemp.csPriority=tasks[n].priority;
            cstemp.length=ftemp2;
            tasks[n].critSec.push_back(cstemp); 
       }
    
    
    float totcslen=0.0;
    bool cssafe=false;
    
    do
    {        
        cssafe=true;
        totcslen=0.0;
        for(int j=0;j<tasks[n].critSec.size();j++)
                totcslen+=tasks[n].critSec[j].length;
        if(totcslen > tasks[n].c)
        {
            cssafe=false;
            float cold=0.0,cnew=0.0;
            cold=totcslen;
           
             for(int j=0;j<tasks[n].critSec.size();j++)
                 if(tasks[n].critSec[j].length > 2*minCsLength)
                        tasks[n].critSec[j].length=tasks[n].critSec[j].length/2.0;
             for(int j=0;j<tasks[n].critSec.size();j++)
                cnew+=tasks[n].critSec[j].length;
             
             if(cold==cnew)
             {
                 tasks[n].c=tasks[n].critSec.size()*2*minCsLength;
                tasks[n].u=float(tasks[n].c)/float(tasks[n].t);
             }
        }
    }
    while(cssafe==false);

}

for(int n=0;n<tasks.size();n++)
{
    
    nonCs=tasks[n].c;
    
    for(int j=0;j<tasks[n].critSec.size();j++)
        nonCs-=tasks[n].critSec[j].length;
    if(nonCs < 0)
    {
        debug9<<"Generation error"<<endl;  
        cout<<"Generation Error"<<endl;
    }
    
    
    tasks[n].normalSec.clear();
    tasks[n].normalSec.resize(tasks[n].critSec.size()+1);
    for(int i=1;i<tasks[n].normalSec.size();i++)
    {
        ftemp1=(float) rand()/RAND_MAX;
  
	ftemp2=nonCs*pow(ftemp1,1.0/float(tasks[n].normalSec.size()-i));
        tasks[n].normalSec[i-1]=nonCs-ftemp2;
	nonCs=ftemp2;
    }    
    tasks[n].normalSec[tasks[n].normalSec.size()-1]=nonCs;
    }

  float uuuu=0.0;
    for(int i=0;i<tasks.size();i++)
        uuuu+=tasks[i].u;      
    
  debug6<<uuuu<<endl;
  
  
debug8<<seed<<endl;
    for(int i=0;i<tasks.size();i++)
    {
        debug8<<i<<"    "<<tasks[i].u<<"    "<<tasks[i].t<<"    "<<tasks[i].c<<"       ";
        for(int j=0;j<tasks[i].resids.size();j++)
            debug8<<tasks[i].resids[j]<<" ";
        debug8<<"M:"<<tasks[i].succSize<<endl;
        for(int j=0;j<tasks[i].critSec.size();j++)
            debug8<<tasks[i].critSec[j].writerID<<"-"<<tasks[i].critSec[j].length<<"-"<<tasks[i].critSec[j].resID<<"  ";
        debug8<<endl;

       
        
    }
  debug8<<"-------------------"<<endl;
 

}


void generateResources()
{
    
    resources.clear();
    resource r0;
    r0.protocol=9;
    r0.resID=0;
    r0.active=false;

    resources.push_back(r0);
    
//      resource r1,r2;
//    for(int i=0;i<tasks.size();i++)
//       
//        {
//      
//            resource r1,r2; 
//            
//            r1.active=false;
//            r1.protocol=9;
//            r1.resID=resources.size();
//            
//            r1.tasks.clear();
//            resources.push_back(r1);
//            
//            r2.active=false;
//            r2.protocol=9;
//            r2.resID=resources.size();
//           
//            r2.tasks.clear();
//            
//            resources.push_back(r2);
//        }
//    for(int i=0;i<tasks.size();i++)
//        for(int j=0;j<tasks[i].critSec.size();j++)
//            tasks[i].critSec[j].resID=0;
}

void generateProcessors(){
    processors.resize(procCnt);
    for(int i=0;i<procCnt;i++)
    {
        processors[i].pID=i;
        processors[i].tasks.clear();
        processors[i].u=0;
    }
}


void setMpcpPrio(int resID)
{
    for(int j=0;j<resources[resID].tasks.size();j++)
    {
        int minp=node_counter;
        int t1=resources[resID].tasks[j];
        for(int k=0;k<resources[resID].tasks.size();k++)
            if(tasks[resources[resID].tasks[k]].procAlloc != tasks[t1].procAlloc)
                minp=min(minp,tasks[resources[resID].tasks[k]].priority);
        for(int k=0;k<tasks[t1].critSec.size();k++)
            if(tasks[t1].critSec[k].resID==resID)
            {
                tasks[t1].critSec[k].csPriority=(-1*node_counter)+minp-1;
                break;
            }
    }
    
}

void setWfPrio(int resID)
{
    for(int j=0;j<resources[resID].tasks.size();j++)
        for(int k=0;k<tasks[resources[resID].tasks[j]].critSec.size();k++)
            if(tasks[resources[resID].tasks[j]].critSec[k].resID == resID)
            {
                tasks[resources[resID].tasks[j]].critSec[k].csPriority=tasks[resources[resID].tasks[j]].priority;
                break;
            }
}

void setMsrpPrio(int resID)
{
    for(int j=0;j<resources[resID].tasks.size();j++)
        for(int k=0;k<tasks[resources[resID].tasks[j]].critSec.size();k++)
            if(tasks[resources[resID].tasks[j]].critSec[k].resID == resID)
            {
                tasks[resources[resID].tasks[j]].critSec[k].csPriority=(-1*node_counter)-2;
                break;
            }
}


void checkRes(int resID)
{
    
    vector <int> local;
    vector <int> global;
     int writer=resources[resID].writer;
    
        resources[resID].active=false;
        resources[resID].protocol=9;
        resources[resID].tasks.clear();
        
        resources[resID+1].active=false;
        resources[resID+1].protocol=9;
        resources[resID+1].tasks.clear();
    
        
    if(tasks[writer].procAlloc != -1)
    {
        for(int j=0;j<tasks[writer].succ.size();j++)
         if(tasks[tasks[writer].succ[j]].procAlloc!=-1)
        {

           
                for(int k=0;k<tasks[tasks[writer].succ[j]].critSec.size();k++)
                    if(tasks[tasks[writer].succ[j]].critSec[k].resID==resID || 
                       (tasks[tasks[writer].succ[j]].critSec[k].resID==resID+1))
                    {

                        if(tasks[writer].procAlloc != tasks[tasks[writer].succ[j]].procAlloc)
                                global.push_back(tasks[writer].succ[j]);
                        else
                                local.push_back(tasks[writer].succ[j]); 
                    }

        }               
                
                
        if(!global.empty())
        {
            resources[resID].active=true;
            resources[resID].protocol=resMode;
            //tasks[writer].critSec[tasks[writer].pred.size()].resID=resID;
            resources[resID].tasks.push_back(writer);
            
            for(int i=0;i<global.size();i++)
            {
                    resources[resID].tasks.push_back(global[i]);
                    for(int j=0;j<tasks[global[i]].critSec.size();j++)
                        if(tasks[global[i]].critSec[j].resID==resID || tasks[global[i]].critSec[j].resID==resID+1)
                        {
                            
                            tasks[global[i]].critSec[j].resID=resID;
                            //tasks[local[i]].critSec[j].csPriority=csPrio;
                           
                            break;
                        }

            }
            if(resMode==0)
                setMpcpPrio(resID);
            if(resMode==2)
                setWfPrio(resID);
            if(resMode==1)
                setMsrpPrio(resID);

        }
        if(!local.empty())
        {

            resources[resID+1].active=true;
            resources[resID+1].protocol=3;
            int csPrio=tasks[writer].priority;
                
            
            
            resources[resID+1].tasks.push_back(writer);
            
            for(int i=0;i<local.size();i++)
            {
               
                    resources[resID+1].tasks.push_back(local[i]);

                    for(int j=0;j<tasks[local[i]].critSec.size();j++)
                        if(tasks[local[i]].critSec[j].resID==resID || tasks[local[i]].critSec[j].resID==resID+1)
                        {
                            
                            tasks[local[i]].critSec[j].resID=resID+1;
                           
                            csPrio=min(csPrio,tasks[local[i]].priority);
                            break;
                        }
            }
            
            for(int i=0;i<resources[resID+1].tasks.size();i++)
                for(int j=0;j<tasks[resources[resID+1].tasks[i]].critSec.size();j++)
                {
                    if(tasks[resources[resID+1].tasks[i]].critSec[j].resID==resID+1 )
                        tasks[resources[resID+1].tasks[i]].critSec[j].csPriority=csPrio;
                   
                }

        }
    }
    else
    {
         for(int j=0;j<tasks[writer].succ.size();j++)
            for(int k=0;k<tasks[tasks[writer].succ[j]].critSec.size();k++)
                if(tasks[tasks[writer].succ[j]].critSec[k].resID==resID)
                {
                    
                    tasks[tasks[writer].succ[j]].critSec[k].csPriority=  tasks[tasks[writer].succ[j]].priority;
                    
                }
    }

    
}



void assignTask(int taskID, int procID)
{
   

    
    if(tasks[taskID].procAlloc != -1)
    {
        cout<<"ASSIGN FAIL IN "<<taskID<<endl;
        exit(1);
    }
    assignedTaskCount++; 
    processors[procID].tasks.push_back(taskID);
    processors[procID].u+=tasks[taskID].u;
    tasks[taskID].procAlloc=procID;
    
    vector <int> rtc;
    for(int i=0;i<tasks[taskID].critSec.size();i++)
    {
        int x=tasks[taskID].critSec[i].resID;
        if(tasks[taskID].critSec[i].resID%2==0)
            x--;
        if(x==-1)
        {
            cout<<"resource=0"<<endl;
            exit(1);
        }
        if(find(rtc.begin(),rtc.end(),x)==rtc.end())
           rtc.push_back(x);
    }
    
    for(int i=0;i<rtc.size();i++)
        checkRes(rtc[i]);


}

void unassignTask(int taskID, int procID)
{

    if(tasks[taskID].procAlloc != procID)
    {
        cout<<"UNASSIGN FAIL"<<endl;
        exit(1);
    }

    assignedTaskCount--;
    int loc=-1;
    
    for(int i=0;i<processors[procID].tasks.size();i++)
        if(processors[procID].tasks[i] == taskID)
        {
            loc=i;
            break;
        }
    if(loc!=-1)
    {
        processors[procID].tasks.erase(processors[procID].tasks.begin()+loc);
        processors[procID].u-=tasks[taskID].u;
        tasks[taskID].procAlloc=-1;
    }
      vector <int> rtc;
    for(int i=0;i<tasks[taskID].critSec.size();i++)
    {
        int x=tasks[taskID].critSec[i].resID;
        if(tasks[taskID].critSec[i].resID%2==0)
            x--;
        if(x==-1)
        {
            cout<<"resource=0"<<endl;
            exit(1);
        }
        if(find(rtc.begin(),rtc.end(),x)==rtc.end())
           rtc.push_back(x);
    }
    
    for(int i=0;i<rtc.size();i++)
        checkRes(rtc[i]);
     
   
    
    for(int i=0;i<tasks[taskID].critSec.size();i++)
    {
        
        tasks[taskID].critSec[i].csPriority=tasks[i].priority;
    }

}




#define	GENERATION2_H



#endif	/* GENERATION2_H */

