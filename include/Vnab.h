#ifndef VNAB_H
#define VNAB_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <XPost.h>

using namespace std;
namespace iret {

const double l2=1.0/log(2.0),dmt=20.0,lfac=log(0.65);
const double lfab=log(0.7);
extern double xttrc,xb,xa,xg;

// Local weighting functions

float q_const(int lc,long n); // Constant 1.0

float d_lc_func(int lc,long n); // Returns lc

float d_wilbur_med(int lc,long n); // Wilbur Poisson formula

float d_bm25(int lc,long n); // Okapi BM25 formula

float s_wilbur_med(int lc,unsigned int n); // Wilbur Poisson formula

// Global weighting functions

float global_const(long n); // Constant 1.0

float global_idf(long n); // IDF

float global_sidf(long n); // IDF but scores even single occuring terms

float global_bm25(long n); // Okapi BM25 formula

float sglobal_const(unsigned int n); // Constant 1.0

float sglobal_idf(unsigned int n); // IDF

float sglobal_sidf(unsigned int n); // IDF but scores even single occuring terms

template<class Y,class Z>
class VnbX : public XPost<Y,Z> {
   public:
      VnbX(const char *nm,const char *pnam);
         // nm: name of XPost
         // pnam: path file suffix, i.e. "path_panm" as place to find path
      ~VnbX(void);

      void Load(Y i); // Set pointers for the ith doc
         // gopen_db_map() must be called first
      void Convert(Doc<Z> *pDoc,float (*d_local)(int,long)); 
         // Create space, fills, and sets pointers
         // gopen_hash() must be called first

      void ScoreAll(float (*global)(long)); // Score all documents
         // Assume that doc space is filled
         // gopen_map() must be called first
         // Use local weights
      void ScoreAllEx(Z *wx); // Scores all documents
         // Assume that doc space is filled
         // gopen_map() must be called first
         // Uses local weights

      Ordr<Y,Z> *Skim(Y n);   // Skim off the top n scoring docs
      Ordr<Y,Z> *Skimgr(Z s); // Skim off the docs scoring >s
      Ordr<Y,Z> *Skimge(Z s); // Skim off the docs scoring >=s

      // Data
      long sflag; // Status of local doc space (0: unset, 1: from load, 2: from convert)
      Z *sco; // Score array for whole space
};

template<class Y,class Z> 
VnbX<Y,Z>::VnbX(const char *nm,const char *pnam) : XPost<Y,Z>(nm,pnam) {
   sco=NULL;
   sflag=0;
}

template<class Y,class Z> 
VnbX<Y,Z>::~VnbX(void) {
   if(sco)delete [] sco;
}

template<class Y,class Z> 
void VnbX<Y,Z>::Load(Y i) {
   if(sflag==2) {
      delete [] this->nwd;
      delete [] this->lwt;
   }
   this->gopen_db_map();
   this->readp_db(i);
   this->readz_db(i);
   sflag=1;
}

template<class Y,class Z> 
void VnbX<Y,Z>::Convert(Doc<Z> *pDoc,float (*d_local)(int,long)) {
   Y i,j,k,u;
   i=pDoc->nw;
   this->gopen_hash();
   if(sflag==2) {
      delete [] this->nwd;
      delete [] this->lwt;
   }
   if(i) {
      this->nwd=new Y[i];
      this->lwt=new Z[i];
      sflag=2;
   }
   else sflag=0;
   k=0;
   for(j=0;j<i;j++) {
      u=this->find(pDoc->word[j]);
      if(u) {
         this->nwd[k]=(Y)(u-1);
         this->lwt[k]=(Z)d_local((int)pDoc->lcnt[j],i);
         k++;
      }
   }
   this->nw=(Y)k;
}

template<class Y,class Z>
void VnbX<Y,Z>::ScoreAll(float (*global)(long)) {
   Y i,j,k;
   Z *flw,xx;
   Indx<Y> *pInd;
   this->gopen_map();
   xttrc=(double)this->ndoc;
   if(sco)delete [] sco;
   sco=new Z[this->ndoc];
   for(i=0;i<this->ndoc;i++)sco[i]=0;
  
   for(i=0;i<this->nw;i++) {
      pInd=this->readp(this->nwd[i]);
      flw=this->readz(this->nwd[i]);
      j=pInd->ix;
      xx=(Z)global(j)*this->lwt[i];
      for(k=0;k<j;k++) {
         sco[pInd->idx[k]]+=xx*flw[k];
      }
   }
}

template<class Y,class Z> 
void VnbX<Y,Z>::ScoreAllEx(Z *wx) {
   Y i,j,k;
   Z *flw,xx;
   Indx<Y> *pInd;
   this->gopen_map();
   xttrc=(double)this->ndoc;
   if(sco)delete [] sco;
   sco=new Z[this->ndoc];
   for(i=0;i<this->ndoc;i++)sco[i]=0;

   for(i=0;i<this->nw;i++ ) {
      pInd=this->readp(this->nwd[i]);
      flw=this->readz(this->nwd[i]);
      j=pInd->ix;
      xx=(Z)wx[this->nwd[i]]*this->lwt[i];
      for(k=0;k<j;k++) {
         sco[pInd->idx[k]]+=xx*flw[k];
      }
   }
}

template<class Y,class Z> 
Ordr<Y,Z> *VnbX<Y,Z>::Skim(Y n) {
   if(sco==NULL)return(NULL);
   Z sx;
   Ordr<Y,Z> *pOrd=new Ordr<Y,Z>(n,this->ndoc,sco);

   Y i=0;
   pOrd->ind(i,sx);
   if(sx>0)return(pOrd);
   else return(NULL);
}

template<class Y,class Z> 
Ordr<Y,Z> *VnbX<Y,Z>::Skimgr(Z s) {
   if(sco==NULL)return(NULL);
   Y *dx=new Y[this->ndoc];
   Y i,m=0;
   for(i=0;i<this->ndoc;i++) {
      if(sco[i]>s) {
         dx[m++]=i;
      }
   }
   if(m) {
      Indx<Y> *pnd=new Indx<Y>(m,dx,0);
      Ordr<Y,Z> *pOrd=new Ordr<Y,Z>(m,pnd,sco);
      delete pnd;
      delete [] dx;
      return(pOrd);
   } 
   else {
      delete [] dx;
      return(NULL);
   }
}

template<class Y,class Z> 
Ordr<Y,Z> *VnbX<Y,Z>::Skimge(Z s) {
   if(sco==NULL)return(NULL);
   Y *dx=new Y[this->ndoc];
   Y i,m=0;
   for(i=0;i<this->ndoc;i++) {
      if(sco[i]>=s) {
         dx[m++]=i;
      }
   }
   if(m) {
      Indx<Y> *pnd=new Indx<Y>(m,dx,0);
      Ordr<Y,Z> *pOrd=new Ordr<Y,Z>(m,pnd,sco);
      delete pnd;
      delete [] dx;
      return(pOrd);
   } 
   else {
      delete [] dx;
      return(NULL);
   }
}

}
#endif
