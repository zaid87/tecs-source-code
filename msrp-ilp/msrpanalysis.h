/* 
 * File:   msrpanalysis.h
 * Author: Zaid
 *
 * Created on October 8, 2013, 12:49 AM
 */

#ifndef MSRPANALYSIS_H

#include "generation2.h"


//#include "generation-ex.h"


//#include "generation2.h"

//bool schedAnalysis3();
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
//    cout<<"in"<<endl;

    float diff=0.0;
    float ri1,ri2; 
    int procID;
    int h;
    float sums1[procCnt]={0.0};
    bool sched=true;
   
    calcBlocking2();
    
    for(int i=0;i<tasks.size();i++)
        debug9<<i<<"   "<<tasks[i].b.lmsrp<<"  "<<tasks[i].b.rmsrp<<endl;
    
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
                {
                        ri2+=(tasks[h].c+tasks[h].b.rmsrp)*ceil(ri1/float(tasks[h].t));
                        //if(i==3)
                        //cout<<"*"<<h<<"--"<<(tasks[h].c+tasks[h].b.rmsrp)<<" "<<ceil(ri1/float(tasks[h].t))<<endl;
                }
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
    
   
        int tot=0;
    for(int i=0;i<tasks.size();i++)
        if(tasks[i].procAlloc!=-1)
            tot++;
    
//    if(sched)
//   cout<<"11"<<endl;
//          else
//              cout<<"22"<<endl;
    

    return sched;
    
}

//bool schedAnalysisProc(int procToCheck)
//{
//
//    float diff=0.0;
//    float ri1,ri2; 
//    int procID;
//    int h;
//    float sums1[procCnt]={0.0};
//    bool sched=true;
//   
//   calcBlocking2();
//       
//        
//    for(int i=0;i<tasks.size();i++)
//    if(tasks[i].procAlloc == procToCheck)
//    {
//                ri2=tasks[i].c+tasks[i].b.lmsrp+tasks[i].b.rmsrp;
//        procID=tasks[i].procAlloc;
//                      
//        do
//        {
//            ri1=ri2;
//            ri2=tasks[i].c+tasks[i].b.lmsrp+tasks[i].b.rmsrp;
//            
//            for(int j=0;j<processors[procID].tasks.size();j++)
//            {
//                h=processors[procID].tasks[j];
//                if(tasks[i].priority > tasks[h].priority)
//                        ri2+=(tasks[h].c+tasks[h].b.rmsrp)*ceil(ri1/float(tasks[h].t));
//            }
//            if(ri2>tasks[i].t)
//            {
//                sched=false;
//                diff=max(diff,ri2-tasks[i].t);
////				if(flag==true)
////				cout<<"The unsched is"<<i<<" on processor "<<tasks[i].procAlloc<<endl;
//                break;
//            }   
//        }
//        while(ri2>ri1);
//        tasks[i].b.totRespTime=ri2;
//       
//    } 
//
//    if(sched==false)
//        maxRespTimeInc=diff;
//    
//    return sched;          
//}


bool schedAnalysis4()
{

bool contin=false;
float diff=0.0;
int h;
bool sched=true;    
int taskid;
int procID;
double objval;
vector <int> njobs;

for(int i=0;i<tasks.size();i++)
{
    tasks[i].ilpB=0.0;
    tasks[i].ilpR=0;
    //tasks[i].priority=-1;
}

resMode=1;
      
    IloEnv env;
    try {
    IloModel       mod(env);

    typedef IloArray<IloNumVarArray> NumVarMatrix;
    typedef IloArray<NumVarMatrix>   NumVar3Matrix;
    typedef IloArray<NumVar3Matrix>   NumVar4Matrix;

    typedef IloArray<IloIntVarArray> IntVarMatrix;
///////////////////////////////////////////////////////////////////////////////ST
    
    float tempm=0;
    for(int x=0;x<tasks.size();x++)
        for(int q=0;q<tasks[x].critSec.size();q++)
            tempm=max(tempm,tasks[x].critSec[q].length);
    const float M=tempm*tasks.size();
    const float M2=10000000;
    
    NumVarMatrix AA(env,tasks.size());
    NumVarMatrix pi(env,tasks.size());
    NumVarMatrix V(env,tasks.size());
    NumVarMatrix X(env,tasks.size());
    
    IloNumVarArray R(env,tasks.size());
    IloNumVarArray B(env,tasks.size());
    IloNumVarArray S(env,tasks.size());
    IloNumVarArray I(env,tasks.size());
    
    NumVarMatrix H(env,tasks.size());
    
    NumVarMatrix SS(env,tasks.size());   //must be the same size as S
    NumVar3Matrix SSS(env,tasks.size()); //must be the same size as S
    NumVar4Matrix SSSS(env,tasks.size());
    
    NumVarMatrix BB(env,tasks.size());
    NumVar3Matrix BBB(env,tasks.size());
    NumVar4Matrix BBBB(env,tasks.size());
    
      NumVar4Matrix Y(env,tasks.size());
    
    NumVar3Matrix FFF(env,tasks.size());
    NumVar4Matrix FFFF(env,tasks.size());
    NumVarMatrix E(env,tasks.size());
    NumVar3Matrix PPP(env,tasks.size());
    NumVar4Matrix PPPP1(env,tasks.size());
    NumVar4Matrix PPPP2(env,tasks.size());
    
    NumVarMatrix C1(env,tasks.size()); 
    
    IloNumVarArray W(env,resources.size());
    IloNumVarArray C(env,resources.size());
    
    //IloNumVarArray LG(env,resources.size());
    //IloNumVarArray FN(env,resources.size());

   
    for(int i=0; i<tasks.size();i++) 
    {
        R[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
        B[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
        S[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
        I[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
        
        AA[i] = IloNumVarArray(env, processors.size());
        pi[i] = IloNumVarArray(env, tasks.size());
        V[i] =  IloNumVarArray(env, tasks.size());
        X[i] =  IloNumVarArray(env, tasks.size());
        
        H[i] =  IloNumVarArray(env, tasks.size());
        
        SS[i]=  IloNumVarArray(env, processors.size());
        SSS[i]=  NumVarMatrix(env, processors.size());    //must be same size as SS
        SSSS[i]=  NumVar3Matrix(env, processors.size());
        for(int k=0;k<processors.size();k++)
        {
            SSS[i][k]=IloNumVarArray(env, resources.size());
            SSSS[i][k]=NumVarMatrix(env, resources.size());
           
        }
        
        BB[i]=  IloNumVarArray(env, resources.size());
        BBB[i]=  NumVarMatrix(env, resources.size());
        BBBB[i]=  NumVar3Matrix(env, resources.size());
        for(int k=0;k<resources.size();k++)
        {
            BBB[i][k]=IloNumVarArray(env, processors.size());
            BBBB[i][k]=NumVarMatrix(env, processors.size());
        }
        
        Y[i]=  NumVar3Matrix(env, processors.size());
        E[i]=  IloNumVarArray(env, tasks.size());
        PPP[i]=  NumVarMatrix(env, processors.size());
        PPPP1[i]=  NumVar3Matrix(env, processors.size());
        PPPP2[i]=  NumVar3Matrix(env, processors.size());
        C1[i]=IloNumVarArray(env, tasks.size());
        FFF[i]=  NumVarMatrix(env, resources.size());
        FFFF[i]=  NumVar3Matrix(env, resources.size()); 

        for(int k=0; k<processors.size(); k++) 
        {
            AA[i][k]= IloNumVar(env, 0.0, 1.0, ILOINT);
            SS[i][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            Y[i][k]=NumVarMatrix(env, resources.size());
            PPP[i][k]=IloNumVarArray(env, resources.size());
            PPPP1[i][k]= NumVarMatrix(env, resources.size());
            PPPP2[i][k]= NumVarMatrix(env, resources.size());
            
            for(int q=0;q<resources.size();q++)
            {
                PPP[i][k][q]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                PPPP1[i][k][q]=IloNumVarArray(env, tasks.size());
                PPPP2[i][k][q]=IloNumVarArray(env, tasks.size());
                Y[i][k][q]=IloNumVarArray(env, tasks.size());
                SSS[i][k][q]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                SSSS[i][k][q]=IloNumVarArray(env, tasks.size());
                for(int j=0;j<tasks.size();j++)
                {
                    SSSS[i][k][q][j]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                    Y[i][k][q][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    PPPP1[i][k][q][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    PPPP2[i][k][q][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                }
            }
            
            //PP[i][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
            
            
        }
        
        for(int k=0; k<tasks.size(); k++) 
        {
            pi[i][k]= IloNumVar(env, 0.0, 1.0, ILOINT);
            V[i][k]= IloNumVar(env, 0.0, 1.0, ILOINT); 
            X[i][k]= IloNumVar(env, 0.0, 1.0, ILOINT);
            H[i][k]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
            E[i][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
           
            C1[i][k]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
        }
        
        for(int q=0;q<resources.size();q++)
        {
					
            BB[i][q]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            for(int k=0; k<processors.size(); k++)
            {
                BBB[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                BBBB[i][q][k]=IloNumVarArray(env, tasks.size());
                for(int j=0;j<tasks.size();j++)
                    BBBB[i][q][k][j]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            }
            
            W[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
            C[q]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
            FFF[i][q]=  IloNumVarArray(env, tasks.size());
            FFFF[i][q]=  NumVarMatrix(env, tasks.size());
            
            
        }
        
        for(int q=0;q<resources.size();q++)
        {
            for(int k=0;k<tasks.size();k++)
            {
                FFF[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                FFFF[i][q][k]=IloNumVarArray(env, tasks.size());
                for(int j=0;j<tasks.size();j++)
                        FFFF[i][q][k][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
            }
        }
  
    }
    
///////////////////////////////////////////////////////////////////////////////CONTRAINTS
    
    for(int i=0;i<tasks.size();i++)
    {
        IloExpr expr(env);
        for (int k = 0; k < processors.size(); k++)
            expr += AA[i][k];
        mod.add(expr == 1);
        expr.end();  
    }
     
         //C2:
    for(int i=0;i<tasks.size();i++)
    {
        IloExpr expr(env);
        for (int k=0;k<tasks.size(); k++)
        {
            expr += pi[i][k];
        }
       
        mod.add(expr == 1);
        expr.end();  
    }
        
    
   //Cnew:
    for(int i=0;i<tasks.size();i++)
    {
        IloExpr expr(env);
        for (int k=0;k<tasks.size();k++)
        {
            
            expr += pi[k][i];
        }
       
        mod.add(expr == 1);
        expr.end();  
    }
   
           
    for(int i=0;i<tasks.size();i++)
        for(int j=0;j<tasks.size();j++)
        {
            if(tasks[i].priority==j)
            {
                mod.add(pi[i][j] == 1);
            }
            else
            {
                mod.add(pi[i][j] == 0);
            }
        }
             
         
    //C3:
    for(int i=0;i<tasks.size();i++)
    {
        mod.add(V[i][i] ==1 );
        for(int x=0;x<tasks.size();x++)
            if(x!=i)
                for(int k=0;k<processors.size();k++)
                {
                    mod.add(V[x][i] >= 1-(2-AA[i][k]-AA[x][k]) );
                    
                }     
    }
    
     //C3new
    for(int i=0;i<tasks.size();i++)
        for(int x=0;x<tasks.size();x++)
            if(x!=i)
    for(int p1=0;p1<processors.size();p1++)
    for(int p2=p1+1;p2<processors.size();p2++)    
    {
        mod.add(IloIfThen(env, AA[i][p1]==1 && AA[x][p2]==1, V[i][x]==0));
        mod.add(IloIfThen(env, AA[i][p1]==1 && AA[x][p2]==1, V[x][i]==0));
 
    }
 
         
             for(int i=0;i<tasks.size();i++)
             {
                mod.add(X[i][i] ==1);
                 for(int x=0;x<tasks.size();x++)
                    if(x!=i)
                    for(int p=0;p<tasks.size();p++)//-1
                    {

                        IloExpr expr(env);
                        for (int j = p+1; j < tasks.size(); j++)
                            expr += pi[x][j];
                        expr+=(1-pi[i][p]);
                        mod.add(X[i][x] <= expr);
                        expr.end();
                    }  
             }
                             

    //C5:
     for(int i=0;i<tasks.size();i++)
        for(int x=0;x<tasks.size();x++)
            if(x!=i)
                   mod.add(X[i][x] + X[x][i]==1 );
                    
   //c5new
        for(int i=0;i<tasks.size();i++)
        for(int x=0;x<tasks.size();x++)
            for(int n=0;n<tasks.size();n++)
            if(x!=i && x!=n && i!=n)
                   mod.add(IloIfThen(env, X[i][x]==1 && X[x][n]==1, X[i][n]==1));
        
    //C6:
    for(int i=0;i<tasks.size();i++)        
        mod.add(R[i] <=tasks[i].t);
            
    //C7:
    for(int i=0;i<tasks.size();i++)
        mod.add(R[i] == tasks[i].c + B[i]+I[i]+S[i]);
            
            
   //C8:
     for(int i=0;i<tasks.size();i++)
     {
        IloExpr expr(env);
        for(int x=0;x<tasks.size();x++)
        if(x!=i)
            {
                expr+=H[i][x]*tasks[x].c;
            } 
        mod.add(I[i] == expr );
        expr.end(); 

     }

    //C9:
    for(int i=0;i<tasks.size();i++)
    {
        mod.add(H[i][i]==1);
        for(int x=0;x<tasks.size();x++)
            if(x!=i)
            {
                mod.add(H[i][x]>=0);
                int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
                mod.add(H[i][x]<=ceilval);
            }
    }

    //C10:
    for(int i=0;i<tasks.size();i++)
        for(int x=0;x<tasks.size();x++)
            if(x!=i)
            {
                mod.add(H[i][x]>=0);
                int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
                mod.add(H[i][x]>= (R[i]/float(tasks[x].t))-ceilval*(1-V[i][x])-ceilval*X[i][x]);
            }
    
    //C11:
    for(int i=0;i<tasks.size();i++)
    {
        IloExpr expr(env);
        for (int k = 0; k < processors.size(); k++)
            expr += SS[i][k];
        mod.add(S[i] == expr);
        expr.end();  
    }
    
    //C12
    for(int i=0;i<tasks.size();i++)
    for(int k=0;k<processors.size();k++)    
    {
        IloExpr expr(env);
        for(int q=0;q<resources.size();q++)
            expr += SSS[i][k][q];
        mod.add(SS[i][k] == expr);
        expr.end();  
    }
    
    //C13
        //C13
    for(int i=0;i<tasks.size();i++)
         for(int k=0;k<processors.size();k++)
              for(int q=1;q<resources.size();q+=2)
              {    
                  
                vector <int> Qtasks;
                Qtasks.clear();
                for(int j=0;j<tasks.size();j++)
                    for(int n=0;n<tasks[j].resids.size();n++)
                        if(tasks[j].resids[n]==resources[q].resID)
                            Qtasks.push_back(j);
                    
                  
                for(int a=0;a<Qtasks.size();a++)
                   if(Qtasks[a]!=i)
                   {


                        int x=Qtasks[a];
                        int wr=resources[q].writer;

                        IloExpr expr(env);
//                            for(int rn=0;rn<tasks[j].critSec.size();rn++)
//                                if(tasks[j].critSec[rn].resID==resources[q].resID)
                           //expr +=H[i][j];
                         expr+=Y[i][k][q][x];

                        float lxq=0.0;
                        for(int rn=0;rn<tasks[x].critSec.size();rn++)
                           if(tasks[x].critSec[rn].resID==resources[q].resID)
                                   lxq=tasks[x].critSec[rn].length;


                           expr*=lxq;
                           expr=expr-M*(1-AA[x][k])-M*AA[i][k]-abs(wr-x)*M*V[wr][x];
                           mod.add(IloIfThen(env, W[q]==1, SSSS[i][k][q][x]>=0) && IloIfThen(env, W[q]!=1, SSSS[i][k][q][x]>=expr));

                        expr.end();

                    } 
              }
     
     for(int i=0;i<tasks.size();i++)
    for(int k=0;k<processors.size();k++)  
    for(int q=1;q<resources.size();q++)
    {
        IloExpr expr(env);
        for(int j=0;j<tasks.size();j++)
            expr += SSSS[i][k][q][j];
        mod.add(SSS[i][k][q] == expr);
        expr.end();  
    }
    

     
    //C14
    for(int i=0;i<tasks.size();i++)
    {
        for (int q = 0; q < resources.size(); q++)
            mod.add(B[i] >= BB[i][q]);
          
    }
    
    //C15
    for(int i=0;i<tasks.size();i++)
         for (int q=0; q<resources.size(); q++)
        {
            IloExpr expr(env);
            for (int k = 0; k < processors.size(); k++)
                expr += BBB[i][q][k];
            mod.add(BB[i][q] == expr);
            expr.end();  
        }
     
    //C18
     for(int i=0;i<tasks.size();i++)     
            for(int q=1;q<resources.size();q+=2)  
            {
                vector <int> Qtasks;
                Qtasks.clear();
                for(int j=0;j<tasks.size();j++)
                    for(int k=0;k<tasks[j].resids.size();k++)
                        if(tasks[j].resids[k]==resources[q].resID)
                            Qtasks.push_back(j);
                
                
                for(int k=0;k<processors.size();k++)
                    for(int x=0;x<tasks.size();x++)
                        if(x!=i)
                 for(int ctr2=0;ctr2<Qtasks.size();ctr2++)
                 if(Qtasks[ctr2]!=x)   
                {
                    int h=Qtasks[ctr2];            

                    int wr=resources[q].writer;
                    float lxq=0.0;
                    for(int rn=0;rn<tasks[x].critSec.size();rn++)
                        if(tasks[x].critSec[rn].resID==resources[q].resID)
                            if(wr==x)
                                lxq=tasks[x].critSec[rn+1].length;
                            else
                                lxq=tasks[x].critSec[rn].length;

                         IloExpr expr(env);
                            //expr +=lxq -lxq*(1-AA[x][k]) -lxq*(1-Z[i][q+1]) -lxq*(1-AA[i][k]) -lxq*X[x][i] -lxq*(2-V[x][i]-V[i][h])-lxq*abs(i-h)*X[i][h];
                            //expr +=lxq -lxq*(1-AA[x][k]) -lxq*(1-AA[i][k]) -lxq*X[x][i] -lxq*(2-V[x][i]-V[i][h])-lxq*abs(i-h)*X[i][h]-lxq*abs(wr-i)*(1-V[i][wr]);
                            expr +=lxq -lxq*(1-AA[x][k]) -lxq*X[x][i] -lxq*(2-V[x][i]-V[i][h])-lxq*abs(i-h)*X[i][h]-lxq*(1-V[i][wr]);
                            
                            mod.add(IloIfThen(env, expr==lxq, BBBB[i][q+1][k][x]>=lxq) && IloIfThen(env, expr!=lxq, BBBB[i][q+1][k][x]>=0));
                         expr.end();  
                     
                
                }
            }
    
    cout<<"HI"<<endl;
    //C19
     for(int i=0;i<tasks.size();i++)
        for(int q=1;q<resources.size();q+=2)
        {
             vector <int> Qtasks;
            Qtasks.clear();
            for(int j=0;j<tasks.size();j++)
                for(int k=0;k<tasks[j].resids.size();k++)
                    if(tasks[j].resids[k]==resources[q].resID)
                        Qtasks.push_back(j);
                
                for(int k=0;k<processors.size();k++)
                for(int jj=0;jj<Qtasks.size();jj++)
                    if(Qtasks[jj]!=i)
              {
                 
                 int x=Qtasks[jj];      
                 for(int ctr2=0;ctr2<Qtasks.size();ctr2++)
                 if(Qtasks[ctr2]!=x && Qtasks[ctr2]!=i)
                 {
                        int h=Qtasks[ctr2]; 
                    int wr=resources[q].writer;
                    float lxq=0.0;
                    for(int rn=0;rn<tasks[x].critSec.size();rn++)
                        if(tasks[x].critSec[rn].resID==resources[q].resID)
                                lxq=tasks[x].critSec[rn].length;
                            
                        IloExpr expr1(env);
                        IloExpr expr2(env);
                        expr1 +=lxq-M*(1-AA[x][k])-abs(wr-x)*M*V[x][wr]-abs(wr-h)*M*V[h][wr]-M*(1-V[i][x])-M*(1-X[i][x])-M*V[i][h];
                        //mod.add(IloIfThen(env, expr1==lxq, BBBB[i][q][k][x]>=lxq) && IloIfThen(env, expr1!=lxq, BBBB[i][q][k][x]>=0));
                        
                     
                        
                         
                         expr2 +=lxq-M*(1-AA[x][k])-abs(wr-x)*M*V[x][wr]-abs(wr-h)*M*V[h][wr]-M*(1-V[i][h])-M*(1-X[i][h])-M*V[i][x];
                         //mod.add(IloIfThen(env, expr2==lxq, BBBB[i][q][k][x]>=lxq) && IloIfThen(env, expr2!=lxq, BBBB[i][q][k][x]>=0));
                         mod.add(IloIfThen(env, W[q]!=0 || (expr2!=lxq && expr1!=lxq ), BBBB[i][q][k][x]>=0) && IloIfThen(env, W[q]==0 && (expr2==lxq || expr1==lxq), BBBB[i][q][k][x]>=lxq));
                        
                         expr1.end();
                         expr2.end();  
                }
            }
        }
     
      for(int i=0;i<tasks.size();i++)
        for(int q=1;q<resources.size();q++)
                for(int k=0;k<processors.size();k++)
                    mod.add(BBB[i][q][k] == IloMax(BBBB[i][q][k]) )  ;
     
       
    for (int q=1; q<resources.size(); q+=2)
    {
        int nr=0;
        vector <int> readers;
        int writer=resources[q].writer;
        IloExpr expr(env); 
        for(int i=0;i<tasks.size();i++)
            if(i!=writer)
                for(int j=0;j<tasks[i].critSec.size();j++)
                    if(tasks[i].critSec[j].resID==resources[q].resID)
                    {
                        expr+=V[i][writer];
                        nr++;
                    }
        mod.add(W[q] <= nr - expr);
        expr.end();    
        mod.add(W[q+1] == 0);
    }
    
    
    for (int q=1; q<resources.size(); q+=2)
    {
        int nr=0;
        int sizeByTask=0;
        int writer=resources[q].writer;
        for(int i=0;i<tasks.size();i++)
            if(i!=writer)
            for(int j=0;j<tasks[i].critSec.size();j++)
                    if(tasks[i].critSec[j].resID==resources[q].resID)    
            {
                
                sizeByTask=tasks[writer].succSize*(1+max(2,int(1+ceil(float(tasks[i].t)/float(tasks[writer].t)))));
                mod.add(C[q]>=sizeByTask-M2*V[writer][i]-M2*(1-W[q]));
               
            }
        mod.add(C[q+1]==0);
    }
    
    
    //copt1
    
   
     for(int i=0;i<tasks.size();i++)
        for(int q=1;q<resources.size();q+=2)
        {
            vector <int> Qtasks; 
            for(int j=0;j<tasks.size();j++)
                for(int k=0;k<tasks[j].resids.size();k++)
                    if(tasks[j].resids[k]==resources[q].resID)
                        Qtasks.push_back(j);
                
            for(int a=0;a<Qtasks.size();a++)
            {
                int x=Qtasks[a];
                //if(x!=i)
                    //for(int b=0;b<Qtasks.size();b++)
                    for(int j=0;j<tasks.size();j++)   
                    {
                            int wr=resources[q].writer;
                           
                        
                            float lxj=0.0;
                            float lxq=0.0;
                            
                            for(int rn=0;rn<tasks[x].critSec.size();rn++)
                                if(tasks[x].critSec[rn].resID==resources[q].resID)
                                    lxq=tasks[x].critSec[rn].length;
                            
                            for(int rn=0;rn<tasks[j].critSec.size();rn++)
                                if(tasks[j].critSec[rn].resID==resources[q].resID)
                                   lxj=tasks[j].critSec[rn].length;
                            
                            if(lxq==0 || lxj==0 || lxq > lxj || x==j || i==j || x==i) 
                                mod.add(FFFF[i][q][x][j]==0);
                            else
                            {
                                 mod.add(IloIfThen(env,abs(wr-x)*V[wr][x]==0 && abs(wr-j)*V[wr][j]==0 && V[j][x]==1,FFFF[i][q][x][j] == E[i][j]) 
                                 && IloIfThen(env, abs(wr-x)*V[wr][x]!=0 || abs(wr-j)*V[wr][j]!=0 || V[j][x]!=1,FFFF[i][q][x][j] == 0));
                                
                               
                            }
                           
                        
                    }
            }
        }
    

    
    
   
    for(int i=0;i<tasks.size();i++)
    for(int q=1;q<resources.size();q+=2)
    {
        
        
                
        for(int x=0;x<tasks.size();x++)
            {
                
                IloExpr expr(env);
               
                for(int j=0;j<tasks.size();j++)
                {
                   
                    expr += FFFF[i][q][x][j];
                }
                mod.add(FFF[i][q][x] == expr);
                expr.end();  
                
            }
   
    }
     
  //copt3
    for(int i=0;i<tasks.size();i++)
        for(int k=0;k<processors.size();k++)
        for(int q=1;q<resources.size();q+=2)
            
        {
           int wr=resources[q].writer; 
          
            vector <int> Qtasks;
            Qtasks.clear();
            for(int j=0;j<tasks.size();j++)
                for(int n=0;n<tasks[j].resids.size();n++)
                    if(tasks[j].resids[n]==resources[q].resID)
                        Qtasks.push_back(j);
            for(int a=0;a<Qtasks.size();a++)
            {
                int x=Qtasks[a];
                mod.add(IloIfThen(env,abs(wr-x)*V[wr][x]==0 && AA[x][k]==1,PPPP1[i][k][q][x]==E[i][x]) && IloIfThen(env, abs(wr-x)*V[wr][x]!=0 || AA[x][k]!=1 , PPPP1[i][k][q][x]==0));
                mod.add(IloIfThen(env,abs(wr-x)*V[wr][x]==0,PPPP2[i][k][q][x]==H[i][x]) && IloIfThen(env, abs(wr-x)*V[wr][x]!=0, PPPP2[i][k][q][x]==0));
              
            }
                          
        }
    
        for(int i=0;i<tasks.size();i++)
            for(int k=0;k<processors.size();k++)
        for(int q=1;q<resources.size();q+=2)
        {
           int wr=resources[q].writer; 
           IloExpr expr1(env);
           IloExpr expr2(env);  
                
            vector <int> Qtasks;
            Qtasks.clear();
            for(int j=0;j<tasks.size();j++)
                for(int n=0;n<tasks[j].resids.size();n++)
                    if(tasks[j].resids[n]==resources[q].resID)
                        Qtasks.push_back(j);
            for(int a=0;a<Qtasks.size();a++)
            {
                int x=Qtasks[a];
                expr1+=PPPP1[i][k][q][x];
                expr2+=PPPP2[i][k][q][x];
            }
            
            //mod.add(PP[i][q]==expr1);
            
            mod.add(PPP[i][k][q] == IloMin(expr1,expr2));
            expr1.end();  
            expr2.end(); 
                          
        }
    
   

    
    
    
    for(int i=0;i<tasks.size();i++)
    {
        for(int x=0;x<tasks.size();x++)
            if(x!=i)
            {
                //int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
             int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t))+1; 
                mod.add(E[i][x]<= ceilval);
                mod.add(IloIfThen(env, V[i][x]==1, E[i][x]==0) && IloIfThen(env, V[i][x]!=1, E[i][x] >= 1+(R[i]/float(tasks[x].t))));
                
            }
    }
    
    for(int i=0;i<tasks.size();i++)
        for(int k=0;k<processors.size();k++)
        for(int q=1;q<resources.size();q+=2)
        {
            vector <int> Qtasks;
            Qtasks.clear();
            for(int j=0;j<tasks.size();j++)
                for(int k=0;k<tasks[j].resids.size();k++)
                    if(tasks[j].resids[k]==resources[q].resID)
                        Qtasks.push_back(j);
            
                for(int j=0;j<Qtasks.size();j++)
                    {
                    int x=Qtasks[j];
                        IloExpr expr1(env);
                        expr1+=PPP[i][k][q]-FFF[i][q][x];

                        mod.add(Y[i][k][q][x]>=IloMin(E[i][x],expr1));
                        expr1.end();
                       
                    }
        }
    
 
     

//    IloExpr exprq(env);
//    for (int q=1; q<resources.size(); q+=2)
//    {
//        exprq+=W[q];
//    }
//    mod.add(exprq<=2);
//
//    
//    IloExpr expr(env);   
//    for(int i=0;i<tasks.size();i++)
//    {
//        expr+=R[i];
//    }
    
    IloExpr expr(env); 
    for(int q=1;q<resources.size();q++)
    {
        expr += C[q];
    }
    
    mod.add(IloMinimize(env,expr));
    expr.end();        
   
////////////////////////////////////////////////////////////////////////////////////////////////////end of constraints            

          IloCplex cplex(mod);
          cplex.exportModel("m0.lp");
          cplex.setOut(env.getNullStream());
         // cplex.setParam(IloCplex::NodeSel, CPX_NODESEL_BESTEST);
         //  cplex.setParam(IloCplex::MemoryEmphasis, CPX_ON);
          
          bool bval;
          bval=cplex.solve();
          if(bval && cplex.getStatus()==IloAlgorithm::Optimal)
          {    
          
         
            cout<< "solution status = " << cplex.getStatus() << endl;
            
           vector <double> x1;
           x1.resize(tasks.size());
           for(int k=0;k<tasks.size();k++)
           {
                   x1[k] = cplex.getValue(R[k]);
                   tasks[k].ilpR=(float)x1[k];
           }



           int var1=0;
           for(int i=0;i<tasks.size();i++)
               for(int j=0;j<tasks.size();j++)
               {
                   var1= cplex.getValue(pi[i][j]); 
                   
                   if(var1==1)
                   {
                       tasks[i].priority=j;
                   }
               }


            

           for(int k=0;k<tasks.size();k++)
               for(int p=0;p<processors.size();p++)
               {
                       if(cplex.getValue(AA[k][p])>0.99)
                       {
                           assignTask(k,p);
                           
                       }
               }
           
           
           for(int q=1;q<resources.size();q++)
           {
               int type=cplex.getValue(W[q]);
               int rcost=cplex.getValue(C[q]);
               if(type==1)
                    resources[q].protocol=2;
           }
           
           
            debug9<<"\t"<<"(R[k])"<<"\t"<<"I[k])"
                       <<"\t"<<"S[k])"<<"\t"<<"(B[k])"<<endl;
           for(int k=0;k<tasks.size();k++)
               debug9<<k<<"\t"<<cplex.getValue(R[k])<<"\t"<<cplex.getValue(I[k])
                       <<"\t"<<cplex.getValue(S[k])<<"\t"<<cplex.getValue(B[k])<<endl;
            debug9<<"--------------------------------------------"<<endl;
            
            
            //////////////////////////////////////////////////////////////////////////////////////LOG
             debug9<<"Resources \t Protocol"<<endl;
           for(int q=1;q<resources.size();q++)
           {
               int type=cplex.getValue(W[q]);
               int rcost=cplex.getValue(C[q]);
               debug9<<q<<"\t"<<type<<"\t"<<rcost<<endl;
                   
           }
            cout<<"i-x"<<"\t"<<"V[i][x]"<<"\t"<<"X[i][x]"<<"\t"<<"H[i][x]"
                                   << endl;
           
           for(int i=0;i<tasks.size();i++)
                       for(int x=0;x<tasks.size();x++)
                           
                       {
                           
                           cout<<i<<"-"<<x<<"\t"<<cplex.getValue(V[i][x]) 
                                   <<"\t"<<cplex.getValue(X[i][x])<<"\t"<<cplex.getValue(H[i][x])
                                   << endl;
                      }
            
               for(int i=0;i<tasks.size();i++)
                    for(int k=0;k<processors.size();k++)
                 for(int q=1;q<resources.size();q++)
                     for(int x=0;x<tasks.size();x++)
               {
                        if( cplex.getValue(SSSS[i][k][q][x])>0)
                    debug10<<"SSSS  "<<i<<"-"<<k<<"-"<<q<<"-"<<x<<"\t"<<cplex.getValue(SSSS[i][k][q][x])<<endl;
                  
               }
//            
            for(int i=0;i<tasks.size();i++)
                    for(int k=0;k<processors.size();k++)
                 for(int q=1;q<resources.size();q++)
                   
               {
                    debug10<<"SSS  "<<i<<"-"<<k<<"-"<<q<<"\t"<<cplex.getValue(SSS[i][k][q])<<endl;
                  
               }
            
//            for(int i=0;i<tasks.size();i++)
//                for(int q=1;q<resources.size();q++)
//                for(int k=0;k<processors.size();k++)
//                    for(int x=0;x<tasks.size();x++)
//                    {
////                        
//                        if(cplex.getValue(BBBB[i][q][k][x]) > 0.0)
//                    debug10<<"BBBB "<<i<<"-"<<q<<"-"<<k<<"-"<<x<<"\t"<<cplex.getValue(BBBB[i][q][k][x])
//                                <<endl;
//                    }
//                
//                
//                for(int i=0;i<tasks.size();i++)
//                for(int q=1;q<resources.size();q++)
//                for(int k=0;k<processors.size();k++)
//                    debug10<<"BBB "<<i<<"-"<<q<<"-"<<k<<"\t"<<cplex.getValue(BBB[i][q][k])
//                                <<endl;
//                for(int i=0;i<tasks.size();i++)
//                {
//                    for (int q = 1; q < resources.size(); q++)
//                        debug10<<"BB "<<i<<"-"<<q<<"\t"<<cplex.getValue(BB[i][q])<<endl;
//
//                }
            
//             for(int i=0;i<tasks.size();i++)
//    {
//        for(int x=0;x<tasks.size();x++)
//            if(x!=i)
//            {
//                //int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
//             int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t))+1; 
//            debug10<<i<<" "<<x<<"   "<<ceilval<<"   "<<cplex.getValue(E[i][x])<<endl;
//              
//                //mod.add(R[i]/float(tasks[x].t)<=C1[i][x]);
//            }
//    }
            
            
            
                for(int i=5;i<6;i++)
                for(int q=1;q<2;q+=2)
                for(int k=0;k<2;k++)
                    for(int x=2;x<5;x++)
                        if(x!=1)
                        for(int j=0;j<tasks.size();j++)
                    {
//                        
                        //if(cplex.getValue(Y[i][q][0][x]) > 0.0)
                    debug10
                            <<"Y "<<i<<"-"<<k<<"-"<<q<<"-"<<x<<"-"<<j<<"\t"
                            <<cplex.getValue(Y[i][k][q][x])<<"\t"
                            <<cplex.getValue(PPP[i][k][q])
                            <<"\t"<<cplex.getValue(E[i][x])
                            <<"\t"<<cplex.getValue(FFF[i][q][x])
                            <<"\t"<<cplex.getValue(FFFF[i][q][x][j])

                             
                           // <<"\t"<<cplex.getValue(FFFF[i][q][x][j])
                            //<<"\t"<<"\t"<<cplex.getValue(C1[i][j])
                                <<endl;
                    }
            
           
            
            
            //////////////////////////////////////////////////////////////////////////////////////
             
              return true;

          }
          else
          {
              cout<<"NS"<<endl;
               cout<< "solution status = " << cplex.getStatus() << endl;
              
              return false;
          }
           
       }
       catch (IloException& ex) {
          cerr << "Error: " << ex << endl;
       }
       catch (...) {
          cerr << "Error" << endl;
       }

       env.end();
    return sched;          
}



#define	MSRPANALYSIS_H



#endif	/* MSRPANALYSIS_H */