#include <iostream>
#include <Vnab.h>

using namespace std;
using namespace iret;

main(int argc,char **argv)
{
  if(argc!=2) {
     cout<<argv[0]<<" [Document ID]"<<endl;
     return 0;
  }
  long i=atol(argv[1]),j,k;
  float sx;
  Ordr<int,float> *pOrd;

  VnbX<int,float> *Cs=new VnbX<int,float>("SAMPLE","db");
  Cs->gopen_db_map();
  Cs->Load(i); // Loads data for ith document, i.e. for the seed document
  // Score all documents using local weights (from XPost) and
  // global weights (from input parameter)
  // Refer to Vnab.h for available global weight functions
  Cs->ScoreAll(global_idf);
  // Obtains top 10 documents

  pOrd=Cs->Skim(10);
  // Prints out top 10 documents
  // j: rank, k: document ID, sx: score (local weight times global weight)
  for(j=0;j<pOrd->ix;j++)
  {
     k=pOrd->ind(j,sx);
     cout<<j<<" "<<k<<" "<<sx<<endl;
  }
  Cs->gclose_db_map();
  delete Cs;
}
