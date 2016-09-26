#include "XPost.h"

namespace iret {

STerm::STerm(const char *par,const char *nam,const char *pnam) : FBase(par,nam,pnam) {
   pLx=NULL;
   pCh=NULL;
   open1=open2=0;
}

STerm::~STerm(void) {
   if(pLx)delete pLx;
   if(pCh)delete pCh;
}

void STerm::create(strMap &Mp) {
   pLx=new Lexos(name);
   map_down_sub((FBase *)pLx,"lexos");
   pLx->create_Lexos(Mp);
   pCh=new Chash(name);
   map_down_sub((FBase *)pCh,"chash");
   pCh->create_ctable_STerm(Mp,3);
}

Lexos *STerm::gopen_Lexos(void) {
   if(!open1) {
      pLx=new Lexos(name);
      map_down_sub((FBase *)pLx,"lexos");
      pLx->gopen_map();
      open1=1;
   }
   return(pLx);
}

void STerm::gclose_Lexos(void) {
   if(open1) {
      pLx->gclose_map();
      delete pLx;
      pLx=NULL;
      open1=0;
   }
}

Chash *STerm::gopen_Chash(void) {
   if(!open2) {
      pCh=new Chash(name);
      map_down_sub((FBase *)pCh,"chash");
      pCh->gopen_ctable_map();
      open2=1;
   }
   return(pCh);
}

void STerm::gclose_Chash(void) {
   if(open2) {
      pCh->gclose_ctable_map();
      delete pCh;
      pCh=NULL;
      open2=0;
   }
}

}
