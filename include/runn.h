#ifndef RUNN_H
#define RUNN_H

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
namespace iret {

const int word_cnt = 5000; // Maximum number of words in a document
const int word_len = 1500; // Maximum word length
const long max_str = 1500; // Maximum string length

int get_qflag(void);
   // This function gets the value of the print flag pflag
   // that is used to control output
int mark(int pflag,long ct,int ivl,const char *what);
   // This function is used to print out information that indicates 
   // how a function is progressing. It is dependent on the value of pflag
int mark(long ct,int ivl,const char *what);
   // Same as above but not dependent on pflag
long rnd(double p);
   // This function rounds off a double and returns the integer that results

// Reads in a string including white space and
// ends the string just before the character a
inline int get_string(char *cnam,ifstream &ifile,char a) {
   char *pch = cnam;
   long j=1;

   start:
   if((*(pch++)=ifile.get())!=EOF) {
      if(*(pch-1)==a){pch--;goto start;}
      while(((*(pch++)=ifile.get())!=a)&&(j<max_str))j++;
      if(j<max_str) {
         *(--pch)='\0';
         return(j);
      }
      else return(0);
   }
   return(0);
}

// Note that ordering functions beginning with
// sS or hS produce an order that is increasing with increasing index, while
// sR or hR produces the reverse order

template <class X>
void sSort(const long ix,X *idx) {
  long k,j,ir,i;
  X rra;

  if(ix<=1)return;

  k=(ix>>1);
  ir=ix-1;
  for(;;) {
    if(k>0) {
      rra=idx[--k];
    }
    else {
      rra=idx[ir];
      idx[ir] = idx[0];
      if(--ir ==0) {
        idx[0]=rra;
        return;
      }
    }
    i=k;
    j=((k+1)<<1)-1;
    while(j<=ir) {
      if(j<ir && (idx[j]<idx[j+1])) ++j;
      if(rra<idx[j]) {
        idx[i]=idx[j];
        j +=(i=j)+1;
      }
      else j=ir+1;
    }
    idx[i]=rra;
  }
}

template <class X> 
void sRort(const long ix,X *idx) {
  long k,j,ir,i;
  X rra;
   
  if(ix<=1)return;
   
  k=(ix>>1); 
  ir=ix-1;
  for(;;) {
    if(k>0) {
      rra=idx[--k];
    }
    else {
      rra=idx[ir]; 
      idx[ir] = idx[0];
      if(--ir ==0) {
        idx[0]=rra;
        return;
      }
    }
    i=k;
    j=((k+1)<<1)-1;
    while(j<=ir) {
      if(j<ir && (idx[j]>idx[j+1])) ++j;
      if(rra>idx[j]) {
        idx[i]=idx[j];
        j +=(i=j)+1;
      }
      else j=ir+1;
    }
    idx[i]=rra;
  }
}

template <class X, class Y>
void hSort(const long n,X *ra,Y *rb) {
  long k, j, ir, i;
  X rra;
  Y rrb;

  if(n<=1)return;

  k=(n>>1);
  ir=n-1;
  for(;;) {
    if(k>0) {
      rra=ra[--k];
      rrb=rb[k];
    }
    else {
      rra=ra[ir];
      rrb=rb[ir];
      ra[ir] = ra[0];
      rb[ir] = rb[0];
      if(--ir ==0) {
        ra[0]=rra;
        rb[0]=rrb;
        return;
      }
    }
    i=k;
    j=((k+1)<<1)-1;
    while(j<=ir) {
      if(j<ir && ra[j] < ra[j+1]) ++j;
      if(rra<ra[j]) {
        ra[i]=ra[j];
        rb[i]=rb[j];
        j +=(i=j)+1;
      }
      else j=ir+1;
    }
    ra[i]=rra;
    rb[i]=rrb;
  }
}

template <class X, class Y>
void hRort(const long n,X *ra,Y *rb) {
  long k,j,ir,i;
  X rra;
  Y rrb;

  if(n<=1)return;

  k=(n>>1);
  ir=n-1;
  for(;;) {
    if(k>0) {
      rra=ra[--k];
      rrb=rb[k];
    }
    else {
      rra=ra[ir];
      rrb=rb[ir];
      ra[ir] = ra[0];
      rb[ir] = rb[0];
      if(--ir ==0) {
        ra[0]=rra;
        rb[0]=rrb;
        return;
      }
    }
    i=k;
    j=((k+1)<<1)-1;
    while(j<=ir) {
      if(j<ir && ra[j] > ra[j+1]) ++j;
      if(rra>ra[j]) {
        ra[i]=ra[j];
        rb[i]=rb[j];
        j +=(i=j)+1;
      }
      else j=ir+1;
    }
    ra[i]=rra;
    rb[i]=rrb;
  }
}

template<class T>
class SCmp : binary_function<T,T,bool> {
public:
   bool operator() (T s,T t){
      int i = strcmp(s,t);
      if(i<0)return true;
      else return false;
   }
};

}
#endif
