/* 
 * File:   assignresources.h
 * Author: Zaid
 *
 * Created on October 3, 2013, 12:05 AM
 */

#ifndef ASSIGNRESOURCES_H

#include "generation2.h"



bool stage2sort(set s1, set s2)
{
      if (s1.maxMemory >  s2.maxMemory)
        return true;

	if (s1.maxMemory <  s2.maxMemory)
        return false;

		if( s1.csl  >   s2.csl   )
			return true;
		if( s1.csl  <   s2.csl   )
			return false;
    
    return false;
}


void stage2v2()
{
    
   

    vector <int> resToChange;
    set s0;
    vector <set> sets2;
             
        for(int i=0;i<resources.size();i++)
        if(resources[i].protocol==1 && resources[i].active==true)
        {
            resources[i].protocol=2;
            setWfPrio(i);
        }
        
    for(int i=0;i<resources.size();i++)
        if(resources[i].protocol==2 && resources[i].active==true)
            resToChange.push_back(i);
    
            
    for(int i=0;i<resToChange.size();i++)
    {
        
        //s0.writerID=resources[resToChange[i]].tasks[0];
        s0.type=resToChange[i];
        s0.setTasks.clear();
        for(int j=0;j<resources[resToChange[i]].tasks.size();j++)
            s0.setTasks.push_back(resources[resToChange[i]].tasks[j]);
        s0.maxMemory=calcMaxMem(s0);
        s0.csl=csl(resToChange[i]);
        sets2.push_back(s0);
    }
         

    sort(sets2.begin(),sets2.end(),stage2sort);
    
    
    for(int i=0;i<sets2.size();i++)
    {
        resources[sets2[i].type].protocol=1;
        setMsrpPrio(sets2[i].type);
        if(schedAnalysis2()==false)
        {
            resources[sets2[i].type].protocol=2;
            setWfPrio(sets2[i].type);
            
        }
    }
    
    
}


#define	ASSIGNRESOURCES_H



#endif	/* ASSIGNRESOURCES_H */

