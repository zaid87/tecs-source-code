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
    
    

IloEnv env;
    try {
        IloModel       mod(env);

        typedef IloArray<IloNumVarArray> NumVarMatrix;
        typedef IloArray<NumVarMatrix>   NumVar3Matrix;
        typedef IloArray<NumVar3Matrix>   NumVar4Matrix;

        typedef IloArray<IloIntVarArray> IntVarMatrix;
    
        float tempm=0;
        for(int x=0;x<tasks.size();x++)
            for(int q=0;q<tasks[x].critSec.size();q++)
                tempm=max(tempm,tasks[x].critSec[q].length);
        const float M=100*tempm*tasks.size();
        
        const int M2=100;


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
        NumVar3Matrix SSS(env,tasks.size()); //must be the same size as S
        NumVarMatrix NA(env,tasks.size());   //must be the same size as S
        NumVar3Matrix NAA(env,tasks.size());  
        
         IloNumVarArray B(env,tasks.size());
        NumVarMatrix BB(env,tasks.size());   //must be the same size as S
        NumVar3Matrix BBB(env,tasks.size());
        
        IloNumVarArray W(env,resources.size());
        IloNumVarArray C(env,resources.size());
        
        
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
            SSS[i]=  NumVarMatrix(env, resources.size());    //must be same size as SS
            
            B[i]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            BB[i]=  IloNumVarArray(env, resources.size());
            BBB[i]=  NumVarMatrix(env, resources.size());
            
            NA[i]=  IloNumVarArray(env, resources.size());
            NAA[i]=  NumVarMatrix(env, resources.size());    //must be same size as SS
            
            for(int q=0;q<resources.size();q++)
            {
                FFF[i][q]=  IloNumVarArray(env, tasks.size());
                FFFF[i][q]=  NumVarMatrix(env, tasks.size());
                
                SS[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                SSS[i][q]=  IloNumVarArray(env, tasks.size());
                
                BB[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                BBB[i][q]=  IloNumVarArray(env, processors.size());
                
                NA[i][q]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                NAA[i][q]=  IloNumVarArray(env, tasks.size());
                
                for(int k=0;k<tasks.size();k++)
                {
                    FFF[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    FFFF[i][q][k]=IloNumVarArray(env, tasks.size());
                    for(int j=0;j<tasks.size();j++)
                            FFFF[i][q][k][j]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                    
                    SSS[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
                   
                    NAA[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
                }
                
                for(int k=0;k<processors.size();k++)
                     BBB[i][q][k]=IloNumVar(env, 0.0, IloInfinity, ILOFLOAT);
            }
          
            

            for(int j=0; j<tasks.size();j++)
            {
                pi[i][j]= IloNumVar(env, 0.0, 1.0, ILOINT);
                G[i][j]= IloNumVar(env, 0.0, 1.0, ILOINT);
                PL[i][j]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                PR[i][j]= IloNumVar(env, 0.0, IloInfinity, ILOINT);
                
            }

            
            for(int p=0;p<processors.size();p++)
                A[i][p]=IloNumVar(env, 0.0, 1.0, ILOINT);

         }
        
        for(int q=0;q<resources.size();q++)
        {
             W[q]=IloNumVar(env, 0.0, 1.0, ILOINT);
             C[q]=IloNumVar(env, 0.0, IloInfinity, ILOINT);
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
        for(int i=0;i<tasks.size();i++)
        {
            mod.add(PL[i][i]==1);
            for(int x=0;x<tasks.size();x++)
                if(x!=i)
                {
                    //mod.add(NP[i][x]>=0);
                    int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
                    mod.add(PL[i][x]>= (R[i]/float(tasks[x].t))-M2*(1-G[i][x])-M2*pi[i][x]);
                    mod.add(PL[i][x]<= 1-0.000001+(R[i]/float(tasks[x].t)));
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
            expr+=tasks[i].c+S[i]+B[i]; 
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
            for(int q=1;q<resources.size();q+=2)
            {
                vector <int> Qtasks; 
                for(int j=0;j<tasks.size();j++)
                    for(int k=0;k<tasks[j].resids.size();k++)
                        if(tasks[j].resids[k]==resources[q].resID)
                            Qtasks.push_back(j);
                    int wr=resources[q].writer;
                    
                    for(int xx=0;xx<Qtasks.size();xx++)
                    if(Qtasks[xx]!=i)    
                    {
                        int x=Qtasks[xx];
                        IloExpr expr(env);
                        
                        for(int jj=0;jj<Qtasks.size();jj++)
                            if(Qtasks[jj]!=x && Qtasks[jj]!=i)
                            {
                                int j=Qtasks[jj];
                                float lj=0.0,lx=0.0;
                                for(int k=0;k<tasks[j].critSec.size();k++)
                                    if(tasks[j].critSec[k].resID==resources[q].resID)
                                        lj=tasks[j].critSec[k].length;
                                
                                for(int k=0;k<tasks[x].critSec.size();k++)
                                    if(tasks[x].critSec[k].resID==resources[q].resID)
                                        lx=tasks[x].critSec[k].length;
                                
                                if(lx<lj)
                                {
                                    //test<<i<<" "<<q<<" "<<x<<" "<<j<<endl;-abs(wr-x)*M*G[wr][x]
//                                    mod.add(IloIfThen(env,G[j][x]==1,FFFF[i][q][x][j] == PR[i][j]));
//                                    mod.add(IloIfThen(env,G[j][x]==0,FFFF[i][q][x][j] == 0));
                                    mod.add(FFFF[i][q][x][j] >= PR[i][j]-M2*(1-G[j][x])-M2*G[j][wr]);
                                    mod.add(FFFF[i][q][x][j] <= M2*G[j][x]);
                                    mod.add(FFFF[i][q][x][j] <= M2*(1-G[j][wr]));
                                    mod.add(FFFF[i][q][x][j] <= PR[i][j]);
                                    expr += FFFF[i][q][x][j];
                                }
                                else
                                    mod.add(FFFF[i][q][x][j] == 0);
                            }
                        
                        mod.add(FFF[i][q][x] == expr);
                        expr.end();

                    }
            }
        
        
    for(int i=0;i<tasks.size();i++)
    {
        IloExpr expr(env);
        for (int q=0; q<resources.size(); q++)
            expr += SS[i][q];
        mod.add(S[i] == expr);
        expr.end();  
    }
    
    //C12
    for(int i=0;i<tasks.size();i++)
    for(int q=0;q<resources.size(); q++)    
    {
        IloExpr expr(env);
        for(int x=0;x<tasks.size();x++)
            expr += SSS[i][q][x];
        mod.add(SS[i][q] == expr-M*W[q]);
        expr.end();  
    }
        
    
    for(int i=0;i<tasks.size();i++)
        for(int q=1;q<resources.size();q+=2)
        {
            vector <int> Qtasks; 
            for(int j=0;j<tasks.size();j++)
                for(int k=0;k<tasks[j].resids.size();k++)
                    if(tasks[j].resids[k]==resources[q].resID)
                        Qtasks.push_back(j);
                int wr=resources[q].writer;
                
                IloExpr expr(env);
                for(int jj=0;jj<Qtasks.size();jj++)
                {
                    int j=Qtasks[jj];
                    mod.add(NAA[i][q][j] >= PL[i][j]-abs(wr-j)*M2*G[wr][j]);
                    expr+=NAA[i][q][j];
                }
                
                mod.add(NA[i][q] == expr);
                expr.end();

                
                for(int xx=0;xx<Qtasks.size();xx++)
                {
                    int x=Qtasks[xx];
                   
                    float lx;
                    for(int k=0;k<tasks[x].critSec.size();k++)
                        if(tasks[x].critSec[k].resID==resources[q].resID)
                            lx=tasks[x].critSec[k].length;
                    mod.add(SSS[i][q][x] >= lx*IloMin(PR[i][x],NA[i][q]-FFF[i][q][x])-abs(wr-x)*M*G[wr][x]);

                }
        }
        
        
        
    for(int i=0;i<tasks.size();i++)
        for(int q=1;q<resources.size();q+=2)
        {
            vector <int> Qtasks; 
            for(int j=0;j<tasks.size();j++)
                for(int k=0;k<tasks[j].resids.size();k++)
                    if(tasks[j].resids[k]==resources[q].resID)
                        Qtasks.push_back(j);
                int wr=resources[q].writer;
                
                
                for(int p=0;p<processors.size();p++)
                for(int xx=0;xx<Qtasks.size();xx++)
                if(Qtasks[xx]!=i && wr!=i)    
                {
                    int x=Qtasks[xx];
                   
                    float lx;
                    for(int k=0;k<tasks[x].critSec.size();k++)
                        if(tasks[x].critSec[k].resID==resources[q].resID)
                            lx=tasks[x].critSec[k].length;
                    for(int jj=0;jj<Qtasks.size();jj++)
                    if(Qtasks[jj]!=i)   
                    {
                        int j=Qtasks[jj];
                        
                        mod.add(BBB[i][q][p] >= lx-M*(1-A[x][p])-M*(1-G[i][j])-M*G[i][x]-M*(1-pi[i][j])-abs(wr-x)*M*G[x][wr]-abs(wr-j)*M*G[j][wr]);
                        mod.add(BBB[i][q][p] >= lx-M*(1-A[x][p])-M*(1-G[i][x])-M*G[j][x]-M*(1-pi[i][x])-abs(wr-x)*M*G[x][wr]);
                    }
                    

                }
        }
        
        
        for(int i=0;i<tasks.size();i++)
            for(int q=1;q<resources.size();q+=2)
            {
                IloExpr expr(env);
                for(int k=0;k<processors.size();k++)
                    expr+=BBB[i][q][k];
                mod.add(BB[i][q] >= expr-M*W[q]);
                expr.end();        
            }
                
        
        
        for(int i=0;i<tasks.size();i++)
            for(int q=1;q<resources.size();q+=2)
            {
                vector <int> Qtasks; 
                for(int j=0;j<tasks.size();j++)
                    for(int k=0;k<tasks[j].resids.size();k++)
                        if(tasks[j].resids[k]==resources[q].resID)
                            Qtasks.push_back(j);
                    
                int wr=resources[q].writer;
                for(int xx=0;xx<Qtasks.size();xx++)
                {
                    int x=Qtasks[xx];
                    float lx;
                    for(int k=0;k<tasks[x].critSec.size();k++)
                        if(tasks[x].critSec[k].resID==resources[q].resID)
                            if(wr==x)
                                lx=tasks[x].critSec[k+1].length;
                            else
                                lx=tasks[x].critSec[k].length;
                        
                    for(int hh=0;hh<Qtasks.size();hh++)
                    {
                        int h=Qtasks[hh];
                        if(x!=i && x!=h)
                        {
                            
                            mod.add(BB[i][q+1] >= lx-M*(1-G[i][x])-M*(1-G[h][x])-M*(1-pi[i][x])-abs(i-h)*M*(1-pi[h][i])-abs(wr-i)*M*(1-G[wr][i]));
                            
                            //mod.add(BB[i][q+1] >= lx-lx*(1-G[i][x])-lx*(1-pi[i][x])-abs(wr-i)*lx*(1-G[wr][i]));
                        }
                        
                    }
                    
                }

            }
        
        for(int i=0;i<tasks.size();i++)
        {
            for (int q = 1; q < resources.size(); q++)
                mod.add(B[i] >= BB[i][q]);

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
        
//       IloExpr expr2(env);
//        for(int q=1;q<resources.size();q+=2)
//        {
//           expr2+=W[q];
//        }
        // mod.add(expr2<=1);
        // expr2.end();
        IloExpr expr(env); 
        for(int q=1;q<resources.size();q++)
        {
           expr += C[q];
        }
        
//        IloExpr expr(env);   
//        for(int i=0;i<tasks.size();i++)
//        {
//            expr+=R[i];
//        }
    
        mod.add(IloMinimize(env,expr));
        expr.end();    
  
        IloCplex cplex(mod);
        cplex.exportModel("m0.lp");
        cplex.setOut(env.getNullStream());
        //cplex.setParam(IloCplex::Param::MIP::Strategy::NodeSelect, 2);
           //cplex.setParam(IloCplex::Param::Emphasis::Memory, 1);
        cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, 0);
cplex.setParam(IloCplex::Param::TimeLimit, 7200);
cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 12000);
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

        
        
//    for(int i=0;i<tasks.size();i++)
//    {
//        mod.add(NP[i][i]==1);
//        for(int x=0;x<tasks.size();x++)
//            if(x!=i)
//            {
//                mod.add(NP[i][x]>=0);
//                int ceilval=ceil(float(tasks[i].t)/float(tasks[x].t));
//                mod.add(NP[i][x]<=ceilval);
//            }
//    }
        
        //        for(int i=0;i<tasks.size();i++)
//            for(int k=0;k<tasks.size();k++)
//                if(k!=i)
//                {
//                    
//                    //mod.add( NP[i][k]-R[i]/float(tasks[k].t) >=0);
//                    //mod.add(NP[i][k]-R[i]/float(tasks[k].t) <=0.99999);
//                    mod.add(NP2[i][k] >= (R[i]/(float(tasks[k].t))));
//                    mod.add(NP2[i][k] <= (R[i]/(float(tasks[k].t)))+0.99999);
//                    mod.add(NP2[i][k] <= ceil(float(tasks[i].t)/(float(tasks[k].t)))*(G[i][k]+pi[k][i]-1));
//                }

//for(int i=0;i<tasks.size();i++)
//        for(int q=1;q<resources.size();q+=2)
//        {
//            vector <int> Qtasks; 
//            for(int j=0;j<tasks.size();j++)
//                for(int k=0;k<tasks[j].resids.size();k++)
//                    if(tasks[j].resids[k]==resources[q].resID)
//                        Qtasks.push_back(j);
//                int wr=resources[q].writer;
//
//                for(int xx=0;xx<Qtasks.size();xx++)
//                {
//                    int x=Qtasks[xx];
//                    IloExpr expr(env);
//                    float lx;
//                    for(int k=0;k<tasks[x].critSec.size();k++)
//                        if(tasks[x].critSec[k].resID==resources[q].resID)
//                            lx=tasks[x].critSec[k].length;
//                    
//                     for(int jj=0;jj<Qtasks.size();jj++)
//                        if(Qtasks[jj]!=x)
//                        {
//                            if(Qtasks[jj]!=i)
//                           {
//                                if(wr!=i){
//                                int j=Qtasks[jj];
//                               expr+=PL[i][j];
//                               }
//                           }
//                            else
//                                 expr+=1;
//                        }
//                        
//                   
//                    expr-=FFF[i][q][x];
//                         
//                    
//
//                    mod.add(SSS[i][q][x] >= lx*IloMin(PR[i][x],expr)-abs(wr-x)*M*G[wr][x]);
//                    //-abs(wr-i)*M*G[wr][i]
//                    expr.end();
//
//                }
//        }    

//            for(int i=0;i<tasks.size();i++)
//            for(int q=1;q<resources.size();q+=2)
//           {
//                vector <int> Qtasks; 
//                for(int j=0;j<tasks.size();j++)
//                    for(int k=0;k<tasks[j].resids.size();k++)
//                        if(tasks[j].resids[k]==resources[q].resID)
//                            Qtasks.push_back(j);
////                    
////                    cout<<q<<"includes ";
////                for(int h=0;h<Qtasks.size();h++)
////                    cout<<Qtasks[h]<<" ";
////                    cout<<endl;
//                
//                int noth,n2;
//                for(int xx=0;xx<Qtasks.size();xx++)
//                if(Qtasks[xx]!=i)
////                for(int jj=0;jj<Qtasks.size();jj++)
////                if(Qtasks[jj]!=i && Qtasks[jj]!=Qtasks[xx]) 
//               {
//                    //int j=Qtasks[jj];
//                    int x=Qtasks[xx];
//                    
//                        float x3= cplex.getValue(SSS[i][q][x]);
////                        noth=cplex.getValue(G[j][x]);
////                        n2=cplex.getValue(PR[i][j]);
//                        if(x3>0)
//                        cout<<i<<"-"<<q<<"-"<<x<<"-"<<"     "<<x3<<"     "<<noth<<"   "<<n2<<endl;
////                        float y3= cplex.getValue(FFF[i][q][x]);
////                        if(y3>0)
////                        cout<<i<<"-"<<q<<"-"<<x<<"     "<<y3<<endl;
//                }
//           }
            
            
//             for(int i=0;i<tasks.size();i++)
//                for(int q=1;q<resources.size();q+=2)
//                    for(int k=0;k<processors.size();k++)
//                    
//                {
//                    float x0=cplex.getValue(BBB[i][q][k]);
//                    cout<<i<<q<<k<<"     "<<x0<<endl;
//                }
          
//          for(int i=0;i<tasks.size();i++)
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
//                   
//                    for(int jj=0;jj<Qtasks.size();jj++)
//                    if(Qtasks[jj]!=i)   
//                    {
//                        int j=Qtasks[jj];
//                        
//                        float x0=cplex.getValue(BBB[i][q][p]);
//                        cout<<i<<q<<p<<"     "<<x0<<endl;
//                    }
//                    
//
//                }
//        }
           
            
//            for(int i=0;i<tasks.size();i++)
//            {
//                for(int j=0;j<tasks.size();j++)
//                    cout<<i<<"-"<<j<<" "<<cplex.getValue(PL[i][j])<<"\t";
//                cout<<endl;
//            }
//            
//            for(int i=0;i<tasks.size();i++)
//                for(int q=1;q<resources.size();q+=2)
//                {
//                    
//                    int x4= cplex.getValue(NA[i][q]);
//                     test<<i<<"-"<<q<<"     "<<x4<<endl;
////                    vector <int> Qtasks; 
////                for(int j=0;j<tasks.size();j++)
////                    for(int k=0;k<tasks[j].resids.size();k++)
////                        if(tasks[j].resids[k]==resources[q].resID)
////                            Qtasks.push_back(j);
////                    int wr=resources[q].writer;
//                    
////                    for(int jj=0;jj<Qtasks.size();jj++)
////                    {
////                        int j=Qtasks[jj];
////                        int x6=cplex.getValue(PL[i][j]);
////                        int x5=cplex.getValue(G[wr][j]);
////                     int x4= cplex.getValue(NAA[i][q][j]);
////                     test<<i<<"-"<<q<<"-"<<j<<"     "<<x4<<"     "<<x5<<"     "<<x6<<endl;
////                     //test<<i<<"    "<<x4<<endl;
////                    }
//                }
            
//            for(int i=0;i<tasks.size();i++)
//           {
//           for(int k=0;k<tasks.size();k++)
//           if(k!=i)    
//           {
//                   x2= cplex.getValue(PR[i][k]);
//                   cout<<i<<"-"<<k<<"\t"<<x2<<endl;
//           }
//           
//           }
// cout<<"-----------------------"<<endl;

//            for(int i=1;i<2;i++)
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
//                int nn=cplex.getValue(NA[i][q]);
//               
//                
//                for(int xx=0;xx<Qtasks.size();xx++)
//                {
//                   int x=Qtasks[xx];
//                   float f=cplex.getValue(FFF[i][q][x]);
//                   float p=cplex.getValue(PR[i][x]);
//                   float s=cplex.getValue(SSS[i][q][x]);
//                   //if(s>0)
//                   cout<<i<<"-"<<q<<"-"<<x<<"\t"<<p<<"\t"<<nn<<"\t"<<f<<"\t"<<s<<endl;
//                  // mod.add( >= lx*IloMin(PR[i][x],NA[i][q]-FFF[i][q][x])-abs(wr-x)*M*G[wr][x]);
//
//                }
//        }
            