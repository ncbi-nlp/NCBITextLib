#ifndef XPOST_H
#define XPOST_H

#include <iostream>
#include <fstream>
#include <FBase.h>
#include <Dmap.h>
#include <Doc.h>
#include <Index.h>
#include <Strset.h>
#include <Hash.h>
#include <runn.h>

using namespace std;
namespace iret {

class STerm : public FBase {
   public:
      STerm(const char *typ,const char *nam,const char *pnam);
         // typ: type of parent class. nam: name of the parent class
         // pnam: used in "path_pnam" as place to find path or
         // if begins with ':' is followed by path itself.
     ~STerm();

      void create(strMap &Mp);
      Lexos *gopen_Lexos(void); // Opens the Lexos maps for use
      void gclose_Lexos(void);  // Closes the Lexos maps for use
      Chash *gopen_Chash(void); // Opens the Chash maps for use
      void gclose_Chash(void);  // Closes the Chash maps for use

      Lexos *pLx; // Points at the lexically order term list object
      Chash *pCh; // Points at the Hash table that can return a term number
};

template<class Y,class Z>
class XPost : public FBase {
   public:
      XPost(const char *nm,const char *pnam);
         // nm: name
         // pnam: path file suffix, i.e. "path_pnam" as place to find path
     ~XPost();

      // Create all structures
      // Simply call create_Allz() after initializing Doc and XPost
      void create_Allz(Doc<Z> &SDc);
         // Creates all files for XPost, postings and local Z values 

      void create_Terms(Doc<Z> &SDc); // Create term structures
         // Reads SDc and creates all the term structures
      void create_DBinz(Doc<Z> &SDc); // Creates binary document representation
         // Creates local Z values in .x file
         // Also creates a .z file with ndoc, max set size and nwrd 
      void create_Postz(void); // Creates posting files for Z values 
         // Also creates a .z file with nwrd, nslcs and ndoc in it

      // Access XPost information
      // Search a term ID: gopen_hash() -> find(). NOTE: find() returns ID+1
      // Search a term: gopen_lexos() -> show()
      void gopen_hash(void);      // Opens pCh for use
      long find(const char *str); // Returns term num+1 if found, else 0
      void gopen_lexos(void);     // Opens pLx for use
      char *show(Y n); // Returns pointer at string numbered n in system

      // Read term IDs and local Z values for a document ID
      //
      // gopen_db_map()
      // -> readp_db() for term IDs / readz_db() for local Z values
      // -> gclose_db_map()
      //
      // ndoc: # documents in system
      // nwrd: # terms in system
      // nw: # terms in a document
      // readp_db(): term IDs are stored in nwd
      // readz_db(): local Z values are stored in lwt
      void gopen_db_map(void);
         // Maps the addr arrays. Maps size arrays
         // Also reads in .z file with ndoc, nsets and nwrd in it
      void readp_db(Y n);
         // Sets pointer at postings. Array for nth term. Sets term count
      void readz_db(Y n);
         // Sets pointer at array of local Z values 
      void gclose_db_map(void);
         // Unmaps the addr arrays and the size arrays

      // Read document IDs and local Z values for a term ID
      //
      // gopen_map()
      // -> readp() for document IDs / readz() for local Z values
      // -> gclose_map()
      //
      // ndoc: # documents in system
      // nwrd: # terms in system
      // readp(): document IDs are stored in pInd. Document freq: pInd->ix
      // readz(): local Z values are stored in wz
      void gopen_map(void);
         // Maps the addr arrays. Maps freq arrays
         // Also reads in .z file with nwrd, nslcs and ndoc in it
      Indx<Y> *readp(Y n);
         // Returns Indx pointer at postings. Index object for nth term.
      Z *readz(Y n);
         // Returns a pointer at list of local Z values
      void gclose_map(void);
         // Unmaps freq and addr

      // Global data
      Y ndoc; // Number of documents in system
      Y nwrd; // Number of terms in system

      // DBIN data
      Y nw;   // Number of terms in doc
      Y *nwd; // Array of term numbers in doc
      Z *lwt; // Array of local Z values in doc

      // Postings data
      Indx<Y> *pInd; // Document frequency and postings
      Z *wz;         // local Z values

      // Local data
      Y *nsiz;     // Maps number of terms in each doc
      long *addrd; // Maps the addresses for docs
      Y *term;     // Maps the term numbers for all docs
      Z *dw;       // Maps local Z values for all docs

      Y *freq;     // Maps the frequency of terms
      Z *sgw;      // Maps square root of global weights
      long *addr;  // Maps the addresses for postings 
      Y *post;     // Maps the postings file
      Z *wc;       // Maps the local weights
      Lexos *pLx;  // Points at the lexically order term list object
      Chash *pCh;  // Points at the Hash table that can return a term number
      STerm *pSt;  // Points at the STerm object
};

template<class Y,class Z>
XPost<Y,Z>::XPost(const char *nm,const char *pnam) : FBase("xpost",nm,pnam) {
   open1=open2=open3=open4=0;
   pSt=new STerm("xpost",nm,pnam);
} 

template<class Y,class Z>
XPost<Y,Z>::~XPost() {
   delete pSt;
}  

template<class Y,class Z>
void XPost<Y,Z>::create_Allz(Doc<Z> &SDc) {
   create_Terms(SDc);
   create_DBinz(SDc);
   create_Postz();
}

template<class Y,class Z>
void XPost<Y,Z>::create_Terms(Doc<Z> &SDc) {
   Y i,*px;
   long k,lxn;
   Z j;
   const char *pch;
   char *qch;
   strMap Mp;
   typename strMap::iterator q,qe=Mp.end();
   SDc.gopen_map();
   for(i=0;i<SDc.ndoc;i++){
      SDc.read(i);
      while(pch=SDc.show(j)){
         if((q=Mp.find(pch))!=qe){
            q->second+=1;
         }
         else {
            lxn=strlen(pch);
            qch=new char[lxn+1];
            strcpy(qch,pch);
            Mp.insert(make_pair(qch,1));
         }
      }
      SDc.clear();
      mark(pflag,(long)i,10000,"documents in");
   }

   long n=(long)Mp.size(),m=(long)SDc.ndoc;
   put_Nnum("z",n,m);

   k=0;
   ofstream *pfout=get_Ostr("f",ios::out);
   q=Mp.begin();
   while(q!=qe){
      i=q->second;
      pfout->write((char*)&i,sizeof(Y));
      q++;
      mark(pflag,++k,10000,"terms");
   }     
   dst_Ostr(pfout);
   if(this->pflag)cout << Mp.size() << " terms in system" << endl;

   this->map_down((FBase *)pSt);
   pSt->create(Mp);
}

template<class Y,class Z>
void XPost<Y,Z>::create_DBinz(Doc<Z> &SDc) {
   long sm=0,i,j,m,n1,n2;
   long *sn,*ss,sp,*px=NULL,*adr;
   Y k;
   Z u;
   const char *pch;

   SDc.gopen_map();
   ndoc=(Y)SDc.ndoc;
   get_Nnum("z",i,j);
   nwrd=(Y)i;
   gopen_hash();
   ofstream *pfw=get_Ostr("wd",ios::out);
   ofstream *pfa=get_Ostr("ad",ios::out);
   ofstream *pfs=get_Ostr("sd",ios::out);
   ofstream *pfx=get_Ostr("xd",ios::out);

   sm=0;
   for(i=0;i<SDc.ndoc;i++) {
      pfa->write((char*)&sm,sizeof(long));
      SDc.read(i);
      k=(Y)SDc.nw;
      pfs->write((char*)&k,sizeof(Y));
      sm+=k;
      while(pch=SDc.show(u)) {
         k=(Y)pCh->count(pch)-1;
         pfw->write((char*)&k,sizeof(Y));
         pfx->write((char*)&u,sizeof(Z));
      }
      SDc.clear();
      mark(pflag,i,10000,"docs");
   }
   dst_Ostr(pfw);
   dst_Ostr(pfa);
   dst_Ostr(pfs);
   dst_Ostr(pfx);
}

template<class Y,class Z>
void XPost<Y,Z>::create_Postz(void) {
   long sm=0,i,j,*adr;
   Y *px,k,m,n,*nc;
   Z *cx,u;

   get_Nnum("z",i,j);
   nwrd=(Y)i;
   ndoc=(Y)j;
   freq=(Y*)get_Mmap("f");
   sm=0;
   adr=new long[i];
   nc=new Y[i];
   for(m=0;m<i;m++) {
      adr[m]=sm;
      sm+=freq[m];
      nc[m]=0;
   }
   px=new Y[sm];
   cx=new Z[sm];

   gopen_db_map();
   for(n=0;n<ndoc;n++) {
      readp_db(n);
      readz_db(n);
      for(m=0;m<nw;m++) {
         k=nwd[m];
         u=lwt[m];
         px[adr[k]+nc[k]]=n;
         cx[adr[k]+nc[k]]=u;
         nc[k]++;
      }
      mark(pflag,(long)n,10000,"docs");
   }
   bin_Writ("a",nwrd*sizeof(long),(char*)adr);
   bin_Writ("p",sm*sizeof(Y),(char*)px);
   bin_Writ("x",sm*sizeof(Z),(char*)cx);
   delete [] px;
   delete [] cx;
   delete [] adr;
   delete [] nc;
   dst_Mmap("f",(char*&)freq);
}         

template<class Y,class Z>
void XPost<Y,Z>::gopen_db_map(void) {
   long i,j;
   get_Nnum("z",i,j);
   nwrd=(Y)i;
   ndoc=(Y)j;
   if(!open1) {
      term=(Y*)get_Mmap("wd");
      if(Exists("xd"))dw=(Z*)get_Mmap("xd");
      addrd=(long*)get_Mmap("ad");
      nsiz=(Y*)get_Mmap("sd");
      open1=1;
   }
}

template<class Y,class Z>
void XPost<Y,Z>::readp_db(Y n) {
   nw=nsiz[n];
   nwd=term+addrd[n];
}

template<class Y,class Z>
void XPost<Y,Z>::readz_db(Y n) {
   nw=nsiz[n];
   lwt=dw+addrd[n];
}

template<class Y,class Z>
void XPost<Y,Z>::gclose_db_map(void) {
   long i;
   if(open1){
      dst_Mmap("wd",(char*&)term);
      if(Exists("xd"))dst_Mmap("xd",(char*&)dw);
      dst_Mmap("ad",(char*&)addrd);
      dst_Mmap("sd",(char*&)nsiz);
      open1=0;
   }
}

template<class Y,class Z>
void XPost<Y,Z>::gopen_map(void) {
   long i,j;
   get_Nnum("z",i,j);
   nwrd=(Y)i;
   ndoc=(Y)j;
   if(!open2){
      freq=(Y*)get_Mmap("f");
      addr=(long*)get_Mmap("a");
      post=(Y*)get_Mmap("p");
      if(Exists("x"))wc=(Z*)get_Mmap("x");
      pInd=new Indx<Y>;
      open2=1;
   }
}

template<class Y,class Z>
Indx<Y> *XPost<Y,Z>::readp(Y n) {
   pInd->ix=freq[n];
   pInd->idx=post+addr[n];
   return(pInd);
}

template<class Y,class Z>
Z *XPost<Y,Z>::readz(Y n) {
   wz=wc+addr[n];
   return(wz);
}

template<class Y,class Z>
void XPost<Y,Z>::gclose_map(void) {
   long i;
   if(open2) {
      dst_Mmap("f",(char*&)freq);
      dst_Mmap("a",(char*&)addr);
      dst_Mmap("p",(char*&)post);
      if(Exists("x"))dst_Mmap("x",(char*&)wc);
      pInd->idx=NULL;
      delete pInd;
      open2=0;
   }
}

template<class Y,class Z>
void XPost<Y,Z>::gopen_hash(void) {
   if(!open3) {
      this->map_down((FBase *)pSt);
      pCh=pSt->gopen_Chash();
      open3=1;
   }
}

template<class Y,class Z>
long XPost<Y,Z>::find(const char *str) {
   return(pCh->count(str));
}

template<class Y,class Z>
void XPost<Y,Z>::gopen_lexos(void) {
   if(!open4) {
      this->map_down((FBase *)pSt);
      pLx=pSt->gopen_Lexos();
      open4=1;
   }
}

template<class Y,class Z>
char *XPost<Y,Z>::show(Y n) {
   return(pLx->show_str((long)n));
}

}
#endif
