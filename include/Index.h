#ifndef DATAOBJ_H
#define DATAOBJ_H

#include <FBase.h>
#include <runn.h>

using namespace std;
namespace iret {

template<class Z>
class Indx {
   public:
      // ix: # elements
      // idx: elements. ith element can be accessed by idx[i]
      Indx(void);  // Simple constructor
      Indx(Z num); // Sets up memory
      Indx(Z begin,Z end_plus_one);
         // Sets memory
         // Fills with values in the interval [begin,end_plus_one)
      Indx(Z num,Z *imd,long oflag);
         // Sets up memory and populates.
         // Sorts if oflag=1. Sorts and uniques if oflag=2
      Indx(Indx<Z> *ind); // Copy constructor
      ~Indx(void); // Frees memory if pointer not null

      void sSort(void); // Performs a simple heapsort on idx
      long unique(void);
         // Call sSort() before use
         // Checks for duplicates and removes

      Indx<Z> *cbool_And(Indx<Z> *jnd);    // AND with the argument
      Indx<Z> *cbool_Or(Indx<Z> *jnd);     // OR with the argument
      Indx<Z> *cbool_Butnot(Indx<Z> *jnd); // Removes the argument

      Z Subvalue(Z j);
         // Identifies an index by a value
         // On success returns value k+1 such that idx[k]=j
         // Otherwise returns 0
      Indx<Z> *Subvalue(Indx<Z> *jnd);
         // jnd must represent a subset
         // The returned Indx object will give the values k such that
         // idx[k] is among jnd->idx values

      // Subset by threshold sxx[idx] defined
      template<typename U>
      Indx<Z> *Greateq(U thresh,U *sxx);
         // Subset of idx values such that *(sxx+idx)>=thresh

      // Save and load an Indx object
      void Save(const char *nam,const char *p_nam);
         // Saves the current object in "index"_nam.x file
         // p_nam: name for setting path
      void Load(const char *nam,const char *p_nam);
         // Loads the object "index"_nam.x into current object
         // p_nam: name for setting path
      void write(ofstream &fout); // Writes the object into a file
      void read(ifstream &fin);   // Reads the object in from a file

      Z ix;   // Number of elements
      Z *idx; // Elements in order
};

template<class Z>
Indx<Z>::Indx(void) {
   ix=0;
   idx=NULL;
}  
   
template<class Z>
Indx<Z>::Indx(Z num) {
   ix=num;
   idx=new Z[ix];
}  

template<class Z>
Indx<Z>::Indx(Z begin,Z end_plus_one) {
   ix=end_plus_one-begin;
   idx=new Z[ix];
   Z i;
   for(i=0;i<ix;i++)idx[i]=begin+i;
}  
   
template<class Z>
Indx<Z>::Indx(Z num,Z *imd,long oflag) {
   Z i;
      
   ix=num;
   idx=new Z[ix];
   for(i=0;i<num;i++)*(idx+i)=*(imd+i);
   if(oflag)sSort();
   if(oflag>1)unique();
}     
      
template<class Z>
Indx<Z>::Indx(Indx<Z> *ind) {
   Z i;
      
   ix=ind->ix;
   idx=new Z[ix];
   for(i=0;i<ix;i++)idx[i]=ind->idx[i];
}

template<class Z>
Indx<Z>::~Indx(void) {
   if(idx!=NULL)delete [] idx;
}

template<class Z>
void Indx<Z>::sSort(void) {
  Z k, j, ir, i;
  Z rra;

  if(ix<2)return;

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

template<class Z>
long Indx<Z>::unique(void) {
   Z i,k;
   Z j;

   if(ix<2)return(ix);
   k=1;
   j=*idx;
   for(i=1;i<ix;i++) {
      if(j<*(idx+i))*(idx+(k++))=j=*(idx+i);
   }
   if(k<ix) {
      Z *jdx=new Z[k];
      for(i=0;i<k;i++)*(jdx+i)=*(idx+i);
      delete [] idx;
      idx=jdx;
      ix=k;
   }
   return(k);
}

template<class Z>
Indx<Z> *Indx<Z>::cbool_And(Indx<Z> *jnd) {
   Z i,j,k,su,w,p,m,bu;
   Z *pdx,*bdx,*sdx,bx,sx;

   if(jnd==NULL) {
      return(NULL);
   }
   if(ix>jnd->ix) {
      bdx=idx;
      bx=ix;
      sdx=jnd->idx;
      sx=jnd->ix;
   }
   else {
      bdx=jnd->idx;
      bx=jnd->ix;
      sdx=idx;
      sx=ix;
   }
   pdx=new Z[sx];
   i=j=k=0;
   while((i<sx)&&(j<bx)) {
      bu=*(bdx+j);
      while((i<sx)&&(*(sdx+i)<bu))i++;
      if(i<sx) {
         su=*(sdx+i);
         if(su==bu) {
            *(pdx+k)=bu;
            k++;j++;i++;
         }
         else {
            if(bx-j>sx-i)w=(bx-j)/(sx-i);
            else w=1;
            while((j+w<bx)&&(su>*(bdx+j+w)))j+=w;
            if(j+w>=bx) {
               w=bx-j-1;
               if(su>*(bdx+j+w))i=sx;
            }
            if(i<sx) {
               if(su==*(bdx+j+w)) {
                  *(pdx+k)=su;
                  k++;i++;j+=w+1;
               }
               else {
                  p=j+w;
                  while(p-j>1) {
                     m=(j+p)/2;
                     if(su<*(bdx+m))p=m;
                     else j=m;
                  }
                  if(su==*(bdx+j)) {
                     *(pdx+k)=su;
                     k++;
                  }
                  i++;j++;
               }
            }
         }
      }
   }
   if(k==0) {
      delete [] pdx;
      return(NULL);
   }
   else {
      Indx<Z> *pnd=new Indx<Z>(k);
      for(i=0;i<k;i++)*(pnd->idx+i)=*(pdx+i);
      delete [] pdx;
      return(pnd);
   }
}

template<class Z>
Indx<Z> *Indx<Z>::cbool_Or(Indx<Z> *jnd) {
   Z i,j,k,bx,ii,jj;
   Z *iix,*jjx,iu,ju,*pdx;

   if(jnd==NULL) {
      Indx<Z> *pnd=new Indx<Z>(ix,idx,0);
      return(pnd);
   }

   ii=ix;
   iix=idx;
   jj=jnd->ix;
   jjx=jnd->idx;
   bx=ii+jj;
   pdx=new Z[bx];
   i=j=k=0;
   while((i<ii)&&(j<jj)) {
      ju=*(jjx+j);
      while((i<ii)&&((iu=*(iix+i))<ju)) {
         *(pdx+k)=iu;
         k++;i++;
      }
      if(i<ii) {
         if(iu==ju) {
            *(pdx+k)=iu;
            k++;i++;j++;
         }
         else {
            while((j<jj)&&(iu>(ju=*(jjx+j)))) {
               *(pdx+k)=ju;
               k++;j++;
            }
            if(j<jj) {
               if(iu==ju) {
                  *(pdx+k)=iu;
                  k++;i++;j++;
               }
            }
         }
      }
   }
   while(i<ii) {
      *(pdx+k)=*(iix+i);
      k++;i++;
   }
   while(j<jj) {
      *(pdx+k)=*(jjx+j);
      k++;j++;
   }

   if(k==0) {
      delete [] pdx;
      return(NULL);
   }
   else {
      Indx<Z> *pnd=new Indx<Z>(k);
      for(i=0;i<k;i++)*(pnd->idx+i)=*(pdx+i);
      delete [] pdx;
      return(pnd);
   }
}

template<class Z>
Indx<Z> *Indx<Z>::cbool_Butnot(Indx<Z> *jnd) {
   Z i,j,k,w,p,m,flab,bx,sx,*pdx;
   Z *bdx,*sdx,bu,su;

   if(jnd==NULL) {
      Indx<Z> *pnd=new Indx<Z>(ix,idx,0);
      return(pnd);
   }

   if(ix>jnd->ix) {
      bdx=idx;
      bx=ix;
      sdx=jnd->idx;
      sx=jnd->ix;
      flab=1;
   }
   else {
      bdx=jnd->idx;
      bx=jnd->ix;
      sdx=idx;
      sx=ix;
      flab=0;
   }
   pdx=new Z[ix];
   for(i=0;i<ix;i++)*(pdx+i)=1;
   if(flab) { // Case ind is big
   i=j=k=0;
   while((i<sx)&&(j<bx)) {
      bu=*(bdx+j);
      while((i<sx)&&(*(sdx+i)<bu))i++;
      if(i<sx) {
         su=*(sdx+i);
         if(su==bu) {
            *(pdx+j)=0;
            k++;j++;i++;
         }
         else {
            if(bx-j>sx-i)w=(bx-j)/(sx-i);
            else w=1;
            while((j+w<bx)&&(su>*(bdx+j+w)))j+=w;
            if(j+w>=bx) {
               w=bx-j-1;
               if(su>*(bdx+j+w))i=sx;
            }
            if(i<sx) {
               if(su==*(bdx+j+w)) {
                  *(pdx+j+w)=0;
                  k++;i++;j+=w+1;
               }
               else {
                  p=j+w;
                  while(p-j>1) {
                     m=(j+p)/2;
                     if(su<*(bdx+m))p=m;
                     else j=m;
                  }
                  if(su==*(bdx+j)) {
                     *(pdx+j)=0;
                     k++;
                  }
                  i++;j++;
               }
            }
         }
      }
   }
   }
   else { // Case ind is small
   i=j=k=0;
   while((i<sx)&&(j<bx)) {
      bu=*(bdx+j);
      while((i<sx)&&(*(sdx+i)<bu))i++;
      if(i<sx){
         su=*(sdx+i);
         if(su==bu) {
            *(pdx+i)=0;
            k++;j++;i++;
         }
         else {
            if(bx-j>sx-i)w=(bx-j)/(sx-i);
            else w=1;
            while((j+w<bx)&&(su>*(bdx+j+w)))j+=w;
            if(j+w>=bx) {
               w=bx-j-1;
               if(su>*(bdx+j+w))i=sx;
            }
            if(i<sx) {
               if(su==*(bdx+j+w)) {
                  *(pdx+i)=0;
                  k++;i++;j+=w+1;
               }
               else {
                  p=j+w;
                  while(p-j>1) {
                     m=(j+p)/2;
                     if(su<*(bdx+m))p=m;
                     else j=m;
                  }
                  if(su==*(bdx+j)) {
                     *(pdx+i)=0;
                     k++;
                  }
                  i++;j++;
               }
            }
         }
      }
   }
   }

   j=ix-k;
   if(k==0){
      delete [] pdx;
      Indx<Z> *pnd=new Indx<Z>(ix,idx,0);
      return(pnd);
   }
   else if(j==0) {
      delete [] pdx;
      return(NULL);
   }
   else {
      Indx<Z> *pnd=new Indx<Z>(j);
      j=0;
      for(i=0;i<ix;i++) {
         if(*(pdx+i)){
            *(pnd->idx+j)=*(idx+i);
            j++;
         }
      }
      delete [] pdx;
      return(pnd);
   }
}

template<class Z>
Z Indx<Z>::Subvalue(Z j) {
   Z x,y,m;
   Z i,k;

   if(j<=(k=idx[0])) {
      if(j!=k)return(0);
      else return(1);
   }
   if(j>=(k=idx[ix-1])) {
      if(j!=k)return(0);
      else return(ix);
   }
   x=0;
   y=ix-1;
   if(y==1)return(0);

   while(y-x>1) {
      m=(y+x)/2;
      if(j>(k=idx[m]))x=m;
      else if(j<k)y=m;
      else return(m+1);
   }
   return(0);
}

template<class Z>
Indx<Z> *Indx<Z>::Subvalue(Indx *jnd) {
   if(jnd==NULL)return(NULL);
   else if(jnd->ix==0)return(NULL);

   Indx<Z> *knd=new Indx<Z>(jnd->ix);

   Z i=0,j=0;
   Z k;
   while(j<jnd->ix) {
      k=jnd->idx[j];
      while(idx[i]<k)i++;
      knd->idx[(j++)]=i;
   }

   return(knd);
}

template<class Z> template<typename U>
Indx<Z> *Indx<Z>::Greateq(U thresh,U *sxx) {
   Z i,k,ct=0;
   for(i=0;i<ix;i++) {
      if(*(sxx+idx[i])>=thresh)ct++;
   }
   if(!ct)return(NULL);
   Indx<Z> *ind=new Indx<Z>(ct);
   ct=0;
   for(i=0;i<ix;i++) {
      if(*(sxx+(k=idx[i]))>=thresh)ind->idx[ct++]=k;
   }
   return(ind);
}

template<class Z>
void Indx<Z>::Save(const char *nam,const char *p_nam) {
   FBase Fb("index",nam,p_nam);
   ofstream *pfout=Fb.get_Ostr("x",ios::out);
   write(*pfout);
   Fb.dst_Ostr(pfout);
}

template<class Z>
void Indx<Z>::Load(const char *nam,const char *p_nam) {
   FBase Fb("index",nam,p_nam);
   ifstream *pfin=Fb.get_Istr("x",ios::in);
   read(*pfin);
   Fb.dst_Istr(pfin);
}

template<class Z>
void Indx<Z>::write(ofstream &fout) {
   Z i;

   fout << ix << endl;
   for(i=0;i<ix;i++) {
      fout << " " << *(idx+i) << endl;
   }
}

template<class Z>
void Indx<Z>::read(ifstream &fin) {
   Z i;

   if(idx!=NULL)delete [] idx;

   fin >> ix;
   idx=new Z[ix];
   for(i=0;i<ix;i++) {
      fin >> idx[i];
   }
}

template<class Y,class Z>
class Ordr : public Indx<Y> {
   public:
      Ordr(void); // Simple constructor
      Ordr(Y n,Y m,Z *sco);
         // sco[i] is score for index i
         // m is database & sco array size. Produces an Order of size n
      Ordr(Y n,Indx<Y> *ind,Z *sco);
         // sco[ind[i]] is interpretation. Gives the top n
      Ordr(Ordr<Y,Z> *pord); // Copy constructor
      ~Ordr(void);

      // Data access
      Y ind(Y i,Z &sco);
         // Returns doc number (index) and
         // Score pair that correlate and are in decreasing score order
         // as i increases from 0 to num-1.
      Y inv(Y i);
         // Returns the number j with i=order[j], else pInd->ix

      Y *order; // Holds the order of decreasing score
      Z *score; // Holds the scores in the ord order
};

template<class Y,class Z>
Ordr<Y,Z>::Ordr(void) : Indx<Y>() {
   order=NULL;
   score=NULL;
}

template<class Y,class Z>
Ordr<Y,Z>::~Ordr(void) {
   if(order)delete [] order;
   if(score)delete [] score;
}

template<class Y,class Z>
Ordr<Y,Z>::Ordr(Y n,Y m,Z *sco) : Indx<Y>() {
   Y i,j,k,*pt,ir,ii;
   Y u,*ord,*inv;
   Z *scx,ss,xx,*bt,*dt;
   n=(m<n)?m:n;

   if(n<2) {
      if(n<1) {
         this->idx=NULL;
         order=NULL;
         score=NULL;
         return;
      }
      ss=*sco;
      i=1;
      j=0;
      while(i<m) {
         xx=sco[i];
         if(ss<xx) {
            j=i;
            ss=xx;
         }
         i++;
      }
      this->idx=new Y[1];
      *this->idx=j;
      this->ix=n;
      order=new Y[1];
      score=new Z[1];
      *order=0;
      *score=ss;
      return;
   }

   this->idx=new Y[n];
   scx=new Z[n];

   pt=this->idx;
   bt=scx;
   dt=sco;
   for(i=0;i<n;i++) {
      *(pt++)=i;
      *(bt++)=*(dt++);
   }

   // Build the initial heap
   k=(n>>1);
   ir=n-1;
   while(k) {
      ss=scx[(--k)];
      ii=this->idx[k];

      i=k;
      j=((k+1)<<1)-1;
      while(j<=ir) {
         if(j<ir && scx[j]>scx[j+1])++j;
         if(ss>scx[j]) {
            scx[i]=scx[j];
            this->idx[i]=this->idx[j];
            j+=(i=j)+1;
         }
         else j=ir+1;
      }
      scx[i]=ss;
      this->idx[i]=ii;
   }
   // Filter the remaining points into heap
   xx=*scx;
   for(u=n;u<m;u++) {
      if((ss=*(dt++))>xx) {
         ii=u;
         i=0;
         j=1;
         while(j<=ir) {
            if(j<ir && scx[j]>scx[j+1])++j;
            if(ss>scx[j]) {
               scx[i]=scx[j];
               this->idx[i]=this->idx[j];
               j+=(i=j)+1;
            }
            else j=ir+1;
         }
         scx[i]=ss;
         this->idx[i]=ii;
         xx=*scx;
      }
   }
   // Order the heap by promotion & filtering
   for(;;) {
      ss=scx[ir];
      ii=this->idx[ir];
      scx[ir]=scx[0];
      this->idx[ir]=this->idx[0];
      if((--ir)==0) {
         scx[0]=ss;
         this->idx[0]=ii;
         break;
      }
      i=0;
      j=1;
      while(j<=ir) {
         if(j<ir && scx[j]>scx[j+1])++j;
         if(ss>scx[j]) {
            scx[i]=scx[j];
            this->idx[i]=this->idx[j];
            j+=(i=j)+1;
         }
         else j=ir+1;
      }
      scx[i]=ss;
      this->idx[i]=ii;
   }

   ord=new Y[n];
   inv=new Y[n];
   pt=inv;
   for(i=0;i<n;i++)*(pt++)=i;
   hSort(n,this->idx,inv);
   pt=inv;
   for(i=0;i<n;i++)*(ord+*(pt++))=i;
   delete [] inv;

   this->ix=n;
   order=ord;
   score=scx;
}

template<class Y,class Z>
Ordr<Y,Z>::Ordr(Y n,Indx<Y> *ind,Z *sco) : Indx<Y>() {
   Y i,j,k,*pt,ir,ii;
   Y u,*ord,*inv;
   Z *scx,ss,xx,*bt,*dt;
   Y m=ind->ix,*udx=ind->idx;
   n=(m<n)?m:n;

   if(n<2) {
      if(n<1) {
         this->idx=NULL;
         order=NULL;
         score=NULL;
         return;
      }
      ss=sco[udx[0]];
      i=1;
      j=udx[0];
      while(i<m) {
         xx=sco[udx[i]];
         if(ss<xx) {
            j=udx[i];
            ss=xx;
         }
         i++;
      }
      this->idx=new Y[1];
      *this->idx=j;
      this->ix=n;
      order=new Y[1];
      score=new Z[1];
      *order=0;
      *score=ss;
      return;
   }

   this->idx=new Y[n];
   scx=new Z[n];

   pt=this->idx;
   bt=scx;
   for(i=0;i<n;i++) {
      *(pt++)=udx[i];
      *(bt++)=sco[udx[i]];
   }

   // Build the initial heap
   k=(n>>1);
   ir=n-1;
   while(k) {
      ss=scx[(--k)];
      ii=this->idx[k];

      i=k;
      j=((k+1)<<1)-1;
      while(j<=ir) {
         if(j<ir && scx[j]>scx[j+1])++j;
         if(ss>scx[j]) {
            scx[i]=scx[j];
            this->idx[i]=this->idx[j];
            j+=(i=j)+1;
         }
         else j=ir+1;
      }
      scx[i]=ss;
      this->idx[i]=ii;
   }
   // Filter the remaining points into heap
   xx=*scx;
   for(u=n;u<m;u++) {
      if((ss=sco[udx[u]])>xx) {
         ii=udx[u];
         i=0;
         j=1;
         while(j<=ir) {
            if(j<ir && scx[j]>scx[j+1])++j;
            if(ss>scx[j]) {
               scx[i]=scx[j];
               this->idx[i]=this->idx[j];
               j+=(i=j)+1;
            }
            else j=ir+1;
         }
         scx[i]=ss;
         this->idx[i]=ii;
         xx=*scx;
      }
   }
   // Order the heap by promotion & filtering
   for(;;) {
      ss=scx[ir];
      ii=this->idx[ir];
      scx[ir]=scx[0];
      this->idx[ir]=this->idx[0];
      if((--ir)==0) {
         scx[0]=ss;
         this->idx[0]=ii;
         break;
      }
      i=0;
      j=1;
      while(j<=ir) {
         if(j<ir && scx[j]>scx[j+1])++j;
         if(ss>scx[j]) {
            scx[i]=scx[j];
            this->idx[i]=this->idx[j];
            j+=(i=j)+1;
         }
         else j=ir+1;
      }
      scx[i]=ss;
      this->idx[i]=ii;
   }

   ord=new Y[n];
   inv=new Y[n];
   pt=inv;
   for(i=0;i<n;i++)*(pt++)=i;
   hSort(n,this->idx,inv);
   pt=inv;
   for(i=0;i<n;i++)*(ord+*(pt++))=i;
   delete [] inv;

   this->ix=n;
   order=ord;
   score=scx;
}

template<class Y,class Z>
Ordr<Y,Z>::Ordr(Ordr<Y,Z> *pOrd) : Indx<Y>() {
   Y i;

   this->ix=pOrd->ix;
   this->idx=new Y[pOrd->ix];
   order=new Y[pOrd->ix];
   score=new Z[pOrd->ix];
   for(i=0;i<pOrd->ix;i++) {
      this->idx[i]=pOrd->idx[i];
      order[i]=pOrd->order[i];
      score[i]=pOrd->score[i];
   }
}

template<class Y,class Z>
Y Ordr<Y,Z>::ind(Y i,Z &sco) {
   sco=*(score+i);
   return(*(this->idx+*(order+i)));
}

template<class Y,class Z>
Y Ordr<Y,Z>::inv(Y i) {
   Y j=0;
   while((j<this->ix)&&(i!=order[j]))j++;
   return(j);
}

template<class Y>
class Store {
public:
   Store(long n); // Size of array. Creates array wx
   ~Store(void);

   void CopyIn(Y *u);  // Makes a copy of u
   void CopyOut(Y *u); // Copys from memory to u

   long num; // Size of array wx
   Y *wx;    // Memory for storage
};

template<class Y>
Store<Y>::Store(long n) {
   num=n;
   wx=new Y[n];
}

template<class Y>
Store<Y>::~Store(void) {
   delete [] wx;
}

template<class Y>
void Store<Y>::CopyIn(Y *u) {
   long i;
   for(i=0;i<num;i++)wx[i]=u[i];
}

template<class Y>
void Store<Y>::CopyOut(Y *u) {
   long i;
   for(i=0;i<num;i++)u[i]=wx[i];
}

// This class is for ThemX
template<class Y,class Z>
class WOrdr : public Ordr<Y,Z> {
  public:
   WOrdr(void);
   WOrdr(Ordr<Y,Z> *pOrd,Z *weg);
      // Uses weg[pOrd->idx[order[i]]] to obtain the weights for terms
      // ith weight corresponds to ind(i,.)
      // Constructor destroys the Order object pointed to by pOrd:
      // A case of catabolism.
   ~WOrdr(void);

   Z ave_sco(Y n); // Averages the n top scores

   Z *weight;
};

template<class Y,class Z>
WOrdr<Y,Z>::WOrdr(void) : Ordr<Y,Z>() {
   weight=NULL;
}

template<class Y,class Z>
WOrdr<Y,Z>::WOrdr(Ordr<Y,Z> *pOrd,Z *weg) {
   if(!pOrd){cout << "Error, pOrd is NULL!" << endl;exit(0);}
   this->idx=pOrd->idx;
   this->ix=pOrd->ix;
   pOrd->idx=NULL;
   this->order=pOrd->order;
   pOrd->order=NULL;
   this->score=pOrd->score;
   pOrd->score=NULL;

   Y i;
   weight=new Z[this->ix];
   for(i=0;i<this->ix;i++)weight[i]=weg[this->idx[this->order[i]]];
   delete pOrd;
}

template<class Y,class Z>
WOrdr<Y,Z>::~WOrdr(void) {
   if(weight!=NULL)delete [] weight;
}

template<class Y,class Z>
Z WOrdr<Y,Z>::ave_sco(Y n) {
   Y i;
   Z xx,sum=0;

   for(i=0;i<n;i++) {
      this->ind(i,xx);
      sum+=xx;
   }
   return(sum/((Z)n));
}

}
#endif
