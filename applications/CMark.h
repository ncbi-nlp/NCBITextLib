#ifndef CMRK_H
#define CMRK_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <Index.h>
#include <XPost.h>
#include <runn.h>

using namespace std;
namespace iret {

// Template class CMark: Used for BayeX and HubeX
template<class Y,class Z>
class CMark : public XPost<Y,Z> {
public:
   CMark(const char *nspost,const char *pnam);
      // Name of XPost set
      // pnam is either used in "path_pnam" as place to find path or
      // if begins with ':' is followed by path itself
   ~CMark(void);

   // Counting functions
   // Must call gopen_db_map() first
   void init_cnt(void);
      // Sets pdoc uniform
      // Sets memory for tx and sx
   void zerot(void);   // Zeroes tx array
   void zeros(void);   // Zeroes sx array
   void freq_tx(void); // Puts freq values in tx array

   void countDoc(Y i);
      // Adds the counts from doc i to tx using read function
   void counsDoc(Y i);
      // Adds the counts from doc i to sx using read function
   void counbDoc(Y i);
      // Adds the counts from doc i to sx & tx using read function

   void countTX(Indx<Y> *cnd); // Adds all counts in cnd to tx
   void countSX(Indx<Y> *cnd); // Adds all counts in cnd to sx
   void countBX(Indx<Y> *cnd); // Adds all counts in cnd to sx & tx

   void Set_Term(Indx<Y> *pTrm);
      // Set the mrk array to process terms listed in pTrm
   void Set_Term_freq(Y nm);
      // Set the mrk array
      // If term_freq<nm then 0, else 1. Based on count array tx
   void Set_All(int n); // Creates and sets mrk array to be n everywhere

   // Relate to the terms processing
   Z eps;  // Size of 1/ndoc
   Z nnx;
   Z nsx;
   Z *tx;
   Z *sx;
   Z *pdoc;
   Y *mrk; // Marks which terms to include in process
   Y tmk;  // Number of words marked
};

template<class Y,class Z>
CMark<Y,Z>::CMark(const char *namspost,const char *pnam) : XPost<Y,Z>(namspost,pnam) {
   mrk=NULL;
   tx=NULL;
   sx=NULL;
   pdoc=NULL;
}

template<class Y,class Z>
CMark<Y,Z>::~CMark() {
   if(mrk!=NULL)delete [] mrk;
   if(tx!=NULL)delete [] tx;
   if(sx!=NULL)delete [] sx;
   if(pdoc!=NULL)delete [] pdoc;
}

template<class Y,class Z>
void CMark<Y,Z>::init_cnt(void) {
   Y i;
   eps=1.0;

   if(tx==NULL)tx=new Z[this->nwrd];
   if(sx==NULL)sx=new Z[this->nwrd];
   if(pdoc==NULL)pdoc=new Z[this->ndoc];
   for(i=0;i<this->ndoc;i++)pdoc[i]=eps;
   if(this->pflag)cout<<"end of init\n";
}

template<class Y,class Z>
void CMark<Y,Z>::zerot(void) {
   nnx=0.0;
   for(Y i=0;i<this->nwrd;i++)*(tx+i)=0;
}

template<class Y,class Z>
void CMark<Y,Z>::zeros(void) {
   nsx=0.0;
   for(Y i=0;i<this->nwrd;i++)*(sx+i)=0;
}

template<class Y,class Z>
void CMark<Y,Z>::freq_tx(void) {
   this->gopen_map();
   nnx=this->ndoc;
   for(Y i=0;i<this->nwrd;i++)*(tx+i)=this->freq[i];
}

template<class Y,class Z>
void CMark<Y,Z>::countDoc(Y i) {
   Y j,k;
   Z pt;
   pt=pdoc[i];

   this->readp_db(i);
   for(k=0;k<this->nw;k++) {
      j=*(this->nwd+k);
      (*(tx+j))+=pt;
   }
}

template<class Y,class Z>
void CMark<Y,Z>::counsDoc(Y i) {
   Y j,k;
   Z pt;
   pt=pdoc[i];

   this->readp_db(i);
   for(k=0;k<this->nw;k++) {
      j=*(this->nwd+k);
      (*(sx+j))+=pt;
   }
}

template<class Y,class Z>
void CMark<Y,Z>::counbDoc(Y i) {
   Y j,k;
   Z pt;
   pt=pdoc[i];

   this->readp_db(i);
   for(k=0;k<this->nw;k++) {
      j=*(this->nwd+k);
      (*(sx+j))+=pt;
      (*(tx+j))+=pt;
   }
}

template<class Y,class Z>
void CMark<Y,Z>::countTX(Indx<Y> *cnd) {
   Y i,j;

   for(i=0;i<cnd->ix;i++) {
      j=cnd->idx[i];
      this->countDoc(j);
      nnx+=pdoc[j];
      mark(i,1000,"docs");
   }
}

template<class Y,class Z>
void CMark<Y,Z>::countSX(Indx<Y> *cnd) {
   Y i,j;

   for(i=0;i<cnd->ix;i++) {
      j=cnd->idx[i];
      this->counsDoc(j);
      nsx+=pdoc[j];
      mark(i,1000,"docs");
   }
}

template<class Y,class Z>
void CMark<Y,Z>::countBX(Indx<Y> *cnd) {
   Y i,j;

   for(i=0;i<cnd->ix;i++) {
      j=cnd->idx[i];
      this->counbDoc(j);
      nnx+=pdoc[j];
      nsx+=pdoc[j];
      mark(i,1000,"docs");
   }
}

template<class Y,class Z>
void CMark<Y,Z>::Set_Term(Indx<Y> *pTrm) {
   Y i;
   if(mrk==NULL)mrk=new Y[this->nwrd];
   for(i=0;i<this->nwrd;i++) mrk[i]=0;
   
   for(i=0;i<pTrm->ix;i++) mrk[pTrm->idx[i]]=1;
   if(this->pflag)cout << "Number of marked terms= " << pTrm->ix << endl;
}

template<class Y,class Z>
void CMark<Y,Z>::Set_Term_freq(Y nm) {
   Y i,nstw=0;
   Z xx=eps*nm;
   if(mrk==NULL)mrk=new Y[this->nwrd];
   for(i=0;i<this->nwrd;i++) {
      if(tx[i]<xx)mrk[i]=0;
      else {
         mrk[i]=1;
         nstw++;
      }
   }
   if(this->pflag)cout << "Number of freq marked terms= " << nstw << endl;
}

template<class Y,class Z>
void CMark<Y,Z>::Set_All(int n) {
   Y i;
   if(mrk==NULL)mrk=new Y[this->nwrd];
   for(i=0;i<this->nwrd;i++)mrk[i]=n;
}

}
#endif
