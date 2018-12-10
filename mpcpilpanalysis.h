/* 
 * File:   ilpanaysis.h
 * Author: admin
 *
 * Created on April 11, 2016, 4:09 PM
 */

#ifndef ILPANAYSIS_H

#include "generation2.h"



int ilpSchedAnalysis()
{
    
    resMode=0;
    

IloEnv env;
    try {
        IloModel       mod(env);

        typedef IloArray<IloNumVarArray> NumVarMatrix;
        typedef IloArray<NumVarMatrix>   NumVar3Matrix;
        typedef IloArray<NumVar3Matrix>   NumVar4Matrix;
        typedef IloArray<NumVar4Matrix>   NumVar5Matrix;
        typedef IloArray<NumVar5Matrix>   NumVar6Matrix;

        typedef IloArray<IloIntVarArray> IntVarMatrix;
    
        float tempm=0;
        for(int x=0;x<tasks.size();x++)
            for(int q=0;q<tasks[x].critSec.size();q++)
                tempm=max(tempm,tasks[x].critSec[q].length);
        const float M=100*tempm*tasks.size();
        
        const float M2=100;


        NumVarMatrix A(env,tasks.size()); //assignment Aip=1 
        NumVarMatrix G(env,tasks.size()); //1 if they are on same core
        IloNumVarArray R(env,tasks.size());
        NumVarMatrix pi(env,tasks.size());
        NumVarMatrix OH(env,tasks.size());   //number of interferences
       
        
        IloNumVarArray S(env,tasks.size());
        NumVarMatrix SS(env,tasks.size());   //must be the same size as S
       
        NumVarMatrix SS2(env,tasks.size());   //must be the same size as S
        NumVar3Matrix SSS1(env,tasks.size()); //must be the same size as S
        NumVar4Matrix SSSS1(env,tasks.size()); //must be the same size as S

        NumVar4Matrix CR1(env,tasks.size());
        NumVar5Matrix CRR1(env,tasks.size());
        
        NumVar4Matrix CF(env,tasks.size());
        NumVar3Matrix CCF(env,tasks.size());

        IloNumVarArray B(env,tasks.size());
        NumVarMatrix BB(env,tasks.size());   //must be the same size as S
        NumVar3Matrix BBB(env,tasks.size());
        NumVar3Matrix NB(env,tasks.size());
        
        IloNumVarArray W(env,resources.size());
        IloNumVarArray C(env,resources.size());
        
        NumVar4Matrix GS(env,tasks.size());
        NumVar5Matrix GGS(env,tasks.size());
        NumVar6Matrix GGGS(env,tasks.size());
        
        IloNumVarArray LOC(env,resources.size());
        IloNumVarArray GLO(env,resources.size());
        
        NumVar3Matrix OB(env,tasks.size());
         NumVarMatrix OLL(env,tasks.size()); 
        
        //NumVarMatrix OHL(env,tasks.size());   //must be the same size as S
        
        
        for(int i=0; i<tasks.size();i++) 
        {
           
            
            A[i] = IloNumVarArray(env, processors.size());
            pi[i] = IloNumVarArray(env, tasks.size());
            G[i] =  IloNumVarArray(env, tasks.size());
            OH[i] =  IloNumVarArray(env, tasks.size());
           
            R[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            
            
            S[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            SS[i]=  IloNumVarArray(env, resources.size());
           
            SS2[i]=  IloNumVarArray(env, resources.size());
            SSS1[i]=  NumVarMatrix(env, resources.size());    //must be same size as SS
           
            SSSS1[i]=  NumVar3Matrix(env, resources.size());    //must be same size as SS
            
            
            B[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            BB[i]=  IloNumVarArray(env, tasks.size());
            NB[i]=  NumVarMatrix(env, tasks.size());
            OLL[i]=  IloNumVarArray(env, tasks.size());
            BBB[i]=  NumVarMatrix(env, tasks.size());
            
            
            GS[i]=NumVar3Matrix(env,resources.size());
            GGS[i]=NumVar4Matrix(env,resources.size());
            GGGS[i]=NumVar5Matrix(env,resources.size());
            
            OB[i]=  NumVarMatrix(env, resources.size()); 
            CR1[i]=  NumVar3Matrix(env, resources.size()); 
            CRR1[i]=  NumVar4Matrix(env, resources.size()); 
            CF[i]=  NumVar3Matrix(env, resources.size()); 
            CCF[i]=  NumVarMatrix(env, tasks.size()); 
            
            //OHL[i]=IloNumVarArray(env, tasks.size());
            
            for(int q=0;q<resources.size();q++)
            {
              
                
                SS[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
               
                SS2[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                SSS1[i][q]=  IloNumVarArray(env, tasks.size());
               
                SSSS1[i][q]=  NumVarMatrix(env, tasks.size());
               
                
                //BB[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                //BBB[i][q]=  IloNumVarArray(env, processors.size());
                
              
               
                
                
                GS[i][q]=NumVarMatrix(env,tasks.size());
                GGS[i][q]=NumVar3Matrix(env,tasks.size());
                GGGS[i][q]=NumVar4Matrix(env,tasks.size());
               OB[i][q]=  IloNumVarArray(env, tasks.size());
               
                CR1[i][q]=  NumVarMatrix(env, tasks.size()); 
                CRR1[i][q]=  NumVar3Matrix(env, tasks.size());
                CF[i][q]=  NumVarMatrix(env, tasks.size());
               
                
                for(int k=0;k<tasks.size();k++)
                {
                    
                    SSS1[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                   
                    SSSS1[i][q][k]=IloNumVarArray(env,tasks.size());
                    
                    
                    for(int j=0;j<tasks.size();j++)
                    {
                        SSSS1[i][q][k][j]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                        
                    }
                    
                    
                    GS[i][q][k]=IloNumVarArray(env,resources.size());
                    GGS[i][q][k]=NumVarMatrix(env,resources.size());
                    GGGS[i][q][k]=NumVar3Matrix(env,resources.size());
                    
                    CR1[i][q][k]=  IloNumVarArray(env, resources.size()); 
                    CRR1[i][q][k]=  NumVarMatrix(env, resources.size());
                    CF[i][q][k]=  IloNumVarArray(env, resources.size()); 
                    
                    
                    for(int r=0;r<resources.size();r++)
                    {
                        GS[i][q][k][r]=IloNumVar(env, 0.0, 1.0, ILOINT);
                        GGS[i][q][k][r]=IloNumVarArray(env,tasks.size());
                        GGGS[i][q][k][r]=NumVarMatrix(env,tasks.size());
                        
                        CR1[i][q][k][r]=  IloNumVar(env, 0.0, IloInfinity, ILOINT);
                        CRR1[i][q][k][r]=  IloNumVarArray(env, tasks.size());
                        CF[i][q][k][r]=  IloNumVar(env, 0.0, IloInfinity, ILOINT);
                        
                        for(int a=0;a<tasks.size();a++)
                        {
                            GGS[i][q][k][r][a]=IloNumVar(env, 0.0, 1.0, ILOINT);
                            GGGS[i][q][k][r][a]=IloNumVarArray(env,tasks.size());
                            for(int b=0;b<tasks.size();b++)
                                GGGS[i][q][k][r][a][b]=IloNumVar(env, 0.0, 1.0, ILOINT);
                            CRR1[i][q][k][r][a]=  IloNumVar(env, 0.0, IloInfinity, ILOINT);
                        }
                    }
                     
                     
                     OB[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    
                }
               
                
//                for(int k=0;k<processors.size();k++)
//                     BBB[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            }
          
            

            for(int j=0; j<tasks.size();j++)
            {
                pi[i][j]= IloNumVar(env, 0.0, 1.0, ILOINT);
                G[i][j]= IloNumVar(env, 0.0, 1.0, ILOINT);
                OH[i][j]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
               
                OLL[i][j]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                NB[i][j]=IloNumVarArray(env, resources.size());
                CCF[i][j]=  IloNumVarArray(env, tasks.size());
                
                BB[i][j]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                BBB[i][j]=IloNumVarArray(env, resources.size());
                
                for(int r=0;r<resources.size();r++)
                {
                    NB[i][j][r]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    BBB[i][j][r]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                    CCF[i][j][r]=  IloNumVar(env, 0.0, IloInfinity, ILOINT);
                }
                //OHL[i][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                
            }

            
            for(int p=0;p<processors.size();p++)
                A[i][p]=IloNumVar(env, 0.0, 1.0, ILOINT);

         }
        
        for(int q=0;q<resources.size();q++)
        {
             W[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
             C[q]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
             
             GLO[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
             LOC[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
             
        }
        
         
        
        ///////////////////////////////////////////////////////////////////////////////CONSTRAINTS
        
        //////////////////////////////////////////////////////////////////////////////////////////////
//        mod.add(A[0][2] == 1);
//        mod.add(A[1][3] == 1);
//        mod.add(A[2][1] == 1);
//        mod.add(A[3][1] == 1);
//        mod.add(A[4][1] == 1);
//        mod.add(A[5][1] == 1);
//        mod.add(A[6][0] == 1);
//        mod.add(A[7][0] == 1);
//        
//        mod.add(pi[0][1] == 1);
//        mod.add(pi[0][2] == 1);
//        mod.add(pi[0][3] == 1);
//        mod.add(pi[0][4] == 1);
//        mod.add(pi[0][5] == 1);
//        mod.add(pi[0][6] == 1);
//        mod.add(pi[0][7] == 1);
//        mod.add(pi[1][2] == 1);
//        mod.add(pi[1][3] == 1);
//        mod.add(pi[1][4] == 1);
//        mod.add(pi[1][5] == 1);
//        mod.add(pi[1][6] == 1);
//        mod.add(pi[1][7] == 1);
//        mod.add(pi[2][3] == 0);//////////////////////////////////////
//        mod.add(pi[2][4] == 0);//////////////////////////////////////
//        mod.add(pi[2][5] == 1);
//        mod.add(pi[2][6] == 1);
//        mod.add(pi[2][7] == 1);
//        mod.add(pi[3][4] == 1);
//        mod.add(pi[3][5] == 1);
//        mod.add(pi[3][6] == 1);
//        mod.add(pi[3][7] == 1);
//        mod.add(pi[4][5] == 1);
//        mod.add(pi[4][6] == 1);
//        mod.add(pi[4][7] == 1);
//        mod.add(pi[5][6] == 1);
//        mod.add(pi[5][7] == 1);
//        mod.add(pi[6][7] == 1);
        
        IloExpr exprZZ(env);
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        
        
        
        //C1:
        for(int i=0;i<tasks.size();i++)
        {
            IloExpr expr(env);
            for (int k = 0; k < processors.size(); k++)
                expr += A[i][k];
            mod.add(expr == 1);
            expr.end();  
        }
        
        //C2:
        for(int i=0;i<tasks.size();i++)
        {
            mod.add(G[i][i]==0);
            for(int j=0;j<tasks.size();j++)
                if(j!=i)
                    for(int p1=0;p1<processors.size();p1++)
                    {
                        mod.add(G[i][j] >= A[i][p1]+A[j][p1]-1 );
                        for(int p2=0;p2<processors.size();p2++)
                            if(p2!=p1)
                                mod.add(G[i][j] <= 2-A[i][p1]-A[j][p2] );

                    }     
        }

        //C3:
        for(int i=0;i<tasks.size();i++)
        {
            mod.add(pi[i][i] ==1);
            for(int j=0;j<tasks.size();j++)
                if(j!=i)
                {
                  
                    mod.add(pi[i][j]+pi[j][i]==1);
                    
                    for(int k=0;k<tasks.size();k++)
                        if(k!=j && k!=i)                            
                            mod.add(pi[i][k] >= pi[i][j]+pi[j][k]-1);

                }
        }
        
//        for(int i=0;i<tasks.size();i++)
//        {
//            for(int j=i+1;j<tasks.size();j++)
//               if(j!=i)
//                {
//                   IloExpr expr1(env);
//                   IloExpr expr2(env);
//                   for(int k=0;k<tasks.size();k++)
//                   {
//                       if(i!=k)
//                            expr1+=pi[i][k];
//                       if(j!=k)
//                            expr2+=pi[j][k];
//                       
//                   }
//                   
//                    mod.add(IloAbs(expr1-expr2) >=1); 
//                    expr1.end();
//                    expr2.end();
//                }
//        }


        //C4:
//        for(int i=0;i<tasks.size();i++)
//        {
//            mod.add(OH[i][i]==1);
//            for(int x=0;x<tasks.size();x++)
//                if(x!=i)
//                {
//                    //mod.add(NP[i][x]>=0);
//                    int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
//                    mod.add(OH[i][x]>= (R[i]/float(tasks[x].t))-M2*(1-G[i][x])-M2*pi[i][x]);
//                    mod.add(OH[i][x]<= 1-0.000001+(R[i]/float(tasks[x].t)));
//                    mod.add(OH[i][x]<= M2*G[i][x]);
//                    mod.add(OH[i][x]<= M2*(1-pi[i][x]));
//                }
//        }
        
        for(int i=0;i<tasks.size();i++)
        {
            mod.add(OH[i][i]==1);
            for(int x=0;x<tasks.size();x++)
                if(x!=i)
                {
                    //mod.add(NP[i][x]>=0);
                    int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
                    mod.add(OH[i][x]>= (((R[i]+R[x]-float(float(tasks[x].c)))/float(tasks[x].t))-M2*(1-G[i][x])-M2*pi[i][x]));
                    mod.add(OH[i][x]<= 1-0.000001+((R[i]+R[x]-float(float(tasks[x].c)))/float(tasks[x].t)));
                    mod.add(OH[i][x]<= M2*G[i][x]);
                    mod.add(OH[i][x]<= M2*(1-pi[i][x]));
                }
        }
        
        
        //C5:
        for(int i=0;i<tasks.size();i++)
        {
            IloExpr expr(env);
            for(int x=0;x<tasks.size();x++)
                if(x!=i)
                    {
                        expr+=OH[i][x]*tasks[x].c;
                    }
            expr+=tasks[i].c+B[i]+S[i]; 
            mod.add(R[i] == expr);
            expr.end(); 
        }
        
        
        //C6:
        for(int i=0;i<tasks.size();i++)        
            mod.add(R[i] <=tasks[i].t);
       
        for(int p=0;p<processors.size();p++)
        {
            IloExpr expr(env);
            for(int i=0;i<tasks.size();i++)            
                expr+=A[i][p]*tasks[i].u;
            
            mod.add(expr <=1.0);
            expr.end(); 
        }
        
        for(int i=0;i<tasks.size();i++)
        {
            mod.add(OLL[i][i]==1);
            for(int x=0;x<tasks.size();x++)
                if(x!=i)
                {
                    //mod.add(NP[i][x]>=0);
                    int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
                    mod.add(OLL[i][x]>= (1+(R[i]/float(tasks[x].t))-M2*(1-G[i][x])-M2*(1-pi[i][x])));
                    mod.add(OLL[i][x]<= 2-0.000001+((R[i]/float(tasks[x].t))));
                    mod.add(OLL[i][x]<= M2*G[i][x]);
                    mod.add(OLL[i][x]<= M2*(pi[i][x]));
                }
        }
        
        
        
        for(int i=0;i<tasks.size();i++)
        {
            //mod.add(PR[i][i]==0);
            for(int r1=1;r1<resources.size();r1+=2)
            if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())
               for(int j=0;j<tasks.size();j++)
               {
                    mod.add(OB[i][r1][j]<= 2-0.000001+(SS[i][r1]/float(tasks[j].t)));
                    mod.add(OB[i][r1][j] >= 1+(SS[i][r1]/float(tasks[j].t)));
                   
               }
            
        }
        
        for(int i=0;i<tasks.size();i++)
        {
            IloExpr expr1(env);
            int tresh=1+tasks[i].critSec.size();
            
            for(int j=0;j<tasks.size();j++)
                if(j!=i)
            {
                IloExpr expr2(env);    
                for(int k=0;k<tasks[j].critSec.size();k++)        
                {
                    int r1=tasks[j].critSec[k].resID;
                    float clength=tasks[j].critSec[k].length;
                    IloExpr expr3(env);
                    for(int a=0;a<tasks[j].critSec.size();a++)
                        if(tasks[j].critSec[a].length>clength)
                        {
                           
                            expr3+=NB[i][j][tasks[j].critSec[a].resID];
                        }
                   
                    mod.add(NB[i][j][r1]>= IloMin(tresh,OLL[i][j])-expr3-CCF[i][j][r1]);
//                    mod.add(NB[i][j][r1]>= IloMin(tresh,OLL[i][j])-expr3);
                    expr3.end();
                        int writer=tasks[j].critSec[k].writerID;
                        //if(tasks[j].critSec[k].resID%2==1)
                        if(j!=writer)
                        {
                            mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*G[writer][j]-M2*W[r1]);
                            for(int h=0;h<tasks.size();h++)
                                if(h!=j && find(tasks[h].resids.begin(),tasks[h].resids.end(),r1)!=tasks[h].resids.end())
                                {
                                    //cout<<i<<"-"<<j<<"-"<<r1<<"-"<<h<<endl;
                                    mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*(1-G[writer][j])-M2*W[r1]-M2*(1-G[h][j])-M2*abs(i-h)*(1-pi[h][i]));
                                }
                        }
                        else
                        {
                            if(tasks[j].critSec[k].resID%2==1)
                            {
                                for(int h=0;h<tasks.size();h++)
                                if(h!=j && find(tasks[h].resids.begin(),tasks[h].resids.end(),r1)!=tasks[h].resids.end())
                                    mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*W[r1]-M2*G[h][j]);
                            }
                            else
                            {
                                for(int h=0;h<tasks.size();h++)
                                if(h!=j && find(tasks[h].resids.begin(),tasks[h].resids.end(),r1)!=tasks[h].resids.end())
                                    mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*W[r1]-M2*(1-G[h][j])-M2*abs(i-h)*(1-pi[h][i]));
                            }
                        }
                        
//                        if(j!=writer)
//                        {
////                            mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*(1-G[writer][j]));
////                            mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*G[writer][j]-M2*W[tasks[j].critSec[k].resID]);
//                            mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*W[r1]);
//                        }
//                        else
//                        {
//                            if(tasks[j].critSec[k].resID%2==1)
//                                mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*G[j][writer]-M2*W[r1]);
//                            else
//                                mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*(1-G[j][writer])-M2*W[r1]);
//                            
////                             for(int i=0;i<tasks.size();i++)
////                if(i!=writer)
////                    for(int j=0;j<tasks[i].critSec.size();j++)
////                        if(tasks[i].critSec[j].resID==resources[q].resID)
////                            if(tasks[j].critSec[k].resID%2==0)
////                                 mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*(1-LOC[tasks[j].critSec[k].resID-1]));
////                            else 
////                                 mod.add(BBB[i][j][r1] >= tasks[j].critSec[k].length*NB[i][j][r1]-M2*(1-GLO[tasks[j].critSec[k].resID])-M2*W[tasks[j].critSec[k].resID]);
//                        }
                     
                    expr2+=BBB[i][j][r1];
                }
               
                mod.add(BB[i][j] == expr2);
                expr1+=BB[i][j];
                expr2.end();
            }
        mod.add(B[i] == expr1);  
        expr1.end();
        }
        
        
        
        for (int q=1; q<resources.size(); q+=2)
        {
            //int nr=0;
            vector <int> readers;
            int writer=resources[q].writer;
            IloExpr expr(env); 
            for(int i=0;i<tasks.size();i++)
                if(i!=writer)
                    for(int j=0;j<tasks[i].critSec.size();j++)
                        if(tasks[i].critSec[j].resID==resources[q].resID)
                        {
                            expr+=G[i][writer];
                            
                            mod.add(LOC[q] >= G[i][writer]);
                            mod.add(GLO[q] >= 1-G[i][writer]);
                            //nr++;
                        }
            expr.end();
           
        }
        
        
        
            for(int r1=1;r1<resources.size();r1+=2)
            {
                vector <int> Qtasks1; 
                for(int j=0;j<tasks.size();j++)
                    for(int k=0;k<tasks[j].resids.size();k++)
                        if(tasks[j].resids[k]==resources[r1].resID)
                            Qtasks1.push_back(j);
                    int w1=resources[r1].writer;
                
                for(int r2=1;r2<resources.size();r2+=2)
                    if(r2!=r1)
                    {
                        vector <int> Qtasks2; 
                        for(int j=0;j<tasks.size();j++)
                            for(int k=0;k<tasks[j].resids.size();k++)
                                if(tasks[j].resids[k]==resources[r2].resID)
                                    Qtasks2.push_back(j);
                        int w2=resources[r2].writer;    
                        
                        for(int ii=0;ii<Qtasks1.size();ii++)
                        {
                            int i=Qtasks1[ii];
                            for(int jj=0;jj<Qtasks2.size();jj++)
                               if(Qtasks2[jj]!=i)
                               {
                                   int j=Qtasks2[jj];
                                   
                                   for(int aa=0;aa<Qtasks1.size();aa++)
                                    if(Qtasks1[aa]!=i)   
                                    {
                                        int a=Qtasks1[aa];
                                        for(int bb=0;bb<Qtasks2.size();bb++)
                                        if(Qtasks2[bb]!=j)
                                        {
                                            IloExpr expr(env);
                                            int b=Qtasks2[bb];
                                            //mod.add(GGGS[i][r1][j][r2][a][b] >= G[i][a]);
                                            mod.add(GGGS[i][r1][j][r2][a][b] >= G[j][b]);
                                            mod.add(GGGS[i][r1][j][r2][a][b] >= pi[a][b]);
                                            
                                            if(w2!=b)
                                            {
                                                mod.add(GGGS[i][r1][j][r2][a][b] >= G[w2][b]);
                                                expr+=G[w2][b];
                                            }
//                                             mod.add(GGGS[i][r1][j][r2][a][b] <= G[i][a]+G[j][b]+pi[a][b]+expr);
                                            mod.add(GGGS[i][r1][j][r2][a][b] <= G[j][b]+pi[a][b]+expr);
                                            
                                            //mod.add(GGS[i][r1][j][r2][a] <= GGGS[i][r1][j][r2][a][b]);
                                            //mod.add(GGS[i][r1][j][r2][a] >=IloMin(GGGS[i][r1][j][r2][a][b],1));
                                            expr.end();

                                        }
                                        for(int x=0;x<tasks.size();x++)
                                            if(x==j || find(Qtasks2.begin(),Qtasks2.end(),x)==Qtasks2.end())
                                                 mod.add(GGGS[i][r1][j][r2][a][x] ==1);
//                                        
                                         
                                        
                                        mod.add(GGS[i][r1][j][r2][a] <=IloMin(GGGS[i][r1][j][r2][a]));
                                        mod.add(GGS[i][r1][j][r2][a] <=1-G[i][a]);
                                        if(w1!=a)
                                        mod.add(GGS[i][r1][j][r2][a] <=(1-G[w1][a]));
                                      
                                        //-G[i][a]-abs(w1-a)*G[w1][a]
                                    }
                                   for(int x=0;x<tasks.size();x++)
                                            if(x==i || find(Qtasks1.begin(),Qtasks1.end(),x)==Qtasks1.end())
                                                 mod.add(GGS[i][r1][j][r2][x] ==0);
                                    mod.add(GS[i][r1][j][r2] ==IloMax(GGS[i][r1][j][r2]));
                               }
                        }
                    }
            }
                
        
        for(int i=0;i<tasks.size();i++)
        for(int r1=1;r1<resources.size();r1+=2)
        if(tasks[i].resids.size()>0)    
        if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())    
        {
            
            vector <int> Qtasks1; 
            for(int j=0;j<tasks.size();j++)
                for(int k=0;k<tasks[j].resids.size();k++)
                    if(tasks[j].resids[k]==resources[r1].resID)
                        Qtasks1.push_back(j);
                int w1=resources[r1].writer;
                
                for(int jj=0;jj<Qtasks1.size();jj++)
                if(Qtasks1[jj]!=i)    
                {
                   
                    int j=Qtasks1[jj];

                    float lj=0.0;
                    for(int a=0;a<tasks[j].critSec.size();a++)
                        if(tasks[j].critSec[a].resID==resources[r1].resID)
                            lj=tasks[j].critSec[a].length;

                    
                    IloExpr expr5(env);
                    for(int r2=1;r2<resources.size();r2+=2)
                    if(r2!=r1)
                    {
                        
                        vector <int> Qtasks2; 
                        for(int a=0;a<tasks.size();a++)
                            for(int k=0;k<tasks[a].resids.size();k++)
                                if(tasks[a].resids[k]==resources[r2].resID)
                                    Qtasks2.push_back(a);

                            int w2=resources[r2].writer;

                        for(int kk=0;kk<Qtasks2.size();kk++)
                        if(Qtasks2[kk]!=j)
                        {
                            
                            int k=Qtasks2[kk];
                            float lx=0.0;
                            for(int a=0;a<tasks[k].critSec.size();a++)
                                if(tasks[k].critSec[a].resID==resources[r2].resID)
                                    lx=tasks[k].critSec[a].length;
                            
                          
                          mod.add(SSSS1[i][r1][j][k] >= lx-M2*(1-pi[i][j])-M2*(1-G[j][k])-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]-M2*W[r2]-M2*W[r1]);
                          
                        }
                           

                    }
                   
                    
                    for(int k=0;k<tasks.size();k++)
                        expr5+=SSSS1[i][r1][j][k];
                    mod.add(SSS1[i][r1][j] >= expr5+lj-M2*(1-pi[i][j])-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-M2*W[r1]);
                    expr5.end();
 
                }
               
        }
        
        for(int i=0;i<tasks.size();i++)
        for(int r1=1;r1<resources.size();r1+=2)
        if(tasks[i].resids.size()>0)    
        if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())    
        {
            
            vector <int> Qtasks1; 
            for(int j=0;j<tasks.size();j++)
                for(int k=0;k<tasks[j].resids.size();k++)
                    if(tasks[j].resids[k]==resources[r1].resID)
                        Qtasks1.push_back(j);
                int w1=resources[r1].writer;
                
                IloExpr exprCF(env);
                
                for(int r2=1;r2<resources.size();r2+=2)
                    if(r2!=r1)
                    {
                        
                        vector <int> Qtasks2; 
                        for(int a=0;a<tasks.size();a++)
                            for(int k=0;k<tasks[a].resids.size();k++)
                                if(tasks[a].resids[k]==resources[r2].resID)
                                    Qtasks2.push_back(a);

                            int w2=resources[r2].writer;

                        for(int k=0;k<tasks.size();k++)
                        if(find(tasks[k].resids.begin(),tasks[k].resids.end(),r2)!=tasks[k].resids.end())    
                        //if(Qtasks2[kk]!=j)
                        {
                            
                            float lx=0.0;
                            for(int a=0;a<tasks[k].critSec.size();a++)
                                if(tasks[k].critSec[a].resID==resources[r2].resID)
                                    lx=tasks[k].critSec[a].length;
                            
                            IloExpr expr(env);
                            IloExpr expr2(env);
                                      
                            for(int jj=0;jj<Qtasks1.size();jj++)
                            if(Qtasks1[jj]!=i && Qtasks1[jj]!=k)    
                            {

                                int j=Qtasks1[jj];

                                float lj=0.0;
                                for(int a=0;a<tasks[j].critSec.size();a++)
                                    if(tasks[j].critSec[a].resID==resources[r1].resID)
                                        lj=tasks[j].critSec[a].length;
                                mod.add(CRR1[i][r1][k][r2][j] >= OB[i][r1][j]-M2*(1-G[j][k])-M2*pi[i][j]-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]-M2*W[r2]-M2*W[r1]);
                                expr+=CRR1[i][r1][k][r2][j];
                                

                            }
                            mod.add(CR1[i][r1][k][r2]==expr);
                            
                            for(int rx=0;rx<tasks[k].critSec.size();rx++)
                            {
                               
                                if(tasks[k].critSec[rx].resID!=resources[r2].resID && tasks[k].critSec[rx].resID%2==1)
                                    if(tasks[k].critSec[rx].length > lx)
                                    {
                                        expr2+=CF[i][r1][k][tasks[k].critSec[rx].resID];
//                                        if(i==6 && r1==1 && k==2)
//                                            cout<<"** "<<tasks[k].critSec[rx].resID<<endl;
                                    }
                            }
                            mod.add(CF[i][r1][k][r2] >= IloMin(OB[i][r1][k],CR1[i][r1][k][r2]-expr2));/////////////MISSING HERE
                            exprCF+=CF[i][r1][k][r2]*lx;
                           
                            //exprZZ+=CF[i][r1][k][r2];
//                          mod.add(SSSS1[i][r1][j][k] >= lx-M2*(1-pi[i][j])-M2*(1-G[j][k])-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]-M2*W[r2]);
//                          mod.add(SSSS2[i][r1][j][k] >= OB[i][r1][j]*lx-M2*pi[i][j]-M2*(1-G[j][k])-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]-M2*W[r2]);           
                          
                            expr.end();
                            expr2.end();
                          
                        }
                        else
                            mod.add(CF[i][r1][k][r2]==0);
                           

                    }
                    else
                    {
                        
                        for(int j=0;j<tasks.size();j++)
                        if(j!=i && find(tasks[j].resids.begin(),tasks[j].resids.end(),r1)!=tasks[j].resids.end())      
                            {

                                float lj=0.0;
                                for(int a=0;a<tasks[j].critSec.size();a++)
                                    if(tasks[j].critSec[a].resID==resources[r1].resID)
                                        lj=tasks[j].critSec[a].length;
                                
                                mod.add(CF[i][r1][j][r1] >= OB[i][r1][j]-M2*pi[i][j]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-M2*W[r1]);
                                exprCF+=CF[i][r1][j][r1]*lj;
                               

                            }
                        else
                            mod.add(CF[i][r1][j][r1]==0);
                    }

                mod.add(SS2[i][r1]==exprCF);
                exprZZ+=SS2[i][r1];
                exprCF.end();
        }
        
        
        for(int i=0;i<tasks.size();i++)
        if(tasks[i].resids.size()>0) 
        for(int k=0;k<tasks.size();k++)
        for(int r2=1;r2<resources.size();r2+=2)
        {
            IloExpr expr4(env); 
            for(int r1=1;r1<resources.size();r1+=2)
                if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())    
                expr4+=CF[i][r1][k][r2];
            mod.add(CCF[i][k][r2]==expr4);
            expr4.end();
            
        }
        
       
        
//        vector <int> Qtasks1; 
//            for(int j=0;j<tasks.size();j++)
//                for(int k=0;k<tasks[j].resids.size();k++)
//                    if(tasks[j].resids[k]==resources[r1].resID)
//                        Qtasks1.push_back(j);
        

                
        
//            
//        
        for(int i=0;i<tasks.size();i++)
        {
            IloExpr expr2(env);
            for(int r1=1;r1<resources.size();r1+=2)
            if(tasks[i].resids.size()>0)  
            if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())
            {
                vector <int> Qtasks1; 
                for(int j=0;j<tasks.size();j++)
                    for(int k=0;k<tasks[j].resids.size();k++)
                        if(tasks[j].resids[k]==resources[r1].resID)
                            Qtasks1.push_back(j);
                    IloExpr expr(env);
                   

                    for(int jj=0;jj<Qtasks1.size();jj++)
                    if(Qtasks1[jj]!=i)    
                    {
                        int j=Qtasks1[jj];
                       
                        
                    }
               
                mod.add(SS[i][r1] == SS2[i][r1]+IloMax(SSS1[i][r1]));
                //mod.add(SS[i][r1] == expr+IloMax(SSS1[i][r1]));
                expr.end();
                expr2+=SS[i][r1];
               
            }
            
            mod.add(S[i] == expr2);
            expr2.end();
        }
 

        
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
                            expr+=G[i][writer];
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
                mod.add(C[q]>=sizeByTask-M2*G[writer][i]-M2*(1-W[q]));
               
            }
        mod.add(C[q+1]==0);
    }
        
         
     /////////////////////////////////////////////////////////////////EOC
        
       IloExpr expr2(env);
        for(int q=1;q<resources.size();q+=2)
        {
           expr2+=W[q];
        }
         mod.add(expr2==0);
  //       expr2.end();
//        IloExpr expr(env); 
//        for(int q=1;q<resources.size();q++)
//        {
//           expr += C[q];
//        }
//        
        
        
        
        for(int q=1;q<resources.size();q+=2)
            mod.add(W[q]==0);
        IloExpr expr(env);   
        for(int i=0;i<tasks.size();i++)
        {
            expr+=R[i];
        }

    
        mod.add(IloMinimize(env,expr));
        exprZZ.end(); 
        expr.end();    
  
        IloCplex cplex(mod);
        cplex.exportModel("m0.lp");
        cplex.setOut(env.getNullStream());
        //cplex.setParam(IloCplex::Param::MIP::Strategy::NodeSelect, 2);
           //cplex.setParam(IloCplex::Param::Emphasis::Memory, 1);
        cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, 0);
cplex.setParam(IloCplex::Param::TimeLimit, 7200);
cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 12100);
//cplex.setParam(IloCplex::Param::MIP::Cuts::Gomory, 2);
cplex.setParam(IloCplex::Param::WorkMem, 13000);
//cplex.setParam(IloCplex::WorkDir,"/home/zalbay/rtas-ext-cplex/17-2nodentune/temp");
//cplex.setParam(IloCplex::Param::MIP::Strategy::File,3);


        
 
        bool bval;
        bval=cplex.solve();
        if(bval && (cplex.getStatus()==IloAlgorithm::Optimal || cplex.getStatus()==IloAlgorithm::Feasible))
        {    
          
         
           cout<< "solution status = " << cplex.getStatus() << endl;
            
           vector <double> x1;
           x1.resize(tasks.size());
           for(int k=0;k<tasks.size();k++)
           {
                   x1[k] = cplex.getValue(R[k]);
                   tasks[k].ilpR=(float)x1[k];
           }
           
           for(int k=0;k<tasks.size();k++)
           {
                float xb;
                xb= cplex.getValue(B[k]);
                tasks[k].ilpB=xb;
           }
           
          
           int x2;
            for(int i=0;i<tasks.size();i++)
            {
                int prio=0;
                for(int k=0;k<tasks.size();k++)
                    if(k!=i)    
                    {
                            x2= cplex.getValue(pi[i][k]);
                            if(x2==1)
                                prio++;

                    }
                tasks[i].priority=tasks.size()-prio-1;
           
            }
           
           
           
           for(int k=0;k<tasks.size();k++)
               for(int p=0;p<processors.size();p++)
                {
                       if(cplex.getValue(A[k][p])>0.99)
                       {
                           assignTask(k,p);
                           
                       }
                } 

		for(int q=1;q<resources.size();q++)
           {
               int type=cplex.getValue(W[q]);
               //int rcost=cplex.getValue(C[q]);
               if(type==1)
                    resources[q].protocol=2;
           }
           
           
           
           
           test<<seed<<endl;
            test<<"task"<<"\t"<<"prio"<<"\t"<<"alloc"<<"\t"<<"Resp"<<"\t"<<"spin"<<"\t"<<"blok"<<endl;   
            for(int i=0;i<tasks.size();i++)
            {
                float blok,spin;
                blok=cplex.getValue(B[i]);
                spin=cplex.getValue(S[i]);
                test<<i<<"\t"<<tasks[i].priority<<"\t"<<tasks[i].procAlloc<<"\t"<<tasks[i].ilpR<<"\t"<<spin<<"\t"<<blok<<endl; 
            }
            test<<"======================"<<endl;
            
        
//           for(int i=0;i<tasks.size();i++)
//        for(int r1=1;r1<resources.size();r1+=2)
//        if(tasks[i].resids.size()>0)    
//        if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())    
//        {
//            
//            vector <int> Qtasks1; 
//            for(int j=0;j<tasks.size();j++)
//                for(int k=0;k<tasks[j].resids.size();k++)
//                    if(tasks[j].resids[k]==resources[r1].resID)
//                        Qtasks1.push_back(j);
//                int w1=resources[r1].writer;
//                
//                for(int r2=1;r2<resources.size();r2+=2)
//                    if(r2!=r1)
//                    {
//                        
//                        vector <int> Qtasks2; 
//                        for(int a=0;a<tasks.size();a++)
//                            for(int k=0;k<tasks[a].resids.size();k++)
//                                if(tasks[a].resids[k]==resources[r2].resID)
//                                    Qtasks2.push_back(a);
//
//                            int w2=resources[r2].writer;
//
////                        for(int k=0;k<tasks.size();k++)
////                        if(find(tasks[k].resids.begin(),tasks[k].resids.end(),r2)!=tasks[k].resids.end())    
////                        //if(Qtasks2[kk]!=j)
////                        {
////                                                   
////                            int dd=cplex.getValue(CF[i][r1][k][r2]);
////                            if(i==3 && dd>0)
////                                cout<<i<<"-"<<r1<<"-"<<k<<"-"<<r2<<"\t"<<dd<<endl;
////                           
////                        }
//                        
//
//                    }
//                    else
//                    {
//                        
//                        for(int j=0;j<tasks.size();j++)
//                        if(j!=i && find(tasks[j].resids.begin(),tasks[j].resids.end(),r1)!=tasks[j].resids.end())      
//                            {
//
//                                 int dd=cplex.getValue(CF[i][r1][j][r1]);
//                                if(i==3 && dd>0)
//                                    cout<<i<<"-"<<r1<<"-"<<j<<"-"<<r1<<"\t"<<dd<<endl;
//                               
//
//                            }
//                       
//                    }   
//        }
            
            
//            if(aa>0)
//            {
//                
//                cout<<"@"<<i<<"-"<<r1<<"-"<<k<<"-"<<r2<<"\t"<<aa<<"\t"<<bb<<endl;
//            }

         
            
//        for(int i=0;i<tasks.size();i++)
//        {
//           
//            
//            for(int j=0;j<tasks.size();j++)
//                if(j!=i)
//            {
//                 
//                for(int k=0;k<tasks[j].critSec.size();k++)        
//                {
//                    int r1=tasks[j].critSec[k].resID;
//                    int aa=cplex.getValue(NB[i][j][r1]);
//                    float dd=cplex.getValue(BBB[i][j][r1]);
//                    float bb=cplex.getValue(BB[i][j]);
//                    float cc=cplex.getValue(B[i]);
//                   //int ee=cplex.getValue(CCF[4][2][5]);
//                    
//                    if(i==3)
//                    cout<<"&"<<i<<"-"<<j<<"-"<<r1<<"\t"<<aa<<"\t"<<dd<<"\t"<<bb<<"\t"<<cc<<endl;
//                  
//                }
//               
//                
//            }
//        
//        }
            
//        for(int i=0;i<tasks.size();i++)
//        if(tasks[i].resids.size()>0) 
//        for(int k=0;k<tasks.size();k++)
//        for(int r2=1;r2<resources.size();r2+=2)
//            
//        for(int r1=1;r1<resources.size();r1+=2)  
//            if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())
//        {
//            int aa=cplex.getValue(CCF[i][k][r2]);
//            int bb=cplex.getValue(CF[i][r1][k][r2]);
//            
//            if(aa>0)
//            {
//                
//                cout<<"@"<<i<<"-"<<r1<<"-"<<k<<"-"<<r2<<"\t"<<aa<<"\t"<<bb<<endl;
//            }
//            //<<"\t"<<bb
//        }
            
       
            


        
            if(cplex.getStatus()==IloAlgorithm::Optimal)
                return 1;
            else
                return 2;
        
        }
        else
        {
            cout<<"NS"<<endl;
            cout<< "solution status = " << cplex.getStatus() << endl;
            return 0;
        }
          
    }
       catch (IloException& ex) {
          cerr << "Error: " << ex << endl;
       }
       catch (...) {
          cerr << "Error" << endl;
       }

       env.end();
       
       
  
       
       return 0;
       
}

#define	ILPANAYSIS_H



#endif	/* ILPANAYSIS_H */
