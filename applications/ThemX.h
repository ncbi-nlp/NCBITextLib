#ifndef THEMS_H
#define THEMS_H

#include <iostream>
#include <Index.h>
#include <XPost.h>
#include "BayeX.h"

namespace iret {

template<class Y,class Z>
class ThemX : public BayeX<Y,Z> {
   public:
      ThemX(const char *nmxpost,const char *pnam);
         // nmxpost is XPost object name
         // pnam points at name used for find path of from path_*pnam file
         // If begins with ":" then ":" is ignored and remainder is the path
      ~ThemX();

      void gopen_ThemX(void);
      void gclose_ThemX(void);

      void Set_Param(Y ntrm,Y stng);
         // ntrm: # terms in theme, stng: stringency
      void SetBackGrnd(Indx<Y> *pBd);
         // Sets a background in which the theme is sought
         // Works with or without setting this

      // Functions to find nearest neighbors from a seed document
      void Load(Y i);
         // Sets pointers for the ith doc
         // gopen_db_map() must be called first
      void ScoreAllvct(void);
         // Scores all docs
         // Assumes that doc space is filled by Load()
         // gopen_map() must be called first
      void ScoreBkgvct(void);
         // Scores all docs
         // Assumes that doc space is filled by Load()
         // gopen_map() must be called first
      Ordr<Y,Z> *Skim(Y n);
         // Skims off the top n this->scoring docs

      // Weighting functions
      WOrdr<Y,Z> *weightSthm(void); // Weights assigned to all terms
      WOrdr<Y,Z> *weightMthm(void); // Weights assigned to mrked terms
      Z *ScoreAll(WOrdr<Y,Z> *pord);
         // Scores the whole space of ndoc docs
         // Scores appear in this->sco and a pointer is returned
      Z *ScoreBkg(WOrdr<Y,Z> *pord);
         // Scores the pBnd space of docs
         // Scores appear in this->sco and a pointer is returned
      Z ScoreLim(WOrdr<Y,Z> *pord); // Sets xlim

      // Processing
      WOrdr<Y,Z> *Initiator(Indx<Y> *pInd);
         // Starts theme generation with pInd
      WOrdr<Y,Z> *Initiator(WOrdr<Y,Z> *wpord);
         // Starts theme generation
      WOrdr<Y,Z> *Iterator(void);
         // Makes one round of the theme EM algorithm
      WOrdr<Y,Z> *Moderator(WOrdr<Y,Z> *wpord,Y clim);
         // Runs to convergence or clim iterations, whichever comes first

      void show_terms(WOrdr<Y,Z> *wzord);
         // Prints out theme terms
      Z global_idf(Y n); // Computes IDF

      Z *axx;        // Alpha values, nwrd size
      Z xttrc;

      Y ntrm;        // Number of terms in theme
      Y stng;        // Stringency
      Z xlim;        // Threshhold for docs
      Indx<Y> *pBnd; // List of background documents
      Indx<Y> *cur;  // Pointer at current select docs
      Y sflag;       // Marks status of local doc space
                     // 0 unset, 1 from load, 2 from convert
      Y bflag;       // 1 if background is set, else 0
      Y mflag;       // 1 if mark array used to select terms, else 0
};

template<class Y,class Z>
ThemX<Y,Z>::ThemX(const char *nmxpost,const char *pnam) : BayeX<Y,Z>(nmxpost,pnam) {
   cur=NULL;
   pBnd=NULL;
   sflag=0;
   bflag=0;
   mflag=0;
}

template<class Y,class Z>
ThemX<Y,Z>::~ThemX(){
}

template<class Y,class Z>
void ThemX<Y,Z>::gopen_ThemX(void) {
   this->gopen_map();
   this->gopen_BayeX();
   this->weg=new Z[this->nwrd];
   this->axx=new Z[this->nwrd];
   pBnd=new Indx<Y>(0,this->ndoc);
}

template<class Y,class Z>
void ThemX<Y,Z>::gclose_ThemX(void) {
   this->gclose_map();
   this->gclose_BayeX();
   delete pBnd;
}

template<class Y,class Z>
void ThemX<Y,Z>::Set_Param(Y ntm,Y stg) {
   ntrm=ntm;
   stng=stg;
}

template<class Y,class Z>
void ThemX<Y,Z>::SetBackGrnd(Indx<Y> *pBd) {
   bflag=1;
   delete pBnd;
   pBnd=pBd;
}

template<class Y,class Z>
void ThemX<Y,Z>::Load(Y i) {
   if(sflag==2) {
      delete [] this->nwd;
      delete [] this->lwt;
   }
   this->readp_db(i);
   this->readz_db(i);
   sflag=1;
}

template<class Y,class Z>
void ThemX<Y,Z>::ScoreAllvct(void) {
   Y i,j,k;
   Z *flw,xx,sum=0;
   Indx<Y> *pInd;
   xttrc=(double)this->ndoc;
   if(this->sco)delete [] this->sco;
   this->sco=new Z[this->ndoc];
   for(i=0;i<this->ndoc;i++)this->sco[i]=0;

   for(i=0;i<this->nw;i++) {
      pInd=this->readp(this->nwd[i]);
      flw=this->readz(this->nwd[i]);
      j=pInd->ix;
      xx=global_idf(j)*this->lwt[i];
      for(k=0;k<j;k++) {
         this->sco[pInd->idx[k]]+=xx*flw[k];
      }
   }
}

template<class Y,class Z>
void ThemX<Y,Z>::ScoreBkgvct(void) {
   Y i,j,k;
   Z *flw,xx,sum=0;
   Indx<Y> *znd,*pnd,*snd;
   xttrc=(double)pBnd->ix;
   if(this->sco)delete [] this->sco;
   this->sco=new Z[this->ndoc];
   for(i=0;i<this->ndoc;i++)this->sco[i]=0;

   for(i=0;i<this->nw;i++) {
      pnd=this->readp(this->nwd[i]);
      znd=pBnd->cbool_And(pnd);
      if(znd) {
         snd=pnd->Subvalue(znd);
         flw=readw(this->nwd[i]);
         j=znd->ix;
         xx=global_idf(j)*this->lwt[i];
         for(k=0;k<j;k++) {
            this->sco[znd->idx[k]]+=xx*flw[snd->idx[k]];
         }
         delete znd;
         delete snd;
      }
   }  
}

template<class Y,class Z>
Ordr<Y,Z> *ThemX<Y,Z>::Skim(Y n) {
   Ordr<Y,Z> *pOrd;
   if(this->sco==NULL)return(NULL);
   Z ss;
   if(!bflag) pOrd=new Ordr<Y,Z>(n,this->ndoc,this->sco);
   else pOrd=new Ordr<Y,Z>(n,pBnd,this->sco);

   Y i=0;
   pOrd->ind(i,ss);
   if(ss>0)return(pOrd);
   else return(NULL);
}

template<class Y,class Z>
WOrdr<Y,Z> *ThemX<Y,Z>::weightSthm(void) {
   Y nstw=0,flag;
   Z min,max,diff,n_t,n_st;
   Z xx,frc,pt,qt,rt;

   this->cs=0;
   frc=((Z)this->nsx)/((Z)this->nnx);
   diff=this->nsx-this->nnx;

   for(Y i=0;i<this->nwrd;i++) {
      n_t=*(this->tx+i);
      if(n_t&&(n_t<this->nnx)) {
         min=(n_t<this->nsx)?n_t:this->nsx;
         max=n_t+diff;
         max=(0<max)?max:0;
         n_st=*(this->sx+i);
         flag=1;
         if(n_st==min) {
            if(n_st-1>n_t*frc)n_st-=1;
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
         *(this->weg+i)=log((double)pt*(1.0-qt))-log((double)qt*(1.0-pt));

         xx=-(Z)n_st*log((double)rt/pt)-(Z)(n_t-n_st)*log((double)rt/qt);
         xx-=(Z)(this->nsx-n_st)*log((double)(1.0-rt)/(1.0-pt));
         xx-=(Z)(this->nnx-this->nsx-n_t+n_st)*log((double)(1.0-rt)/(1.0-qt));
         *(this->axx+i)=xx;

         xx=log((double)(this->nsx-n_st))+log((double)(this->nnx-this->nsx));
         xx-=(log((double)this->nsx)+log((double)(this->nnx-this->nsx-n_t+n_st)));
      }
      else {
         *(this->weg+i)=0;
         *(this->axx+i)=0;
      }
   }
   if(this->pflag)cout << "Number of weighted terms= " << nstw << endl;
   Ordr<Y,Z> *pord=new Ordr<Y,Z>(ntrm,this->nwrd,this->axx);
   WOrdr<Y,Z> *wpord=new WOrdr<Y,Z>(pord,this->weg);
   return(wpord);
}

template<class Y,class Z>
WOrdr<Y,Z> *ThemX<Y,Z>::weightMthm(void) {
   Y nstw=0,flag;
   Z min,max,diff,n_t,n_st;
   Z xx,frc,pt,qt,rt;

   this->cs=0;
   frc=((Z)this->nsx)/((Z)this->nnx);
   diff=this->nsx-this->nnx;

   for(Y i=0;i<this->nwrd;i++) {
      n_t=*(this->tx+i);
      if(this->mrk[i]&&n_t&&(n_t<this->nnx)) {
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
         *(this->weg+i)=log((double)pt*(1.0-qt))-log((double)qt*(1.0-pt));

         xx=-(Z)n_st*log((double)rt/pt)-(Z)(n_t-n_st)*log((double)rt/qt);
         xx-=(Z)(this->nsx-n_st)*log((double)(1.0-rt)/(1.0-pt));
         xx-=(Z)(this->nnx-this->nsx-n_t+n_st)*log((double)(1.0-rt)/(1.0-qt));
         *(this->axx+i)=xx;

         xx=log((double)(this->nsx-n_st))+log((double)(this->nnx-this->nsx));
         xx-=(log((double)this->nsx)+log((double)(this->nnx-this->nsx-n_t+n_st)));
      }
      else {
         *(this->weg+i)=0;
         *(this->axx+i)=0;
      }
   }
   if(this->pflag)cout << "Number of weighted terms= " << nstw << endl;
   Ordr<Y,Z> *pord=new Ordr<Y,Z>(ntrm,this->nwrd,this->axx);
   WOrdr<Y,Z> *wpord=new WOrdr<Y,Z>(pord,this->weg);
   return(wpord);
}

template<class Y,class Z>
Z *ThemX<Y,Z>::ScoreAll(WOrdr<Y,Z> *wpord) {
   Y i,j,n;
   Indx<Y> *pnd,*qnd=(Indx<Y> *)wpord;
   Z *wtt=wpord->weight,xx;

   if(this->sco!=NULL)delete [] this->sco;
   this->sco=new Z[this->ndoc];
   for(n=0;n<this->ndoc;n++)this->sco[n]=0;

   for(i=0;i<qnd->ix;i++) {
      j=wpord->ind(i,xx);
      pnd=this->readp(j);
      xx=wtt[i];
      for(n=0;n<pnd->ix;n++) {
         this->sco[pnd->idx[n]]+=(Z)xx;
      }
      mark(i+1,10,"terms scored");
   }
   return(this->sco);
}

template<class Y,class Z>
Z *ThemX<Y,Z>::ScoreBkg(WOrdr<Y,Z> *wpord) {
   Y i,j,n;
   Indx<Y> *pnd,*qnd=(Indx<Y> *)wpord,*znd;
   Z *wtt=wpord->weight,xx;

   if(this->sco!=NULL)delete [] this->sco;
   this->sco=new Z[this->ndoc];
   for(n=0;n<this->ndoc;n++)this->sco[n]=0;

   for(i=0;i<qnd->ix;i++) {
      j=wpord->ind(i,xx);
      pnd=this->readp(j);
      znd=pBnd->cbool_And(pnd);
      xx=wtt[i];
      if(znd) {
         for(n=0;n<znd->ix;n++) {
            this->sco[znd->idx[n]]+=(Z)xx;
         }
         delete znd;
      }
      mark(i+1,10,"terms scored");
   }
   return(this->sco);
};

template<class Y,class Z>
Z ThemX<Y,Z>::ScoreLim(WOrdr<Y,Z> *wpord) {
   Y i,j,n;
   Z sy;
   Z xx,sum=0;
   Indx<Y> *pnd,*qnd=(Indx<Y> *)wpord;
   Z *wtt=wpord->weight;
   if(!qnd->ix){cout << "Error, ix=0" << endl;exit(0);}
   for(i=0;i<qnd->ix;i++) {
      sum+=wtt[i];
   }
   xx=(Z)qnd->ix;
   xlim=stng*sum/xx;
   return(xlim);
}

template<class Y,class Z>
WOrdr<Y,Z> *ThemX<Y,Z>::Initiator(Indx<Y> *pInd) {
   Y i;
   Indx<Y> *qInd;
   Ordr<Y,Z> *qord;
   WOrdr<Y,Z> *wpord;
   if(!bflag) {
      for(i=0;i<this->nwrd;i++)this->tx[i]=this->freq[i];
      this->nnx=this->ndoc;
      this->zeros();
      this->countSX(pInd);
   }
   else {
      this->zerot();
      this->countTX(pBnd);
      Indx<Y> *znd=pBnd->cbool_And(pInd);
      if(!znd){cout << "Empty intersection!" << endl;exit(0);}
      this->zeros();
      this->countSX(znd);
      delete znd;
   }
   if(!mflag)wpord=weightSthm();
   else wpord=weightMthm();
   if(!bflag)ScoreAll(wpord);
   else ScoreBkg(wpord);
   xlim=ScoreLim(wpord);
   if(this->pflag)cout << "xlim " << xlim << endl;
   if(cur!=NULL) { delete cur; cur=NULL; }
   cur=pBnd->Greateq(xlim,this->sco);
   if((!cur)||(cur->ix<20)) {
      if(cur!=NULL) { delete cur; cur=NULL; }
      qord=new Ordr<Y,Z>(20,pBnd,this->sco);
      cur=new Indx<Y>(qord);
      delete qord;
   }
   return(wpord);
}

template<class Y,class Z>
WOrdr<Y,Z> *ThemX<Y,Z>::Initiator(WOrdr<Y,Z> *wpord) {
   Y i;
   Indx<Y> *qInd;
   Ordr<Y,Z> *qord;
   if(!bflag) {
      for(i=0;i<this->nwrd;i++)this->tx[i]=this->freq[i];
      this->nnx=this->ndoc;
      ScoreAll(wpord);
   }
   else {
      this->zerot();
      this->countTX(pBnd);
      ScoreBkg(wpord);
   }
   xlim=ScoreLim(wpord);
   if(this->pflag)cout << "xlim " << xlim << endl;
   if(cur!=NULL) { delete cur; cur=NULL; }
   cur=pBnd->Greateq(xlim,this->sco);
   if((!cur)||(cur->ix<20)) {
      if(cur!=NULL) { delete cur; cur=NULL; }
      qord=new Ordr<Y,Z>(20,pBnd,this->sco);
      cur=new Indx<Y>(qord);
      delete qord;
   }
   return(wpord);
}

template<class Y,class Z>
WOrdr<Y,Z> *ThemX<Y,Z>::Iterator(void) {
   Indx<Y> *qInd;
   Ordr<Y,Z> *qord;
   WOrdr<Y,Z> *wpord;
   this->zeros();
   this->countSX(cur);
   if(!mflag)wpord=weightSthm();
   else wpord=weightMthm();
   if(!bflag)ScoreAll(wpord);
   else ScoreBkg(wpord);
   xlim=ScoreLim(wpord);
   if(this->pflag)cout << "xlim " << xlim << endl;
   if(cur!=NULL) { delete cur; cur=NULL; }
   cur=pBnd->Greateq(xlim,this->sco);
   if((!cur)||(cur->ix<20)) {
      if(cur!=NULL) { delete cur; cur=NULL; }
      qord=new Ordr<Y,Z>(20,pBnd,this->sco);
      cur=new Indx<Y>(qord);
      delete qord;
   }
   return(wpord);
}

template<class Y,class Z>
WOrdr<Y,Z> *ThemX<Y,Z>::Moderator(WOrdr<Y,Z> *wpord,Y clim) {
   Z nwt,owt;
   Y cmt=0;
   WOrdr<Y,Z> *wzord;
   Z alpha;

   nwt=wpord->weight[0];
   owt=nwt+1.0;
   while((owt!=nwt)&&(cmt<clim)) {
      owt=nwt;
      wzord=Iterator();
      if(this->pflag) cout << "docs " << cur->ix << endl;
      nwt=wzord->weight[0];
      if((owt!=nwt)&&(cmt+1<clim))delete wzord;
      cmt++;
   }
   return(wzord);
}

template<class Y,class Z>
void ThemX<Y,Z>::show_terms(WOrdr<Y,Z> *wzord) {
   Y i,j;
   Z alpha;
   this->gopen_lexos();
   for(i=0;i<wzord->ix;i++) {
      j=wzord->ind(i,alpha);
      cout << i+1 <<'\t' << alpha << '\t' << wzord->weight[i] << '\t' <<\
      this->show(j) << '\t' << this->tx[j] << '\t' << this->sx[j] << endl;
   }
}

template<class Y,class Z>
Z ThemX<Y,Z>::global_idf(Y n) {
   double l2=1.0/log(2.0),dmt=20.0;

   if(n>dmt)return(l2*log(xttrc/((double)n)));
   else if(n>1)return(l2*log(xttrc/dmt));
   else return(0.0);
}

}
#endif

