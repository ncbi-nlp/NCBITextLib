#include <Hash.h>

using namespace std;
namespace iret {

Hash::Hash(void) : FBase("hshset","null") {
}

Hash::Hash(const char *nam) : FBase("hshset",nam) {
}

Hash::~Hash() {
}

void Hash::create_htable(strMap &Mp,int excess) {
   char cnam[max_str];
   const char *cptr,*uptr;
   int u,len;
   long ct,i,j,k;
   ofstream *pfout;

   nwrds=Mp.size();
   ct=nwrds;
   tnum=1;
   u=0;
   while(ct=ct/2){tnum*=2;u++;}
   if(u>30){cout << "Error in size, " << u << endl;exit(0);}
   i=0;
   while((u<32)&&(i<excess)){tnum*=2;u++;i++;}
   tnum--;
   harr=new long[tnum+2];
   for(ct=0;ct<tnum+2;ct++)harr[ct]=0;

   farr=new long[3072];
   ct=1;
   for(i=0;i<3072;i++) {
      farr[i]=ct=(ct*331)&tnum;
   }
  
   long *pc0=farr+128,*pc1=farr+384,*pc2=farr+640;
   long *pc3=farr+896,*pc4=farr+1152,*pc5=farr+1408;
   long *pc6=farr+1664,*pc7=farr+1920,*pc8=farr+2176;
   long *pc9=farr+2432,*pc10=farr+2688,*pc11=farr+2944;
   
   typename strMap::iterator p=Mp.begin();
   typename strMap::iterator q=Mp.end();
   while(p!=q) {
      cptr=p->first;
      ct=0;
      i=0;
      while(u=*(cptr++)) {
         switch(i) {
            case 0: ct+=*(pc0+u);
                    break;
            case 1: ct+=*(pc1+u);
                    break;
            case 2: ct+=*(pc2+u);
                    break;
            case 3: ct+=*(pc3+u);
                    break;
            case 4: ct+=*(pc4+u);
                    break;
            case 5: ct+=*(pc5+u);
                    break;
            case 6: ct+=*(pc6+u);
                    break;
            case 7: ct+=*(pc7+u);
                    break;
            case 8: ct+=*(pc8+u);
                    break;
            case 9: ct+=*(pc9+u);
                    break;
            case 10: ct+=*(pc10+u);
                    break;
            case 11: ct+=*(pc11+u);
                     i-=12;
                    break;
         }
         i++;
      }
      (harr[ct&tnum])++;
      p++;
   }

   // Set start points in harr
   k=0;
   for(i=0;i<tnum+2;i++) {
      j=harr[i];
      harr[i]=k;
      k+=j;
   }
   if(k!=nwrds){cout << "Error in summing!" << endl;exit(0);}

   // Write out harr
   bin_Writ("ha",(tnum+2)*sizeof(long),(char*)harr);

   // Set addresses
   const char **addt=new const char*[nwrds];
   p=Mp.begin();
   q=Mp.end();
   while(p!=q) {
      uptr=cptr=p->first;
      ct=0;
      i=0;
      while(u=*(cptr++)) {
         switch(i){
            case 0: ct+=*(pc0+u);
                    break;
            case 1: ct+=*(pc1+u);
                    break;
            case 2: ct+=*(pc2+u);
                    break;
            case 3: ct+=*(pc3+u);
                    break;
            case 4: ct+=*(pc4+u);
                    break;
            case 5: ct+=*(pc5+u);
                    break;
            case 6: ct+=*(pc6+u);
                    break;
            case 7: ct+=*(pc7+u);
                    break;
            case 8: ct+=*(pc8+u);
                    break;
            case 9: ct+=*(pc9+u);
                    break;
            case 10: ct+=*(pc10+u);
                    break;
            case 11: ct+=*(pc11+u);
                     i-=12;
                    break;
         }
         i++;
      }
      k=ct&tnum;
      addt[harr[k]]=uptr;
      (harr[k])++;
      p++;
   }

   // Write out string file
   pfout=get_Ostr("str");
   k=0;
   for(i=0;i<nwrds;i++) {
      *pfout << addt[i] << ends;
      len=strlen((char*)addt[i])+1;
      addt[i]=(char*)k;
      k+=len;
   }
   dst_Ostr(pfout);

   // Write out addr file
   bin_Writ("ad",nwrds*sizeof(long),(char*)addt);
   delete [] addt;
   addt=NULL;

   // Write out counts
   pfout=get_Ostr("nm");
   *pfout << nwrds << " " << tnum << " " << k << endl;
   dst_Ostr(pfout);
   delete [] harr;
   delete [] farr;
   harr=NULL;
   farr=NULL;
}

void Hash::gopen_htable_map(void) {
   char cnam[max_str],*cptr;
   int fld;
   long ct,asize,i;
   
   ifstream *pfin=get_Istr("nm");
   *pfin >> nwrds >> tnum >> asize;
   dst_Istr(pfin);

   harr=(long*)get_Mmap("ha");
   addr=(long*)get_Mmap("ad");
   strmap=get_Mmap("str");

   farr=new long[3072];
   ct=1;
   for(i=0;i<3072;i++) {
      farr[i]=ct=(ct*331)&tnum;
   }
  
   px0=farr+128,px1=farr+384,px2=farr+640;
   px3=farr+896,px4=farr+1152,px5=farr+1408;
   px6=farr+1664,px7=farr+1920,px8=farr+2176;
   px9=farr+2432,px10=farr+2688,px11=farr+2944;
}

void Hash::gopen_htable_copy(Hash *pH) {
   char cnam[max_str],*cptr;
   int fld;
   long ct,asize,i;

   nwrds=pH->nwrds;
   tnum=pH->tnum;

   harr=pH->harr;
   addr=pH->addr;
   strmap=pH->strmap;

   farr=pH->farr;

   px0=farr+128,px1=farr+384,px2=farr+640;
   px3=farr+896,px4=farr+1152,px5=farr+1408;
   px6=farr+1664,px7=farr+1920,px8=farr+2176;
   px9=farr+2432,px10=farr+2688,px11=farr+2944;
}

long Hash::find(const char *str) {
   register long ct=0,i=0,k;
   register int ic;
   register const char *utr=str;
   while(ic=*(utr++)) {
      switch(i) {
         case 0: ct+=*(px0+ic);
                 break;
         case 1: ct+=*(px1+ic);
                 break;
         case 2: ct+=*(px2+ic);
                 break;
         case 3: ct+=*(px3+ic);
                 break;
         case 4: ct+=*(px4+ic);
                 break;
         case 5: ct+=*(px5+ic);
                 break;
         case 6: ct+=*(px6+ic);
                 break;
         case 7: ct+=*(px7+ic);
                 break;
         case 8: ct+=*(px8+ic);
                 break;
         case 9: ct+=*(px9+ic);
                 break;
         case 10: ct+=*(px10+ic);
                 break;
         case 11: ct+=*(px11+ic);
                  i-=12;
                 break;
      }
      i++;
   }
   k=ct&tnum;
   ct=harr[k+1];
   i=harr[k];
   switch(ct-i) {
      case 0: return(0);
              break;
      case 1: if(!strcmp(str,strmap+addr[i]))return(i+1);
              else return(0);
              break;
      case 2: ic=strcmp(str,strmap+addr[i]);
              if(ic>0) {
                 if(!strcmp(str,strmap+addr[i+1]))return(i+2);
                 else return(0);
              }
              else if(ic<0)return(0);
              else return(i+1);
              break;
      default: ic=strcmp(str,strmap+addr[i]);
               if(ic<0)return(0);
               else if(!ic)return(i+1);
               ct--;
               ic=strcmp(str,strmap+addr[ct]);
               if(ic>0)return(0);
               else if(!ic)return(ct+1);
               while(ct-i>1) {
                  k=(ct+i)/2;
                  ic=strcmp(str,strmap+addr[k]);
                  if(ic>0)i=k;
                  else if(ic<0)ct=k;
                  else return(k+1);
               }
               return(0);
   }
}

void Hash::gclose_htable_map(void) {
   dst_Mmap("ha",(char*&)harr);
   dst_Mmap("ad",(char*&)addr);
   dst_Mmap("str",strmap);
   delete [] farr;
   harr=NULL;
   farr=NULL;
   addr=NULL;
   strmap=NULL;
}

Chash::Chash() : Hash() {
   change_type("cshset");
}

Chash::Chash(const char *str) : Hash(str) {
   change_type("cshset");
}

Chash::~Chash(void){}

void Chash::create_ctable(strMap &Mp,int excess) {
   create_htable(Mp,excess);
   gopen_htable_map();
   long n,i=0;
   long *pct=new long[Mp.size()];
   typename strMap::iterator p=Mp.begin();
   typename strMap::iterator q=Mp.end();
   while(p!=q){
      if(n=find(p->first)) {
         pct[n-1]=p->second;
      }        
      else {
         cout << "Error in Map!" << endl;exit(0);
      }
      p++;
      mark(pflag,++i,10000,"count terms");
   }
   bin_Writ("ct",Mp.size()*sizeof(long),(char*)pct);
   delete [] pct;
   gclose_htable_map();
}

void Chash::create_ctable_STerm(strMap &Mp,int excess) {
   create_htable(Mp,excess);
   gopen_htable_map();
   long n,i=1;
   long *pct=new long[Mp.size()];
   typename strMap::iterator p=Mp.begin();
   typename strMap::iterator q=Mp.end();
   while(p!=q) {
      if(n=find(p->first)) {
         pct[n-1]=i;
      }        
      else {
         cout << "Error in Map!" << endl;exit(0);
      }
      p++;
      mark(pflag,++i,10000,"count terms");
   }
   bin_Writ("ct",Mp.size()*sizeof(long),(char*)pct);
   delete [] pct;
   gclose_htable_map();
}

void Chash::gopen_ctable_map(void) {
   gopen_htable_map();
   cnt=(long*)get_Mmap("ct");
}   

void Chash::gclose_ctable_map(void) {
   gclose_htable_map();
   dst_Mmap("ct",(char*&)cnt);
   cnt=NULL;
}   

long Chash::count(const char *str) {
   long n=find(str);
   if(n)return(cnt[n-1]);
   else return(0);
}

}
