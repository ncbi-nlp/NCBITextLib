#ifndef DMAP_H
#define DMAP_H

#include <fstream>
#include <iostream>
#include <runn.h>

using namespace std;
namespace iret {

template<class Z>
class Dmap : public std::map<const char *,Z,SCmp<const char *> > {
   public:
      Dmap(void); 
     ~Dmap(void);

   void add_count(const char *str,Z n);
      // Adds the string *str with its count n if not already in list
      // String is a key and count is data
      // If string is already a key the count is incremented by n
   void correct(const char *str,Z n);
      // If str is in the tree the count is changed to n
      // Otherwise nothing is done
   Z count(const char *str);
      // Returns the count if a key (in list) otherwise returns 0

   inline void Set(void) {
      // Sets iterators ready to iterate through the map
      qs=this->begin();
      qz=this->end();
   }

   Z total;      // Holds the total of all counts added for all keys
   long cnt_key; // Holds number of unique keys
   typename map<const char *,Z>::iterator qs,qz;
};

template<class Z>
Dmap<Z>::Dmap() : map<const char *,Z,SCmp<const char *> >() {
   total=0;
   cnt_key=0;
}

template<class Z>
Dmap<Z>::~Dmap() {
}

template<class Z>
void Dmap<Z>::add_count(const char *str,Z n) {
   int lxn;
   char *pch;
   typename map<const char *,Z>::iterator q,qe=this->end();
   if((q=this->find(str))!=qe){
      q->second+=n;
   }
   else {
      lxn=strlen(str);
      pch=new char[lxn+1];
      strcpy(pch,str);
      this->insert(make_pair(pch,n));
      cnt_key++;
   }
   total+=n;
}

template<class Z>
void Dmap<Z>::correct(const char *str,Z n) {
   typename map<const char *,Z>::iterator q,qe=this->end();
   if((q=this->find(str))!=qe) {
      q->second=n;
   }
}

template<class Z>
Z Dmap<Z>::count(const char *str) {
   typename map<const char *,Z>::iterator q,qe=this->end();
   if((q=this->find(str))!=qe) {
      return(q->second);
   }
   else return(0);
}

typedef Dmap<long> strMap;

}
#endif
