#ifndef BAYEX_H
#define BAYEX_H

#include <iostream>
#include <fstream>
#include <Index.h>
#include "CMark.h"

namespace iret {

// Template class BayeX: Bayes classifiers
template<class Y,class Z>
class BayeX : public CMark<Y,Z> {
   public:
      BayeX(const char *nmxpost,const char *pnam);
         // nmxpost is XPost object name
         // pnam points at name used for find path of from path_*pnam file
         // If begins with ":" then ":" is ignored and remainder is the path
      ~BayeX();

      void gopen_BayeX(void);
         // Opens for operating. Initializes all pdoc to 1.0
      void gclose_BayeX(void);

      // Weighting functions
      void weightSall(void); // Weights assigned to all terms by MBM 
      void weightSmrk(void); // Weights assigned to all terms by MBM 

      // Scoring function
      // Scores only terms with 1 in mrk array
      Z *ScoreAll(void); 
      Z *ScoreSet(Indx<Y> *ind);

      // Allows to save and load weights after learning
      void Save(int n);    // Saves the set of weights weg. Marks with n
      void Load(int n);    // Loads the set of weights weg marked with n
      void Release(int n); // Unmaps the weight vector mapped by Load()

      // Global data
      Z *weg; // For weights, nwrd size
      Z *csg; // For individual cs numbers
      Z cs;   // Additve constant for correct scoring of docs
      Z *sco; // For document scores
};

template<class Y,class Z>
BayeX<Y,Z>::BayeX(const char *namspost,const char *pnam) : CMark<Y,Z>(namspost,pnam) {
   weg=NULL;
   csg=NULL;
   sco=NULL;
}

template<class Y,class Z>
BayeX<Y,Z>::~BayeX() {
   if(weg!=NULL)delete [] weg;
   if(csg!=NULL)delete [] csg;
   if(sco!=NULL)delete [] sco;
}

template<class Y,class Z>
void BayeX<Y,Z>::gopen_BayeX(void) {
   Y i;

   this->gopen_db_map();
   this->init_cnt();
   for(i=0;i<this->ndoc;i++) this->pdoc[i]=1.0;
}

template<class Y,class Z>
void BayeX<Y,Z>::gclose_BayeX(void) {
   this->gclose_db_map();
}

template<class Y,class Z>
void BayeX<Y,Z>::weightSall(void) {
   Y nstw=0,flag,i;
   Z min,max,diff,n_t,n_st;
   Z xx,frc,pt,qt,rt;

   if(weg!=NULL)delete [] weg;
   weg=new Z[this->nwrd];
   if(csg!=NULL)delete [] csg;
   csg=new Z[this->nwrd];
   cs=0;
   frc=((Z)this->nsx)/((Z)this->nnx);
   diff=this->nsx-this->nnx;

   for(i=0;i<this->nwrd;i++) {
      n_t=*(this->tx+i);
      if(n_t&&(n_t<this->nnx)) {
         min=(n_t<this->nsx)?n_t:this->nsx;
         max=n_t+diff;
         max=(0<max)?max:0;
         n_st=*(this->sx+i);
         flag=1;
         if(n_st==min) {
            if(n_st-1>n_t*frc)n_st -=1;
            else flag=0;
         }
         else if(n_st<=max) {
            if(max+1<n_t*frc)n_st=max+1;
            else flag=0;
         }
      }
      else flag=0;

      if(flag) {
         nstw++;
         pt =(Z)n_st/(Z)this->nsx;
         qt =(Z)(n_t-n_st)/(Z)(this->nnx-this->nsx);
         rt =(Z)n_t/(Z)this->nnx;
         // Calculate wt for this term
         *(weg+i)=log(pt*(1.0-qt))-log(qt*(1.0-pt));

         xx=log((Z)(this->nsx-n_st))+log((Z)(this->nnx-this->nsx));
         xx-=(log((Z)this->nsx)+log((Z)(this->nnx-this->nsx-n_t+n_st)));
         *(csg+i)=xx;
         cs+=xx;
      }
      else {
         *(weg+i)=0;
         *(csg+i)=0;
      }
      mark(i,10000,"terms weighted");
   }

   if(this->pflag)cout << "cs= " << cs << endl;
   if(this->pflag)cout << "Number of weighted terms= " << nstw << endl;
}

template<class Y,class Z>
void BayeX<Y,Z>::weightSmrk(void) {
   Y nstw=0,flag,i;
   Z min,max,diff,n_t,n_st;
   Z xx,frc,pt,qt,rt;

   if(weg!=NULL)delete [] weg;
   weg=new Z[this->nwrd];
   if(csg!=NULL)delete [] csg;
   csg=new Z[this->nwrd];
   cs=0;
   frc=((Z)this->nsx)/((Z)this->nnx);
   diff=this->nsx-this->nnx;

   for(i=0;i<this->nwrd;i++) {
      if((!this->mrk[i])||(this->sx[i]<2)){weg[i]=csg[i]=0;continue;}
      n_t=*(this->tx+i);
      if(n_t&&(n_t<this->nnx)) {
         min=(n_t<this->nsx)?n_t:this->nsx;
         max=n_t+diff;
         max=(0<max)?max:0;
         n_st=*(this->sx+i);
         flag=1;
         if(n_st==min) {
            if(n_st-1>n_t*frc)n_st -=1;
            else flag=0;
         }
         else if(n_st<=max) {
            if(max+1<n_t*frc)n_st=max+1;
            else flag=0;
         }
      }
      else flag=0;

      if(flag) {
         nstw++;
         pt =(Z)n_st/(Z)this->nsx;
         qt =(Z)(n_t-n_st)/(Z)(this->nnx-this->nsx);
         rt =(Z)n_t/(Z)this->nnx;
         // Calculate wt for this term
         *(weg+i)=log(pt*(1.0-qt))-log(qt*(1.0-pt));

         xx=log((Z)(this->nsx-n_st))+log((Z)(this->nnx-this->nsx));
         xx-=(log((Z)this->nsx)+log((Z)(this->nnx-this->nsx-n_t+n_st)));
         *(csg+i)=xx;
         cs+=xx;
      }
      else {
         *(weg+i)=0;
         *(csg+i)=0;
      }
      mark(i,10000,"terms weighted");
   }

   if(this->pflag)cout << "cs= " << cs << endl;
   if(this->pflag)cout << "Number of weighted terms= " << nstw << endl;
}

template<class Y,class Z>
Z *BayeX<Y,Z>::ScoreAll(void) {
   Y i,j,n;
   Z xx;

   cs=0;
   for(i=0;i<this->nwrd;i++) {
      if(this->mrk[i])cs+=csg[i];
   }
   if(sco!=NULL)delete [] sco;
   sco=new Z[this->ndoc];
   for(n=0;n<this->ndoc;n++) {
      xx=cs;
      this->readp_db(n);
      for(i=0;i<this->nw;i++) {
         j=this->nwd[i];
         if(this->mrk[j])xx+=weg[j];
      }
      sco[n]=xx;
      mark(n+1,1000,"docs scored");
   }
   return(sco);
}

template<class Y,class Z>
Z *BayeX<Y,Z>::ScoreSet(Indx<Y> *ind) {
   Y i,j,n,u;
   Z xx;

   cs=0;
   for(i=0;i<this->nwrd;i++) {
      if(this->mrk[i])cs+=csg[i];
   }
   if(sco!=NULL)delete [] sco;
   sco=new Z[this->ndoc];
   for(u=0;u<ind->ix;u++) {
      n=ind->idx[u];
      xx=cs;
      this->readp_db(n);
      for(i=0;i<this->nw;i++) {
         j=this->nwd[i];
         if(this->mrk[j])xx+=weg[j];
      }
      sco[n]=xx;
      mark(u+1,1000,"docs scored");
   }
   return(sco);
}

template<class Y,class Z>
void BayeX<Y,Z>::Save(int n) {
   this->put_Nnum(n,"zb",this->ndoc,this->nwrd);
   this->bin_Writ(n,"weightb",this->nwrd*sizeof(Z),(char*)weg);
   this->bin_Writ(n,"threshb",sizeof(Z),(char*)&cs);
}

template<class Y,class Z>
void BayeX<Y,Z>::Load(int n) {
   long i,j,k;
   this->get_Nnum(n,"zb",i,k);
   this->ndoc=i;
   this->nwrd=k;
   weg=(Z*)this->get_Mmap(n,"weightb");
   ifstream *pfin=this->get_Istr(n,"threshb",ios::in);
   pfin->read((char*)&cs,sizeof(Z));
   this->dst_Istr(pfin);
}

template<class Y,class Z>
void BayeX<Y,Z>::Release(int n) {
    this->dst_Mmap(n,"weightb",(char*&)weg);
}

}
#endif
