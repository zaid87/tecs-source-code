/* 
 * File:   heuristic.h
 * Author: Zaid
 *
 * Created on October 3, 2013, 12:17 AM
 */

#ifndef HEURISTIC_H

vector <sys> candidateSystems;
vector <sys> removedSystems;
sys optimalSys;
int optimalSysCost=-1;


bool cpusort(int p1,int p2)
{
    if(processors[p1].u > processors[p2].u)
        return true;
    return false;
}




void saveSystemInprio()
{
    
    sys s1;
    set s0;
    s1.btasks=tasks;
    s1.bprocessors=processors;
    s1.bresources=resources;
    s1.bsets=sets;
    s1.nextset=s0;
    s1.nextproc=-1;
    s1.cost=solutionCost();
    int loc;
    if(candidateSystems.empty())
        loc=0;
    for(int a=0;a<candidateSystems.size();a++)
        if(s1.cost < candidateSystems[a].cost)
        {
            loc=a;
            break;
        }
    
    candidateSystems.insert(candidateSystems.begin()+loc,s1);
    
    if(candidateSystems.size()==tasks.size()+1)
    {
        removedSystems.push_back(candidateSystems[candidateSystems.size()-1]);
        if(removedSystems.size() > 1* tasks.size())
            removedSystems.erase(removedSystems.begin());
        
        candidateSystems.erase(candidateSystems.begin()+candidateSystems.size()-1);
    }
    

}



void nextSystemInprio() //0: dont assign tasks, 1:assign task
{
    
    tasks=candidateSystems[0].btasks;
    processors=candidateSystems[0].bprocessors;
    resources=candidateSystems[0].bresources;
    sets=candidateSystems[0].bsets;
    
    assignedTaskCount=0;
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc != -1)
            assignedTaskCount++;
    
}


bool checkEquivalence()
{
    for(int i=0;i<candidateSystems.size();i++)
    {
        int j;
        for(j=0;j<tasks.size();j++)
            if(tasks[j].procAlloc !=  candidateSystems[i].btasks[j].procAlloc)
                break;
        if(j==tasks.size())
            return true;
    }
    for(int i=0;i<removedSystems.size();i++)
    {
        int j;
        for(j=0;j<tasks.size();j++)
            if(tasks[j].procAlloc !=  removedSystems[i].btasks[j].procAlloc)
                break;
        if(j==tasks.size())
            return true;
    }
    return false;
    
}


void generateSwaps()
{
    bool solFound=false;
    int thrshCost=candidateSystems[candidateSystems.size()-1].cost;
    
    removedSystems.push_back(candidateSystems[0]);
        if(removedSystems.size() > 1*tasks.size())
            removedSystems.erase(removedSystems.begin());
    candidateSystems.erase(candidateSystems.begin());
    
    
    for(int t=0;t<tasks.size();t++)
    {
        solFound=false;
        int currProc=tasks[t].procAlloc;
        unassignTask(t,tasks[t].procAlloc);
        
        for(int i=0;i<processors.size();i++)
            if(i!=currProc)
        {
               
            assignTask(t,i);
            wfall();
            if(schedAnalysis2()==true)
            {
                
               stage2v2();
               int sc=solutionCost();
                
                if(sc < thrshCost)
                    if(! checkEquivalence())
                {
                        
                   saveSystemInprio();
                   thrshCost=candidateSystems[candidateSystems.size()-1].cost;
                   if(sc < optimalSysCost)
                   {
                        optimalSys.bprocessors=processors;
                        optimalSys.bresources=resources;
                        optimalSys.bsets.clear();
                        optimalSys.btasks=tasks;
                        optimalSysCost=sc;
                      //  cout<<"opt="<<optimalSysCost<<endl;
                   } 
                }
            }
            
            unassignTask(t,i);
        }
           
       assignTask(t,currProc);
    ///////////////////////////////////////////////////////////////////////////////
     for(int i=0;i<processors.size();i++)
                  if(i!=currProc)
    for(int n=0;n<processors[i].tasks.size();n++)
               {
                    
                    int j=processors[i].tasks[n];
                    
                    if(tasks[j].u >= tasks[t].u)
                    {        
                        for(int k=0;k<processors.size();k++)
                            if(k!=i)//k!=i
                            {
                                
                                unassignTask(t,tasks[t].procAlloc);
                                assignTask(t,i);
                                unassignTask(j,i);
                                assignTask(j,k);
                                wfall();
                                if(schedAnalysis2()==true)
                                {
                                    
                                    stage2v2();
                                   int sc=solutionCost();

                                    if(sc < thrshCost)
                                        if(! checkEquivalence())
                                    {

                                       saveSystemInprio();
                                       thrshCost=candidateSystems[candidateSystems.size()-1].cost;
                                       if(sc < optimalSysCost)
                                       {
                                            optimalSys.bprocessors=processors;
                                            optimalSys.bresources=resources;
                                            optimalSys.bsets.clear();
                                            optimalSys.btasks=tasks;
                                            optimalSysCost=sc;
                                          //  cout<<"opt="<<optimalSysCost<<endl;
                                       } 
                                    }
                                }
                                
                                unassignTask(j,k);
                                assignTask(j,i);
                                unassignTask(t,tasks[t].procAlloc);
                                assignTask(t,currProc);
                                     
                            } 
                    }
                        
            }
    }
    
    
}

void bnb()
{
    
    candidateSystems.clear();
    set sdummy;
    saveSystemInprio();
    saveSystemInprio();
    
    int loopCount=-1;
    int loopCost;
    
    while(!candidateSystems.empty())
    {
   
        
        nextSystemInprio();
        int oldopt=optimalSysCost;
        generateSwaps();
        if(oldopt==optimalSysCost)
            loopCount++;
        else
            loopCount=0;
        
        if(loopCount==10*tasks.size())
        {
            cout<<"break";
            breakused=true;
            break;
        }
        
        
        if(optimalSysCost==0)
            break;
            
    }

}

int updateSetsParameters()
{
   
    vector < pair <int,int> > candProc;
    for(int i=0;i<sets.size();i++)
    {
            candProc.clear();
            for(int j=0;j<processors.size();j++)
            {
                //int pre=solutionCost();
                assignTask(sets[i].setTasks[0],j);
                if(schedAnalysis2())
                {
                    //int tempp=solutionCost()-pre;
                    //cout<<tempp<<endl;
                    unassignTask(sets[i].setTasks[0],j);
                    int tempp=calcMemCostPS(j,i);
                    candProc.push_back(make_pair(tempp,j));
                    
                    
                }
                else
                    unassignTask(sets[i].setTasks[0],j);
                
            }
            if(candProc.size()==0)
            {
                //cout<<"Failed in task"<<sets[i].setTasks[0]<<endl;
                return -1;
            }
            if(candProc.size()==1)
            {
                sets[i].lastChance=99999999;
                sets[i].bestp=candProc[0].second;
               
            }
            else
            {
                sort(candProc.begin(),candProc.end(),candprocsortx);
                sets[i].lastChance=candProc[1].first-candProc[0].first;
                sets[i].bestp=candProc[0].second;
                
            }
            
            
    }
    return 0;
}

void BFD()
{

    
    resMode=2;
    bool schedulable=false;
    vector <int> taskids;
    for(int i=0;i<tasks.size();i++)
        taskids.push_back(i);
    sort(taskids.begin(),taskids.end(),bdftasksort);
    
    vector <int> cpus;
    for(int i=0;i<processors.size();i++)
        cpus.push_back(i);
    sort(cpus.begin(),cpus.end(),cpusort);
    
  for(int i=0;i<tasks.size();i++)
    for(int j=0;j<cpus.size();j++)
      {
            assignTask(i,cpus[j]);
            schedulable=schedAnalysis2();
//            if(!schedulable && j==cpus.size()-1)
//                debug2<<"Assignment Failed in task "<<i<<endl;
            if(!schedulable)
                unassignTask(i,cpus[j]);
            else
            {
                sort(cpus.begin(),cpus.end(),cpusort);
                break;
            }
        }

    bool solFound=false;
     if( (assignedTaskCount == node_counter && schedAnalysis2()))
    {
      
        solFound=true;
        
    }
    
        if(solFound)
        {
            
            stage2v2();
            
            optimalSys.bprocessors=processors;
            optimalSys.bresources=resources;
            optimalSys.bsets=sets;
            optimalSys.btasks=tasks;
            optimalSysCost=solutionCost();
          
            
            bnb();
            
            processors=optimalSys.bprocessors;
            resources=optimalSys.bresources;
            sets=optimalSys.bsets;
            tasks=optimalSys.btasks;
            assignedTaskCount=0;
            for(int i=0;i<tasks.size();i++)
                if(tasks[i].procAlloc != -1)
                    assignedTaskCount++;
            debug1<<"--------------------------------------------------------"<<endl;
            
        }
    
}


void runAlgo()
{
    optimalSysCost=-1;
    candidateSystems.clear();
    removedSystems.clear();
    
    optimalSys.btasks.clear();
    optimalSys.bsets.clear();
    optimalSys.bprocessors.clear();
    optimalSys.bresources.clear();
    
    
    breakused=false;
    resMode=2;
   
    bool solFound=false;

    
    do
    {
        if(updateSetsParameters()==-1)
            break;
        
        sort(sets.begin(),sets.end(),setsortx);
        
        debug1<<"single-task assigned"<<sets[0].bestp<<"   "<<sets[0].setTasks[0]
                <<"   "<<sets[0].lastChance<<"   "<<sets[0].util<<endl;
        assignSet(0,sets[0].bestp); 
        
        
        //assignTask(sets[0].setTasks[0],sets[0].bestp);
        //sets.erase(sets.begin());
    }
    while(!sets.empty());
    
    
    if( (assignedTaskCount == node_counter && schedAnalysis2()))
    {
    
            stage2v2();
            
            optimalSys.bprocessors=processors;
            optimalSys.bresources=resources;
            optimalSys.bsets=sets;
            optimalSys.btasks=tasks;
            optimalSysCost=solutionCost();
           
            
            bnb();
            
            processors=optimalSys.bprocessors;
            resources=optimalSys.bresources;
            sets=optimalSys.bsets;
            tasks=optimalSys.btasks;
            assignedTaskCount=0;
            for(int i=0;i<tasks.size();i++)
                if(tasks[i].procAlloc != -1)
                    assignedTaskCount++;
            debug1<<"--------------------------------------------------------"<<endl;
            
        }
    
 
    else
        {
            deAllocateTasks();
                BFD();
        }
    
    
}





#define	HEURISTIC_H



#endif	/* HEURISTIC_H */

