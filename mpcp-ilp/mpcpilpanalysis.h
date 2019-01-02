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
        NumVarMatrix PL(env,tasks.size());   //number of interferences
        NumVarMatrix PR(env,tasks.size()); 
        
        NumVar3Matrix FFF(env,tasks.size());
        NumVar4Matrix FFFF(env,tasks.size());
        
        IloNumVarArray S(env,tasks.size());
        NumVarMatrix SS(env,tasks.size());   //must be the same size as S
        NumVar3Matrix SSS1(env,tasks.size()); //must be the same size as S
        NumVar4Matrix SSSS1(env,tasks.size()); //must be the same size as S
        NumVar3Matrix SSS2(env,tasks.size()); //must be the same size as S
        NumVar4Matrix SSSS2(env,tasks.size()); //must be the same size as S
        NumVarMatrix NA(env,tasks.size());   //must be the same size as S
        NumVar3Matrix NAA(env,tasks.size());  
        
         IloNumVarArray B(env,tasks.size());
        NumVarMatrix BB(env,tasks.size());   //must be the same size as S
        //NumVar3Matrix BBB(env,tasks.size());
        
        IloNumVarArray W(env,resources.size());
        IloNumVarArray C(env,resources.size());
        
        NumVar4Matrix GS(env,tasks.size());
        NumVar5Matrix GGS(env,tasks.size());
        NumVar6Matrix GGGS(env,tasks.size());
        
//        IloNumVarArray LOC(env,resources.size());
//        IloNumVarArray GLO(env,resources.size());
        
        NumVar3Matrix OB(env,tasks.size());
        
        //NumVarMatrix PLL(env,tasks.size());   //must be the same size as S
        
        
        for(int i=0; i<tasks.size();i++) 
        {
           
            
            A[i] = IloNumVarArray(env, processors.size());
            pi[i] = IloNumVarArray(env, tasks.size());
            G[i] =  IloNumVarArray(env, tasks.size());
            PL[i] =  IloNumVarArray(env, tasks.size());
            PR[i] =  IloNumVarArray(env, tasks.size());
            R[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            
            FFF[i]=  NumVarMatrix(env, resources.size());
            FFFF[i]=  NumVar3Matrix(env, resources.size());
            
            S[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            SS[i]=  IloNumVarArray(env, resources.size());
            SSS1[i]=  NumVarMatrix(env, resources.size());    //must be same size as SS
            SSS2[i]=  NumVarMatrix(env, resources.size());    //must be same size as SS
            SSSS1[i]=  NumVar3Matrix(env, resources.size());    //must be same size as SS
            SSSS2[i]=  NumVar3Matrix(env, resources.size());    //must be same size as SS
            
            B[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            BB[i]=  IloNumVarArray(env, tasks.size());
            //BBB[i]=  NumVarMatrix(env, resources.size());
            
            NA[i]=  IloNumVarArray(env, resources.size());
            NAA[i]=  NumVarMatrix(env, resources.size());    //must be same size as SS
            
            GS[i]=NumVar3Matrix(env,resources.size());
            GGS[i]=NumVar4Matrix(env,resources.size());
            GGGS[i]=NumVar5Matrix(env,resources.size());
            
            OB[i]=  NumVarMatrix(env, resources.size());    
            
            //PLL[i]=IloNumVarArray(env, tasks.size());
            
            for(int q=0;q<resources.size();q++)
            {
                FFF[i][q]=  IloNumVarArray(env, tasks.size());
                FFFF[i][q]=  NumVarMatrix(env, tasks.size());
                
                SS[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                SSS1[i][q]=  IloNumVarArray(env, tasks.size());
                SSS2[i][q]=  IloNumVarArray(env, tasks.size());
                SSSS1[i][q]=  NumVarMatrix(env, tasks.size());
                SSSS2[i][q]=  NumVarMatrix(env, tasks.size());
                
                //BB[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                //BBB[i][q]=  IloNumVarArray(env, processors.size());
                
                NA[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                NAA[i][q]=  IloNumVarArray(env, tasks.size());
                
                GS[i][q]=NumVarMatrix(env,tasks.size());
                GGS[i][q]=NumVar3Matrix(env,tasks.size());
                GGGS[i][q]=NumVar4Matrix(env,tasks.size());
               OB[i][q]=  IloNumVarArray(env, tasks.size());
                
                for(int k=0;k<tasks.size();k++)
                {
                    FFF[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    FFFF[i][q][k]=IloNumVarArray(env, tasks.size());
                    for(int j=0;j<tasks.size();j++)
                            FFFF[i][q][k][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    
                    SSS1[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                    SSS2[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                    SSSS1[i][q][k]=IloNumVarArray(env,tasks.size());
                    SSSS2[i][q][k]=IloNumVarArray(env,tasks.size());
                    
                    for(int j=0;j<tasks.size();j++)
                    {
                        SSSS1[i][q][k][j]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                        SSSS2[i][q][k][j]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                    }
                    NAA[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    
                    GS[i][q][k]=IloNumVarArray(env,resources.size());
                    GGS[i][q][k]=NumVarMatrix(env,resources.size());
                    GGGS[i][q][k]=NumVar3Matrix(env,resources.size());
                    
                    for(int r=0;r<resources.size();r++)
                    {
                        GS[i][q][k][r]=IloNumVar(env, 0.0, 1.0, ILOINT);
                        GGS[i][q][k][r]=IloNumVarArray(env,tasks.size());
                        GGGS[i][q][k][r]=NumVarMatrix(env,tasks.size());
                        
                        for(int a=0;a<tasks.size();a++)
                        {
                            GGS[i][q][k][r][a]=IloNumVar(env, 0.0, 1.0, ILOINT);
                            GGGS[i][q][k][r][a]=IloNumVarArray(env,tasks.size());
                            for(int b=0;b<tasks.size();b++)
                                GGGS[i][q][k][r][a][b]=IloNumVar(env, 0.0, 1.0, ILOINT);
                        }
                    }
                     BB[i][k]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                     OB[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                }
               
                
//                for(int k=0;k<processors.size();k++)
//                     BBB[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            }
          
            

            for(int j=0; j<tasks.size();j++)
            {
                pi[i][j]= IloNumVar(env, 0.0, 1.0, ILOINT);
                G[i][j]= IloNumVar(env, 0.0, 1.0, ILOINT);
                PL[i][j]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                PR[i][j]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                
                //PLL[i][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                
            }

            
            for(int p=0;p<processors.size();p++)
                A[i][p]=IloNumVar(env, 0.0, 1.0, ILOINT);

         }
        
        for(int q=0;q<resources.size();q++)
        {
             W[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
             C[q]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
             
//             GLO[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
//             LOC[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
             
        }
        
         
        
        ///////////////////////////////////////////////////////////////////////////////CONSTRAINTS
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
//            mod.add(PL[i][i]==1);
//            for(int x=0;x<tasks.size();x++)
//                if(x!=i)
//                {
//                    //mod.add(NP[i][x]>=0);
//                    int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
//                    mod.add(PL[i][x]>= (R[i]/float(tasks[x].t))-M2*(1-G[i][x])-M2*pi[i][x]);
//                    mod.add(PL[i][x]<= 1-0.000001+(R[i]/float(tasks[x].t)));
//                    mod.add(PL[i][x]<= M2*G[i][x]);
//                    mod.add(PL[i][x]<= M2*(1-pi[i][x]));
//                }
//        }
        
        for(int i=0;i<tasks.size();i++)
        {
            mod.add(PL[i][i]==1);
            for(int x=0;x<tasks.size();x++)
                if(x!=i)
                {
                    //mod.add(NP[i][x]>=0);
                    int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
                    mod.add(PL[i][x]>= (((R[i]+R[x]-float(float(tasks[x].c)))/float(tasks[x].t))-M2*(1-G[i][x])-M2*pi[i][x]));
                    mod.add(PL[i][x]<= 1-0.000001+((R[i]+R[x]-float(float(tasks[x].c)))/float(tasks[x].t)));
                    mod.add(PL[i][x]<= M2*G[i][x]);
                    mod.add(PL[i][x]<= M2*(1-pi[i][x]));
                }
        }
        
        for(int i=0;i<tasks.size();i++)
        {
            mod.add(PR[i][i]==0);
            for(int x=0;x<tasks.size();x++)
                if(x!=i)
                {
                    //int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
                    int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t))+1; 
                    mod.add(PR[i][x]<= 2-0.000001+(R[i]/float(tasks[x].t)));
                    mod.add(PR[i][x] >= 1+(R[i]/float(tasks[x].t))-M2*(G[i][x]));
                     mod.add(PR[i][x] <= M2*(1-G[i][x]));

                }
         }
    

        //C5:
        for(int i=0;i<tasks.size();i++)
        {
            IloExpr expr(env);
            for(int x=0;x<tasks.size();x++)
                if(x!=i)
                    {
                        expr+=PL[i][x]*tasks[x].c;
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
        
        
//        for(int i=0;i<tasks.size();i++)
//        {
//            mod.add(PLL[i][i]==0);
//            for(int x=0;x<tasks.size();x++)
//                if(x!=i)
//                {
//                    //int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
//                    mod.add(PLL[i][x]<= 2-0.000001+(R[i]/float(tasks[x].t)));
//                    mod.add(PLL[i][x] >= 1+(R[i]/float(tasks[x].t))-M2*(1-G[i][x])-M2*(1-pi[i][x]));
//                    mod.add(PLL[i][x] <= M2*G[i][x]);
//                    mod.add(PLL[i][x] <= M2*pi[i][x]);
//
//                }
//         }
        
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
            IloExpr expr(env);
            for(int j=0;j<tasks.size();j++)
                if(j!=i)
                {
                    float longest=0.0;
                    for(int k=0;k<tasks[j].critSec.size();k++)
                     
                    {
//                        if(tasks[j].critSec[k].length > longest)
//                            longest=tasks[j].critSec[k].length;
                        int writer=tasks[j].critSec[k].writerID;
                        vector <int> Qtasks1; 
                                for(int a=0;a<tasks.size();a++)
                                    for(int b=0;b<tasks[a].resids.size();b++)
                                        if(tasks[a].resids[b]==tasks[j].critSec[k].resID)
                                            Qtasks1.push_back(a);
                                    
                        for(int a=0;a<Qtasks1.size();a++)
                        {

                            int zz=Qtasks1[a];
                            if(zz!=j)
                            {
                                if(tasks[j].critSec[k].resID%2==1)  
                                    mod.add(BB[i][j] >= tasks[j].critSec[k].length*(1+tasks[i].critSec.size())-M2*(1-G[i][j])-M2*(1-pi[i][j])-M2*G[j][zz]-abs(writer-j)*M2*G[j][writer]-M2*W[tasks[j].critSec[k].resID]);
                                mod.add(BB[i][j] >= tasks[j].critSec[k].length*(1+tasks[i].critSec.size())-M2*(1-G[i][j])-M2*(1-pi[i][j])-M2*(1-G[j][zz])-abs(writer-j)*M2*(1-G[j][writer]));
                                
                            }
                        }
                        
                    }
                    
                    expr+=BB[i][j];
                }
            mod.add(B[i] == expr);
        }
        
        
        
//        for (int q=1; q<resources.size(); q+=2)
//        {
//            //int nr=0;
//            vector <int> readers;
//            int writer=resources[q].writer;
//            IloExpr expr(env); 
//            for(int i=0;i<tasks.size();i++)
//                if(i!=writer)
//                    for(int j=0;j<tasks[i].critSec.size();j++)
//                        if(tasks[i].critSec[j].resID==resources[q].resID)
//                        {
//                            expr+=G[i][writer];
//                            
//                            mod.add(LOC[q] >= G[i][writer]);
//                            mod.add(GLO[q] >= 1-G[i][writer]);
//                            //nr++;
//                        }
//           
//        }
        
        
        
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

                    IloExpr expr(env);
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
                            
                           
                          
                          mod.add(SSSS1[i][r1][j][k] >= lx-M2*(1-pi[i][j])-M2*(1-G[j][k])-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]-M2*W[r2]);
                          mod.add(SSSS2[i][r1][j][k] >= OB[i][r1][j]*lx-M2*pi[i][j]-M2*(1-G[j][k])-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]-M2*W[r2]);
            
                          
                          
                          
                        }
                           

                    }
                    for(int k=0;k<tasks.size();k++)
                        expr+=SSSS2[i][r1][j][k];
                    mod.add(SSS2[i][r1][j] >= expr+OB[i][r1][j]*lj-M2*pi[i][j]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-M2*W[r1]);
                    
                    for(int k=0;k<tasks.size();k++)
                        expr5+=SSSS1[i][r1][j][k];
                    mod.add(SSS1[i][r1][j] >= expr5+lj-M2*(1-pi[i][j])-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-M2*W[r1]);
                  
                    expr.end();
                    expr5.end();
 
                }
               
        }
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
                        expr+=SSS2[i][r1][j];

                    }
                mod.add(SS[i][r1] == expr+IloMax(SSS1[i][r1]));
                expr.end();
                expr2+=SS[i][r1];
               
            }
            
            mod.add(S[i] == expr2);
            expr2.end();
        }
        
     
                        
        
        
        
//        for(int i=0;i<tasks.size();i++)
//            for(int q=1;q<resources.size();q+=2)
//            {
//                vector <int> Qtasks; 
//                for(int j=0;j<tasks.size();j++)
//                    for(int k=0;k<tasks[j].resids.size();k++)
//                        if(tasks[j].resids[k]==resources[q].resID)
//                            Qtasks.push_back(j);
//                    int wr=resources[q].writer;
//                    
//                    for(int xx=0;xx<Qtasks.size();xx++)
//                    if(Qtasks[xx]!=i)    
//                    {
//                        int x=Qtasks[xx];
//                        IloExpr expr(env);
//                        
//                        for(int jj=0;jj<Qtasks.size();jj++)
//                            if(Qtasks[jj]!=x && Qtasks[jj]!=i)
//                            {
//                                int j=Qtasks[jj];
//                                float lj=0.0,lx=0.0;
//                                for(int k=0;k<tasks[j].critSec.size();k++)
//                                    if(tasks[j].critSec[k].resID==resources[q].resID)
//                                        lj=tasks[j].critSec[k].length;
//                                
//                                for(int k=0;k<tasks[x].critSec.size();k++)
//                                    if(tasks[x].critSec[k].resID==resources[q].resID)
//                                        lx=tasks[x].critSec[k].length;
//                                
//                                if(lx<lj)
//                                {
//                                    //test<<i<<" "<<q<<" "<<x<<" "<<j<<endl;-abs(wr-x)*M*G[wr][x]
////                                    mod.add(IloIfThen(env,G[j][x]==1,FFFF[i][q][x][j] == PR[i][j]));
////                                    mod.add(IloIfThen(env,G[j][x]==0,FFFF[i][q][x][j] == 0));
//                                    mod.add(FFFF[i][q][x][j] >= PR[i][j]-M2*(1-G[j][x])-M2*G[j][wr]);
//                                    mod.add(FFFF[i][q][x][j] <= M2*G[j][x]);
//                                    mod.add(FFFF[i][q][x][j] <= M2*(1-G[j][wr]));
//                                    mod.add(FFFF[i][q][x][j] <= PR[i][j]);
//                                    expr += FFFF[i][q][x][j];
//                                }
//                                else
//                                    mod.add(FFFF[i][q][x][j] == 0);
//                            }
//                        
//                        mod.add(FFF[i][q][x] == expr);
//                        expr.end();
//
//                    }
//            }
//        
//        
//    for(int i=0;i<tasks.size();i++)
//    {
//        IloExpr expr(env);
//        for (int q=0; q<resources.size(); q++)
//            expr += SS[i][q];
//        mod.add(S[i] == expr);
//        expr.end();  
//    }
//    
//    //C12
//    for(int i=0;i<tasks.size();i++)
//    for(int q=0;q<resources.size(); q++)    
//    {
//        IloExpr expr(env);
//        for(int x=0;x<tasks.size();x++)
//            expr += SSS[i][q][x];
//        mod.add(SS[i][q] == expr-M*W[q]);
//        expr.end();  
//    }
//        
//    
//    for(int i=0;i<tasks.size();i++)
//        for(int q=1;q<resources.size();q+=2)
//        {
//            vector <int> Qtasks; 
//            for(int j=0;j<tasks.size();j++)
//                for(int k=0;k<tasks[j].resids.size();k++)
//                    if(tasks[j].resids[k]==resources[q].resID)
//                        Qtasks.push_back(j);
//                int wr=resources[q].writer;
//                
//                IloExpr expr(env);
//                for(int jj=0;jj<Qtasks.size();jj++)
//                {
//                    int j=Qtasks[jj];
//                    mod.add(NAA[i][q][j] >= PL[i][j]-abs(wr-j)*M2*G[wr][j]);
//                    expr+=NAA[i][q][j];
//                }
//                
//                mod.add(NA[i][q] == expr);
//                expr.end();
//
//                
//                for(int xx=0;xx<Qtasks.size();xx++)
//                {
//                    int x=Qtasks[xx];
//                   
//                    float lx;
//                    for(int k=0;k<tasks[x].critSec.size();k++)
//                        if(tasks[x].critSec[k].resID==resources[q].resID)
//                            lx=tasks[x].critSec[k].length;
//                    mod.add(SSS[i][q][x] >= lx*IloMin(PR[i][x],NA[i][q]-FFF[i][q][x])-abs(wr-x)*M*G[wr][x]);
//
//                }
//        }
//        
//        
//        
//    for(int i=0;i<tasks.size();i++)
//        for(int q=1;q<resources.size();q+=2)
//        {
//            vector <int> Qtasks; 
//            for(int j=0;j<tasks.size();j++)
//                for(int k=0;k<tasks[j].resids.size();k++)
//                    if(tasks[j].resids[k]==resources[q].resID)
//                        Qtasks.push_back(j);
//                int wr=resources[q].writer;
//                
//                
//                for(int p=0;p<processors.size();p++)
//                for(int xx=0;xx<Qtasks.size();xx++)
//                if(Qtasks[xx]!=i && wr!=i)    
//                {
//                    int x=Qtasks[xx];
//                   
//                    float lx;
//                    for(int k=0;k<tasks[x].critSec.size();k++)
//                        if(tasks[x].critSec[k].resID==resources[q].resID)
//                            lx=tasks[x].critSec[k].length;
//                    for(int jj=0;jj<Qtasks.size();jj++)
//                    if(Qtasks[jj]!=i)   
//                    {
//                        int j=Qtasks[jj];
//                        
//                        mod.add(BBB[i][q][p] >= lx-M*(1-A[x][p])-M*(1-G[i][j])-M*G[i][x]-M*(1-pi[i][j])-abs(wr-x)*M*G[x][wr]-abs(wr-j)*M*G[j][wr]);
//                        mod.add(BBB[i][q][p] >= lx-M*(1-A[x][p])-M*(1-G[i][x])-M*G[j][x]-M*(1-pi[i][x])-abs(wr-x)*M*G[x][wr]);
//                    }
//                    
//
//                }
//        }
//        
//        
//        for(int i=0;i<tasks.size();i++)
//            for(int q=1;q<resources.size();q+=2)
//            {
//                IloExpr expr(env);
//                for(int k=0;k<processors.size();k++)
//                    expr+=BBB[i][q][k];
//                mod.add(BB[i][q] >= expr-M*W[q]);
//                expr.end();        
//            }
//                
//        
//        
//        for(int i=0;i<tasks.size();i++)
//            for(int q=1;q<resources.size();q+=2)
//            {
//                vector <int> Qtasks; 
//                for(int j=0;j<tasks.size();j++)
//                    for(int k=0;k<tasks[j].resids.size();k++)
//                        if(tasks[j].resids[k]==resources[q].resID)
//                            Qtasks.push_back(j);
//                    
//                int wr=resources[q].writer;
//                for(int xx=0;xx<Qtasks.size();xx++)
//                {
//                    int x=Qtasks[xx];
//                    float lx;
//                    for(int k=0;k<tasks[x].critSec.size();k++)
//                        if(tasks[x].critSec[k].resID==resources[q].resID)
//                            if(wr==x)
//                                lx=tasks[x].critSec[k+1].length;
//                            else
//                                lx=tasks[x].critSec[k].length;
//                        
//                    for(int hh=0;hh<Qtasks.size();hh++)
//                    {
//                        int h=Qtasks[hh];
//                        if(x!=i && x!=h)
//                        {
//                            
//                            mod.add(BB[i][q+1] >= lx-M*(1-G[i][x])-M*(1-G[h][x])-M*(1-pi[i][x])-abs(i-h)*M*(1-pi[h][i])-abs(wr-i)*M*(1-G[wr][i]));
//                            
//                            //mod.add(BB[i][q+1] >= lx-lx*(1-G[i][x])-lx*(1-pi[i][x])-abs(wr-i)*lx*(1-G[wr][i]));
//                        }
//                        
//                    }
//                    
//                }
//
//            }
//        
//        for(int i=0;i<tasks.size();i++)
//        {
//            for (int q = 1; q < resources.size(); q++)
//                mod.add(B[i] >= BB[i][q]);
//
//        }
        
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
         mod.add(expr2==1);
         expr2.end();
//        IloExpr expr(env); 
//        for(int q=1;q<resources.size();q++)
//        {
//           expr += C[q];
//        }
        
        
        
        
        for(int q=1;q<resources.size();q+=2)
            mod.add(W[q]==0);
        IloExpr expr(env);   
        for(int i=0;i<tasks.size();i++)
        {
            expr+=R[i];
        }

    
        mod.add(IloMinimize(env,expr));
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
           
           
           
//           for (int q=1; q<resources.size(); q+=2)
//        {
//            int q1=cplex.getValue(LOC[q]);
//            int q2=cplex.getValue(GLO[q]);
//            cout<<q1<<"\t"<<q2<<endl;
//             
//           
//        }
           
//           for(int r1=1;r1<resources.size();r1+=2)
//            {
//                vector <int> Qtasks1; 
//                for(int j=0;j<tasks.size();j++)
//                    for(int k=0;k<tasks[j].resids.size();k++)
//                        if(tasks[j].resids[k]==resources[r1].resID)
//                            Qtasks1.push_back(j);
//                    int w1=resources[r1].writer;
//                
//                for(int r2=1;r2<resources.size();r2+=2)
//                    if(r2!=r1)
//                    {
//                        vector <int> Qtasks2; 
//                        for(int j=0;j<tasks.size();j++)
//                            for(int k=0;k<tasks[j].resids.size();k++)
//                                if(tasks[j].resids[k]==resources[r2].resID)
//                                    Qtasks2.push_back(j);
//                        int w2=resources[r2].writer;    
//                        
//                        for(int ii=0;ii<Qtasks1.size();ii++)
//                        {
//                            int i=Qtasks1[ii];
//                            for(int jj=0;jj<Qtasks2.size();jj++)
//                               if(Qtasks2[jj]!=i)
//                               {
//                                   int j=Qtasks2[jj];
//                                   int gval=cplex.getValue(GS[i][r1][j][r2]);
//                                   if(i==5 && r1==1)
//                                   //if(gval>0)
//                                   cout<<i<<"-"<<r1<<"-"<<j<<"-"<<r2<<"\t"<<gval<<endl;
//                               }
//                        }
//                    }
//            }
           
//           for(int r1=1;r1<resources.size();r1+=2)
//            {
//                vector <int> Qtasks1; 
//                for(int j=0;j<tasks.size();j++)
//                    for(int k=0;k<tasks[j].resids.size();k++)
//                        if(tasks[j].resids[k]==resources[r1].resID)
//                            Qtasks1.push_back(j);
//                    int w1=resources[r1].writer;
//                
//                for(int r2=1;r2<resources.size();r2+=2)
//                    if(r2!=r1)
//                    {
//                        vector <int> Qtasks2; 
//                        for(int j=0;j<tasks.size();j++)
//                            for(int k=0;k<tasks[j].resids.size();k++)
//                                if(tasks[j].resids[k]==resources[r2].resID)
//                                    Qtasks2.push_back(j);
//                        int w2=resources[r2].writer;    
//                        
//                        for(int ii=0;ii<Qtasks1.size();ii++)
//                        {
//                            int i=Qtasks1[ii];
//                            for(int jj=0;jj<Qtasks2.size();jj++)
//                               if(Qtasks2[jj]!=i)
//                               {
//                                   int j=Qtasks2[jj];
//                                   
//                                   for(int aa=0;aa<Qtasks1.size();aa++)
//                                    if(Qtasks1[aa]!=i)   
//                                    {
//                                        int a=Qtasks1[aa];
//                                        for(int bb=0;bb<Qtasks2.size();bb++)
//                                        if(Qtasks2[bb]!=j)
//                                        {
//                                           int b=Qtasks2[bb]; 
//                                           int gval=cplex.getValue(GGGS[i][r1][j][r2][a][b]);
//                                           if(j==6 && r2==1 && i==2 && r1==7)
//                                           {
//                                           //if(i==5 && r1==1 && j==1 && r2==3)
//                                           cout<<i<<"-"<<r1<<"-"<<j<<"-"<<r2<<"-"<<a<<"-"<<b<<"\t"<<gval;
//                                                   cout<<"\t"<<cplex.getValue(G[i][a])<<"\t"<<cplex.getValue(G[j][b])<<"\t"<<cplex.getValue(pi[a][b])
//                                                           <<"\t"<<cplex.getValue(G[w1][a])<<"\t"<<cplex.getValue(G[w2][b])<<"\t\t"<<w1<<"-"<<w2<<endl;
//                                           }
////+G[j][b]+pi[a][b]expr+=G[w1][a];
//                                        }
//                                        int gval2=cplex.getValue(GGS[i][r1][j][r2][a]);
//                                        
////                                           if(i==5 && r1==1 && j==1 && r2==3)
//                                        //if(j==7 && r2==1 && i==1 && r1==3)
//                                        if(j==6 && r2==1 && i==2 && r1==7)
//                                       {
//                                           cout<<"**"<<i<<"-"<<r1<<"-"<<j<<"-"<<r2<<"-"<<a<<"\t"<<gval2<<endl;
//                                           //cout<<"********"<<i<<"-"<<r1<<"-"<<j<<"-"<<r2<<"\t"<<gval3<<endl;
//                                       }
//                                       
//                                    }
//                                   int gval3=cplex.getValue(GS[i][r1][j][r2]);
//                                    if(j==6 && r2==1 && i==2 && r1==7)
//                                       {
//                                           //cout<<"**"<<i<<"-"<<r1<<"-"<<j<<"-"<<r2<<"-"<<a<<"\t"<<gval2<<endl;
//                                           cout<<"********"<<i<<"-"<<r1<<"-"<<j<<"-"<<r2<<"\t"<<gval3<<endl;
//                                       }
//                                   
//                                    
//                               }
//                        }
//                    }
//            }
           
//           
//            for(int i=1;i<2;i++)
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
//                for(int jj=0;jj<Qtasks1.size();jj++)
//                if(Qtasks1[jj]!=i)    
//                {
//                   
//                    int j=Qtasks1[jj];
//
//                    float lj=0.0;
//                    for(int a=0;a<tasks[j].critSec.size();a++)
//                        if(tasks[j].critSec[a].resID==resources[r1].resID)
//                            lj=tasks[j].critSec[a].length;
//
//                    IloExpr expr(env);
//                    for(int r2=1;r2<resources.size();r2+=2)
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
//                        for(int kk=0;kk<Qtasks2.size();kk++)
//                        if(Qtasks2[kk]!=j && Qtasks2[kk]!=i)
//                        {
//                            
//                            int k=Qtasks2[kk];
//                            
//                            float lx=0.0;
//                            for(int a=0;a<tasks[k].critSec.size();a++)
//                                if(tasks[k].critSec[a].resID==resources[r2].resID)
//                                    lx=tasks[k].critSec[a].length;
//                            
//                            float gval1=cplex.getValue(SSSS1[i][r1][j][k]);
//                             float gval2=cplex.getValue(SSSS2[i][r1][j][k]);
//                             float gval3=cplex.getValue(SSS1[i][r1][j]);
//                             float gval4=cplex.getValue(SSS2[i][r1][j]);
//                             float g5=cplex.getValue(SS[i][r1]);
//                             float g6=cplex.getValue(S[i]);
//                             int i1=cplex.getValue(OB[i][r1][j]);
//                             float g7=cplex.getValue(OB[i][r1][j]*lj-M2*pi[i][j]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]);
//                                          
//                                           //if(i==5 && r1==1 && j==1 && r2==3)
//                                           
//                             if(gval3>0 || gval4>0)
//                             
//                                   
//                                   debug9<<i<<"-"<<r1<<"-"<<j<<"-"<<k<<"\t"<<gval1<<"\t"<<gval2
//                                    <<"\t\t"<<gval3<<"\t"<<gval4<<"-----"<<g5<<endl;
////                                           <<"\t"<<i1<<"-"<<lj<<"-"<<g7<<endl;
//                            
//                             
//                            
//                          
////                          mod.add(SSSS1[i][r1][j][k] >= lx-M2*(1-pi[i][j])-M2*(1-G[j][k])-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]);
////                          mod.add(SSSS2[i][r1][j][k] >= OB[i][r1][j]*lx-M2*pi[i][j]-M2*(1-G[j][k])-M2*GS[j][r1][k][r2]-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]-abs(w2-k)*M2*G[k][w2]);
////            
////                          mod.add(SSS1[i][r1][j] >= SSSS1[i][r1][j][k]+lj-M2*(1-pi[i][j])-abs(w1-i)*M2*G[i][w1]-abs(w1-j)*M2*G[j][w1]);
////                          expr+=SSSS2[i][r1][j][k];
//                          
//                        }
//                           
//
//                    }
//                    
// 
//                }
//               
//        }
           
//           for(int q=1;q<resources.size();q+=2)
//           {
//               int type1=cplex.getValue(LOC[q]);
//               int type2=cplex.getValue(GLO[q]);
//               //int rcost=cplex.getValue(C[q]);
//               cout<<type1<<"   "<<type2<<endl;
//               
//           }
           
//           int p1=cplex.getValue(pi[1][7]);
//           int p2=cplex.getValue(pi[7][1]);
//           cout<<"**"<<p1<<"  "<<p2<<endl; 

//           for(int i=0;i<tasks.size();i++)
//           {
//               int cg;
//                for(int j=0;j<tasks.size();j++)
//                {
//                
//                    cg=cplex.getValue(pi[i][j]);
//                    cout<<cg<<"\t";
//                }
//               cout<<endl;
//                    
//           }
           
//           float a1,a2=0,a3=0;
//           a1=cplex.getValue(SS[1][3]);
//           a2=cplex.getValue(S[1]);
//           a3=cplex.getValue(OB[2][7][5]);
//           
//           cout<<a1<<"    "<<a2<<"    "<<a3<<endl;
//           
//           for(int i=1;i<2;i++)
//        {
//            IloExpr expr2(env);
//            for(int r1=1;r1<resources.size();r1+=2)
//            if(tasks[i].resids.size()>0)  
//            if(find(tasks[i].resids.begin(),tasks[i].resids.end(),r1)!=tasks[i].resids.end())
//            {
//                vector <int> Qtasks1; 
//                for(int j=0;j<tasks.size();j++)
//                    for(int k=0;k<tasks[j].resids.size();k++)
//                        if(tasks[j].resids[k]==resources[r1].resID)
//                            Qtasks1.push_back(j);
//                    IloExpr expr(env);
//
//                    for(int jj=0;jj<Qtasks1.size();jj++)
//                    if(Qtasks1[jj]!=i)    
//                    {
//                        int j=Qtasks1[jj];
//                        float a1,a2=0;
//                        a1=cplex.getValue(SSS1[i][r1][j]);
//                        a2=cplex.getValue(SSS2[i][r1][j]);
//                        cout<<i<<"-"<<r1<<"-"<<j<<"\t"<<a1<<"\t"<<a2<<endl;
//           
//                    }
//               
//            }
//           
//        }
           
//           for(int a=0;a<tasks.size();a++)
//               cout<<a<<"-"<<cplex.getValue(GGS[9][1][4][3][a])<<endl;
           
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

for(int i=0;i<tasks.size();i++)
        {
		for(int j=0;j<tasks.size();j++)
		{
			int rr=cplex.getValue(pi[i][j]);
                        cout<<rr<<"\t";
		  	//cout<<rr<"\t";
		}
            cout<<endl;
        }
            

        
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
