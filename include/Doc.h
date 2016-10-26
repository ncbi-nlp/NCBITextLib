#ifndef DOC_H
#define DOC_H

#include <fstream>
#include <vector>
#include <FBase.h>
#include <Dmap.h>

using namespace std;
namespace iret {

template<class Z>
class Doc : public FBase {
   public:
      Doc(const char *nm,const char *pnm); 
         // Creates the space for a document to be stored
         // nm: name stored. pnm: path file suffix
      ~Doc();

   // Reading functions
      void gopen_map(void);
         // Mmaps data and sets up w file for read()
         // Set gct=-1
      void read(void);
         // Call gopen_map() before use
         // Reads at the current file pointer position.
         // Increments gct by 1
      void read(long n);
         // Call gopen_map() before use
         // Reads in a document numbered n (starts at 0)
         // Sets gct=n
      void gclose_map(void);
         // Unmaps files 

   // Writing functions
   //
   // Two ways to add a document:
   // 1) open() -> add() -> close() -> write() -> clear()
   // 2) build() -> write() -> clear()
      void gopen_write(void);
         // Sets up for writing data
         // Call gopen_write() before writing and gclose_write() after done
      void open();
         // Creates the map at pMp and enters the document data into it.
      void add(const char *str,Z n);
         // Adds the string into the data and adds its count n
         // During this addition the map at pMp is used.
      void close();
         // Transfers the data into the document structure and deletes *pMp
         // Close for additions to the document
      void build(Dmap<Z> &Mp);
         // Skips open and close. Builds the document structure from Mp
      void write();
         // Writes out a document. Increments the global counter
      void clear();
         // Clears the data for building another new document
         // Sets sizes to zero
      void gclose_write();
         // Writes out the document number and closes associated files

   // Access functions
      void reset() { ct=0; };
         // Resets the counter
      inline const char *show(Z &j) { 
         // Gives the address to a term and its local value is placed in j.
         if(ct<nw) {
            j=lcnt[ct];
            ct++;
            return(word[ct-1]);
         }
         else {
            j=0;
            return(NULL);
         }
      }

   // Global data
      long ndoc;  // Number of documents in set
      long *addr; // Array for addresses of documents

   // File pointers used in writing and accessing data
      fstream *pfwd; // Points at the file stream object for .w file
      ofstream *pfa; // For writing document address for .a file
      long gct;      // Global document counter used in writing documents

   // Local data
      int nw; // Number of words in document
      int ct; // Current word in current document
      vector<const char *> word; // Holds the words in the document
      vector<Z> lcnt; // Holds the local terms frequencies in the document
      char sword[max_str]; // Holds the individual word read from a file
      Dmap<Z> *pMp; // Used with the open, add and close functions
};

template<class Z>
Doc<Z>::Doc(const char *nam,const char *pnam) : FBase("docset",nam,pnam) {
   nw=0;
   open1=0;
   open2=0;
   pMp=NULL;
}

template<class Z>
Doc<Z>::~Doc() {
   if(open2)close();
   for(int i=0;i<nw;i++) {
      delete [] word[i];
   }
   if(pMp)delete pMp;
}

template<class Z>
void Doc<Z>::gopen_map(void) {
   char cnam[max_str];
   ifstream *pfin;

   pfin=get_Istr("n");
   *pfin >> ndoc;
   dst_Istr(pfin);

   if(!open1) {
      addr=(long*)get_Mmap("a");
      pfwd=get_Fstr("w",ios::in);

      nw=0;
      gct=-1;
      open1=1;
   }
   else {
      pfwd->seekg(0, ios::beg);
      gct=-1;
   }
}

template<class Z>
void Doc<Z>::read(void) {
   Z xx;
   char *px;
   *pfwd >> nw;
   pfwd->get();
   for(int i=0;i<nw;i++) {
      *pfwd >> xx;
      lcnt.push_back(xx);
      pfwd->get();
      pfwd->getline(sword,max_str);
      px=new char[strlen(sword)+1];
      strcpy(px,sword);
      word.push_back(px);
   }
   ct=0;
   gct++;
}

template<class Z>
void Doc<Z>::read(long n) {
   pfwd->seekg(*(addr+n));
   read();
}

template<class Z>
void Doc<Z>::gclose_map(void) {
   if(open1) {
      dst_Mmap("a",(char*&)addr);
      dst_Fstr(pfwd);
      open1=0;
   }
}

template<class Z>
void Doc<Z>::gopen_write(void) {
   if(!open1) {
      pfwd=get_Fstr("w",ios::out);
      pfa= get_Ostr("a",ios::out);
      gct=0;
      open1=1;
      nw=ct=0;
      pMp=new Dmap<Z>;
   }
}

template<class Z>
void Doc<Z>::open(void) {
   int i;
   if(pMp==NULL)pMp=new Dmap<Z>;
   if(!open2) {
      for(i=0;i<nw;i++) {
         pMp->insert(make_pair(word[i],lcnt[i]));
      }
      nw=ct=0;
      open2=1;
   }
}

template<class Z>
void Doc<Z>::add(const char *str,Z n){
   pMp->add_count(str,n);
}

template<class Z>
void Doc<Z>::close(void){
   int lxn;
   char *pch;
   if(open2) {
      nw=0;
      pMp->Set();
      while(pMp->qs!=pMp->qz) {
         lcnt.push_back(pMp->qs->second);
         word.push_back(pMp->qs->first);
         nw++;pMp->qs++;
      }
      pMp->clear();
      open2=0;
   }
}

template<class Z>
void Doc<Z>::build(Dmap<Z> &Mp) {
   int lxn;
   char *pch;
   if(open2) {
      close();
      clear();
      open2=0;
   }
   else clear();

   nw=0;
   Mp.Set();
   while(Mp.qs!=Mp.qz) {
      lcnt.push_back(Mp.qs->second);
      word.push_back(Mp.qs->first);
      nw++;Mp.qs++;
   }
   Mp.clear();
}

template<class Z>
void Doc<Z>::write(void) {
   long u=pfwd->tellp();
   pfa->write((char*)&u,sizeof(long));

   *pfwd << nw << endl;
   for(ct=0;ct<nw;ct++)*pfwd << lcnt[ct] << "\t" << word[ct] << endl;
   *pfwd << endl;

   gct++;
}

template<class Z>
void Doc<Z>::clear(void) {
   for(int i=0;i<nw;i++) delete [] word[i];
   lcnt.clear();
   word.clear();
   ct=nw=0;
}

template<class Z>
void Doc<Z>::gclose_write(void) {
   if(open1) {
      ndoc=gct;
      put_Nnum("n",ndoc);

      dst_Fstr(pfwd);
      pfwd=NULL;
      dst_Ostr(pfa);
      pfa=NULL;
      open1=0;
   }
}

}
#endif
