#include <Strset.h>

using namespace std;
namespace iret {

Strset::Strset(const char *nam) : FBase("strset",nam) {
}

Strset::~Strset(void) {
}

void Strset::gopen_write(void) {
   num=0;
   pfa=get_Ostr("a");
   pfs=get_Ostr("s");
}

void Strset::add_str(const char *pch) {
   long i=pfs->tellp();   
   pfa->write((const char*)&i,sizeof(long));
   *pfs << pch << ends;
   num++;
}

void Strset::gclose_write(void) {
   ofstream *pfn=get_Ostr("n");
   *pfn << num << endl;
   dst_Ostr(pfn);
   dst_Ostr(pfa);
   dst_Ostr(pfs);
}

void Strset::gopen_map(void) {
   cflag=1;
   if(Gcom(MAP_F)) {
      ifstream *pfn=get_Istr("n");
      *pfn >> num;
      dst_Istr(pfn);

      addr=(long*)get_Mmap("a");
      str=get_Mmap("s");
   }
}

char *Strset::show_str(long n) {
   if(n<0)return(NULL);
   if(n>=num)return(NULL);
   return(str+addr[n]);
}

void Strset::gclose_map(void) {
   cflag=1;
   if(Rcom(MAP_F)) {
      dst_Mmap("a",(char*&)addr);
      dst_Mmap("s",str);
   }
}

Lexos::Lexos(const char *nam) : Strset(nam) {
   this->change_type("lexos");
}

Lexos::~Lexos(void) {
}

void Lexos::create_Lexos(strMap &Mp) {
   long i=0;
   this->gopen_write();
   typename strMap::iterator p=Mp.begin();
   typename strMap::iterator q=Mp.end();
   while(p!=q) {
      this->add_str(p->first);
      p++;
      mark(pflag,++i,10000,"strings out");
   }
   this->gclose_write();
}

long Lexos::find(const char *ssr) {
   int j;
   a=b=0;
   if((j=stc_my(ssr,str+addr[0]))<0)return(0);
   else if(j==0)return(1);

   if((j=stc_my(ssr,str+addr[num-1]))>0)return(0);
   else if(j==0)return(num);

   long i,x=0,y=num-1;
   while(y-x>1) {
      i=(y+x)/2;
      if((j=stc_my(ssr,str+addr[i]))==0)return(i+1);
      else if(j<0)y=i;
      else x=i;
   }
   return(0);
}

int Lexos::stc_my(const char *ssr,const char *ptr) {
   register int i=(a<b) ? a : b;
   register const char *p1=ssr+i;
   register const char *p2=ptr+i;
   register int j=0;
   while((*p1==*p2)&&(*p1!='\0')) {
      j++;
      p1++;
      p2++;
   }
   if(*p1==*p2)return(0);
   else if(*p1<*p2) {
      b=i+j;
      return(-1);
   }
   else {
      a=i+j;
      return(1);
   }
}

}
