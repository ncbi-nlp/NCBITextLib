#ifndef HUBR_H
#define HUBR_H

#include <iostream>
#include <fstream>
#include "CMark.h"

using namespace std;
namespace iret {

// Template class HubeX: SVM classifiers (PROBE for learning)
template<class Y,class Z>
class HubeX : public CMark<Y,Z> {
public:
   HubeX(const char *nspost,const char *pnam);
      // nspost is XPost object name
      // pnam is used in "path_pnam" where to find path or
      // if ':' is first char then remainder is the path
   ~HubeX(void);

   void gopen_HubeX(void); // Opens for operating
   void gclose_HubeX(void);

   // Learning functions
   void Set_Docs(Indx<Y> *gdd,Indx<Y> *bdd);
      // Set cls array. gdd: positive docs, bdd: negative docs
   void Set_Lambda_Norm(Z lam); // Set lambda

   void Learn(void);
      // Optimizer
      // im is improvement threshold and iter the iteration limit
   Z *ScoreSet(Indx<Y> *ind);
      // Must call gopen_db_map() of XPost first
   Z *ScoreAll(void);
      // Must call gopen_map() of XPost first

   // Same as above but use local weights
   void Set_Lambda_Norm_lw(Z lam);
   void Learn_lw(void); 
      // Uses local weights for learning
   Z *ScoreSet_lw(Indx<Y> *ind);
      // Must call gopen_db_map() of XPost first
   Z *ScoreAll_lw(void);
      // Must call gopen_map() of XPost first 

   void Save(int n);    // Saves the set of weights wt. Marks with n
   void Load(int n);    // Loads the set of weights wt marked with n
   void Release(int n); // Unmaps the weight vector mapped by load

   // Relate to documents
   Y tdoc; // Number of training documents 
   Y *cls; // Marks the class with 1 or -1

   // Relate to term processing
   Z lambda; // Lambda
   Z *wt;    // Weights of the process
   Z *wd;    // Derivative factor 
   Z th;     // Threshhold
   Y *sr;    // Maps the tdoc training set
   Z *sco;   // Document scores
};

template<class Y,class Z>
HubeX<Y,Z>::HubeX(const char *namspost,const char *pnam) : CMark<Y,Z>(namspost,pnam) {
   wt=NULL;
   wd=NULL;
   sr=NULL;
   sco=NULL;
   cls=NULL;
}

template<class Y,class Z>
HubeX<Y,Z>::~HubeX() {
   if(wt!=NULL)delete [] wt;
   if(wd!=NULL)delete [] wd;
   if(sr!=NULL)delete [] sr;
   if(sco!=NULL)delete [] sco;
   if(cls!=NULL)delete [] cls;
}

template<class Y,class Z>
void HubeX<Y,Z>::gopen_HubeX(void) {
   this->gopen_db_map();
   this->init_cnt();
}

template<class Y,class Z>
void HubeX<Y,Z>::gclose_HubeX(void) {
   this->gclose_db_map();
}

template<class Y,class Z>
void HubeX<Y,Z>::Set_Docs(Indx<Y> *gdd,Indx<Y> *bdd) {
   Y i;

   if(cls!=NULL)delete [] cls;
   cls=new Y[this->ndoc];
   for(i=0;i<this->ndoc;i++)cls[i]=0;
   for(i=0;i<gdd->ix;i++){
      cls[gdd->idx[i]]=1;
   }
   for(i=0;i<bdd->ix;i++){
      cls[bdd->idx[i]]=-1;
   }
   tdoc=gdd->ix+bdd->ix;
}

template<class Y,class Z>
void HubeX<Y,Z>::Set_Lambda_Norm(Z lam) {
   Y i,j,k; 
   Z sum=0,xx,yy,zz,cnx;
         
   this->gopen_db_map();
   for(i=0;i<this->ndoc;i++) {
      if(cls[i]) {
         this->readp_db(i);
         xx=0;
         for(j=0;j<this->nw;j++) {
            if(this->mrk[this->nwd[j]]) {
               xx++;
            }
         }
         sum+=sqrt(xx);
      }
   }
   sum/=(Z)tdoc;
   lambda=lam*sum*sum;
   if(this->pflag)cout << "lambda= " << lambda << endl;
   lambda*=(Z)tdoc;
}

template<class Y,class Z>
void HubeX<Y,Z>::Learn(void) {
   Y i,j,k,n,sc,iz,iu,rp,ix;
   Y flag,mode,ct,gt,cu;
   Z xx,yy,cv,dv,dw,wx,blk;
   Z sum,zz,uu,del,grad,eta;
   Z dth,f,of,xrch,xvt,ux,uy;
   Z fac,minf,rch,rfac;
   Z ss,dl,tho,*wto,llm;
   Z efu,efd,ominf,xfac;

   Store<Z> St(this->nwrd+1);
   this->gopen_db_map();
   if(wt!=NULL)delete [] wt;
   if(wd!=NULL)delete [] wd;
   if(sr!=NULL)delete [] sr;
   wt=new Z[this->nwrd+1];
   wd=new Z[this->nwrd];
   sr=new Y[tdoc];

   th=0;
   for(i=0;i<this->nwrd;i++) {
      wt[i]=0;
   }

   n=0;
   for(i=0;i<this->ndoc;i++) {
      if(cls[i]){
         sr[n++]=i;
      }
   }
   llm=0;
   of=ominf=minf=1.0E100;
   fac=2.0/3.0;
   xfac=2.0/3.0;
   rfac=3.0/2.0;
   mode=0;
   efu=0;

   gt=ct=cu=0;
   flag=1;
 
 while(flag){
   gt++;ct++;
   // Deal with regularizer
   f=th*th;
   for(i=0;i<this->nwrd;i++) {
      wd[i]=lambda*(xx=wt[i]);
      f+=xx*xx;
   }
   f*=0.5*lambda;
   dth=lambda*th;

   // Deal with main loss function
   for(i=0;i<tdoc;i++) {
      j=sr[i];
      yy=cls[j];
      this->readp_db(j);
      sum=th;
      for(iz=0;iz<this->nw;iz++) {
         if(this->mrk[iu=this->nwd[iz]]) {
            sum+=wt[iu];
         }
      }
      // Loss zz calculation from p=sum
      xx=yy*sum;
      if(xx<1.0) {
         if(xx>-1.0) {
            zz=1.0-xx;
            uu=-2.0*yy*zz;
            f+=zz*zz;
         }
         else {
            f+=-4.0*xx;
            uu=-4.0*yy;
         }
         dth+=uu;
         for(iz=0;iz<this->nw;iz++) {
            if(this->mrk[iu=this->nwd[iz]]) {
               wd[iu]+=uu;
            }
         }
      }
   }
   // Track the minimum f seen
   if(f>of)cu++;
   if(f<minf){minf=f;wt[this->nwrd]=th;St.CopyIn(wt);}
   if(cu==2) {
      switch(mode) {
         case 0: cu=ct=0;
                 if(minf==ominf) {
                    mode=1;
                    fac*=xfac;
                 }
                 else ominf=minf;
                 break;
         case 1: efd=(ominf-minf)/ct;
                 ominf=minf;
                 mode=2;
                 fac*=rfac;
                 cu=ct=0;
                 break;
         case 2: efu=(ominf-minf)/ct;
                 ominf=minf;
                 cu=ct=0;
                 if(efd>=efu) {
                    fac*=xfac;
                 }
                 mode=0;
      }
   }
   of=f;     

   del=f-minf+fac*minf;

   if(fac<0.05)break;
   grad=dth*dth;
   for(i=0;i<this->nwrd;i++)grad+=wd[i]*wd[i];
   eta=del/grad;
   th-=eta*dth;
   for(i=0;i<this->nwrd;i++)wt[i]-=eta*wd[i];

   if(this->pflag)cout << endl << gt << "  " << cu << " mode " << mode << endl;
   if(this->pflag)cout << "    f " << f << " minf " << minf << " fac " << fac << endl;
   if(this->pflag)cout << " grad " << grad << " eta " << eta << endl; 
 }

   St.CopyOut(wt);
   th=wt[this->nwrd];
   delete [] wd;
   wd=NULL;
   delete [] sr;
   sr=NULL;
   if(this->pflag)cout << "Minf " << minf << endl;
}

template<class Y,class Z>
Z *HubeX<Y,Z>::ScoreSet(Indx<Y> *ind) {
   Y i,n;
   Z sum;

   if(sco!=NULL)delete [] sco;
   sco=new Z[ind->ix];

   for(i=0;i<ind->ix;i++) {
      this->readp_db(ind->idx[i]);
      sum=th;
      for(n=0;n<this->nw;n++)sum+=wt[this->nwd[n]];
      sco[i]=sum;
      mark(i+1,100,"docs scored");
   }
   return(sco);
}

template<class Y,class Z>
Z *HubeX<Y,Z>::ScoreAll(void) {
   Y j,n;
   Z sum;
   
   if(sco!=NULL)delete [] sco;
   sco=new Z[this->ndoc];
      
   for(j=0;j<this->ndoc;j++) {
      this->readp_db(j);
      sum=th;
      for(n=0;n<this->nw;n++)sum+=wt[this->nwd[n]];
      sco[j]=sum;
      mark(j+1,100,"docs scored");
   }
   return(sco);
}  

template<class Y,class Z>
void HubeX<Y,Z>::Set_Lambda_Norm_lw(Z lam) {
   Y i,j,k;
   Z sum=0,xx,yy,zz,cnx;

   this->gopen_db_map();
   for(i=0;i<this->ndoc;i++){
      if(cls[i]){
         this->readp_db(i);
         this->readz_db(i);
         xx=0;
         for(j=0;j<this->nw;j++){
            if(this->mrk[this->nwd[j]]) {
               xx+=this->lwt[j]*this->lwt[j];
            }
         }
         sum+=sqrt(xx);
      }
   }
   sum/=(Z)tdoc;
   lambda=lam*sum*sum;
   if(this->pflag)cout << "lambda= " << lambda << endl;
   lambda*=(Z)tdoc;
}

template<class Y,class Z>
void HubeX<Y,Z>::Learn_lw(void) {
   Y i,j,k,n,sc,iz,iu,rp,ix;
   Y flag,flag2,ct,gt,cu,mode;
   Z xx,yy,cv,dv,dw,wx,blk;
   Z sum,zz,uu,del,grad,eta;
   Z dth,f,of,xrch,*wn,ux,uy;
   Z fac,minf,rfac,llm;
   Z efu,efd,ominf,xfac;

   Store<Z> St(this->nwrd+1);
   this->gopen_db_map();
   if(wt!=NULL)delete [] wt;
   if(wd!=NULL)delete [] wd;
   if(sr!=NULL)delete [] sr;
   wt=new Z[this->nwrd+1];
   wd=new Z[this->nwrd];
   sr=new Y[tdoc];

   th=0;
   for(i=0;i<this->nwrd;i++)wt[i]=0;

   n=0;
   for(i=0;i<this->ndoc;i++) {
      if(cls[i]) {
         sr[n++]=i;
      }
   }

   llm=0;
   of=ominf=minf=1.0E100;
   fac=2.0/3.0;
   xfac=2.0/3.0;
   rfac=3.0/2.0;
   mode=0;
   efu=0;

   gt=ct=cu=0;
   flag=1;
 
 while(flag) {
   gt++;ct++;
   // Deal with regularizer
   f=th*th;
   for(i=0;i<this->nwrd;i++) {
      wd[i]=lambda*(xx=wt[i]);
      f+=xx*xx;
   }
   f*=0.5*lambda;
   dth=lambda*th;

   // Deal with main loss function
   for(i=0;i<tdoc;i++) {
      j=sr[i];
      yy=cls[j];
      this->readp_db(j);
      this->readz_db(j);
      sum=th;
      for(iz=0;iz<this->nw;iz++) {
         if(this->mrk[iu=this->nwd[iz]]) {
            sum+=wt[iu]*this->lwt[iz];
         }
      }
      // Loss zz calculation from p=sum
      xx=yy*sum;
      if(xx<1.0) {
         if(xx>-1.0) {
            zz=1.0-xx;
            uu=-2.0*yy*zz;
            f+=zz*zz;
         }
         else {
            f+=-4.0*xx;
            uu=-4.0*yy;
         }
         dth+=uu;
         for(iz=0;iz<this->nw;iz++) {
            if(this->mrk[iu=this->nwd[iz]]) {
               wd[iu]+=uu*this->lwt[iz];
            }
         }
      }
   }
   // Track the minimum f seen
   if(f>of)cu++;
   if(f<minf){minf=f;wt[this->nwrd]=th;St.CopyIn(wt);}
   if(cu==2) {
      switch(mode) {
         case 0: cu=ct=0;
                 if(minf==ominf) {
                    mode=1;
                    fac*=xfac;
                 }
                 else ominf=minf;
                 break;
         case 1: efd=(ominf-minf)/ct;
                 ominf=minf;
                 mode=2;
                 fac*=rfac;
                 cu=ct=0;
                 break;
         case 2: efu=(ominf-minf)/ct;
                 ominf=minf;
                 cu=ct=0;
                 if(efd>=efu) {
                    fac*=xfac;
                 }
                 mode=0;
      }
   }
   of=f;

   del=f-minf+fac*minf;

   if(fac<0.05)break;
   grad=dth*dth;
   for(i=0;i<this->nwrd;i++)grad+=wd[i]*wd[i];
   eta=del/grad;
   th-=eta*dth;
   for(i=0;i<this->nwrd;i++)wt[i]-=eta*wd[i];

   if(this->pflag)cout << endl << gt << "  " << cu << " mode " << mode << endl;
   if(this->pflag)cout << "    f " << f << " minf " << minf << " fac " << fac << endl;
   if(this->pflag)cout << " grad " << grad << " eta " << eta << endl;
 }

   St.CopyOut(wt);
   th=wt[this->nwrd];
   delete [] wd;
   wd=NULL;
   delete [] sr;
   sr=NULL;
   if(this->pflag)cout << "Minf " << minf << endl;
}

template<class Y,class Z>
Z *HubeX<Y,Z>::ScoreSet_lw(Indx<Y> *ind) {
   Y i,j,n;
   Z sum;

   if(sco!=NULL)delete [] sco;
   sco=new Z[ind->ix];

   for(i=0;i<ind->ix;i++) {
      j=ind->idx[i];
      this->readp_db(j);
      this->readz_db(j);
      sum=th;
      for(n=0;n<this->nw;n++)sum+=wt[this->nwd[n]]*this->lwt[n];
      sco[i]=sum;
      mark(i+1,100,"docs scored");
   }
   return(sco);
}

template<class Y,class Z>
Z *HubeX<Y,Z>::ScoreAll_lw(void) {
   Y j,n;
   Z sum;

   if(sco!=NULL)delete [] sco;
   sco=new Z[this->ndoc];

   for(j=0;j<this->ndoc;j++) {
      this->readp_db(j);
      this->readz_db(j);
      sum=th;
      for(n=0;n<this->nw;n++)sum+=wt[this->nwd[n]]*this->lwt[n];
      sco[j]=sum;
      mark(j+1,100,"docs scored");
   }
   return(sco);
}

template<class Y,class Z>
void HubeX<Y,Z>::Save(int n) {
   this->put_Nnum(n,"zh",this->ndoc,this->nwrd);
   this->bin_Writ(n,"weight",this->nwrd*sizeof(Z),(char*)wt);
   this->bin_Writ(n,"thresh",sizeof(Z),(char*)&th);
}

template<class Y,class Z>
void HubeX<Y,Z>::Load(int n) {
   long i,j,k;
   this->get_Nnum(n,"zh",i,k);
   this->ndoc=i;
   this->nwrd=k;
   wt=(Z*)this->get_Mmap(n,"weight");
   ifstream *pfin=this->get_Istr(n,"thresh",ios::in);
   pfin->read((char*)&th,sizeof(Z));
   this->dst_Istr(pfin);
}

template<class Y,class Z>
void HubeX<Y,Z>::Release(int n) {
    this->dst_Mmap(n,"weight",(char*&)wt);
}

}
#endif
