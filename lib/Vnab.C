#include <Vnab.h>

using namespace std;
namespace iret {

double xttrc,xb,xa,xg;

// Local weight functions

float q_const(int lc,long n) {
   return(1.0);
}

float d_lc_func(int lc,long n) {
   return((float)lc);
}

float d_wilbur_med(int lc,long m) {
  float u,v;
  double md;
  if(m<dmt)md=dmt;
  else md=(double)m;
  v=(float)exp(md*0.0044+(lc-1.0)*lfab);
  return(1.0/(1.0+v));
}

float d_bm25(int lc,long m) {
  return((float)lc*2.9/((float)lc+1.9*m/75.3207)); // 75.3207: avg doc length
}

float s_wilbur_med(int lc,unsigned int m) {
  float u,v;
  double md;
  if(m<dmt)md=dmt;
  else md=(double)m;
  v=(float)exp(md*0.0044+(lc-1.0)*lfab);
  return(1.0/(1.0+v));
}

// Global weight functions

float global_const(long n) {
   return(1.0);
}

float global_idf(long n) {
   if(n>dmt)return(l2*log(xttrc/((double)n)));
   else if(n>1)return(l2*log(xttrc/dmt));
   else return(0.0);
}

float global_sidf(long n) {
   if(n>dmt)return(l2*log(xttrc/((double)n)));
   else if(n>0)return(l2*log(xttrc/dmt));
   else return(0.0);
}

float global_bm25(long n) {
   float xx=l2*log((xttrc-n+0.5)/(n+0.5));
   return(xx);
}

float sglobal_const(unsigned int n) {
   return(1.0);
}

float sglobal_idf(unsigned int n) {
   if(n>dmt)return(l2*log(xttrc/((double)n)));
   else if(n>1)return(l2*log(xttrc/dmt));
   else return(0.0);
}

float sglobal_sidf(unsigned int n) {
   if(n>dmt)return(l2*log(xttrc/((double)n)));
   else if(n>0)return(l2*log(xttrc/dmt));
   else return(0.0);
}

}
