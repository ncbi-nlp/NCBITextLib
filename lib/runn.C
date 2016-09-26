#include <runn.h>

using namespace std;
namespace iret {

int get_qflag(void) {
   int pflag=1;
   ifstream fin("quiet.flag",ios::in);
   if(fin.is_open()) {
      fin >> pflag;
      fin.close();
      fin.clear();
   }
   return(pflag);
}

int mark(int pflag,long ct,int ivl,const char *what) {
   if(pflag&&((ct%ivl)==0)) {
      cout << what << " count=" << ct << endl;
      return(1);
   }
   else return(0);
}

int mark(long ct,int ivl,const char *what) {
   if((ct%ivl)==0) {
      cout << what << " count=" << ct << endl;
      return(1);
   }
   else return(0);
}

}
