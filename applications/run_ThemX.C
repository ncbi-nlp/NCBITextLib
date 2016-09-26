#include <iostream>
#include "ThemX.h"

using namespace std;
using namespace iret;

int main(int argc,char **argv)
{
  char line[10000];
  int posid[10000];
  int idx=0,poscnt=0;
  WOrdr<int,float> *pWord,*qWord;

  // Reads document IDs (XPost) with positive labels
  ifstream pfin("./samples.txt");
  while(pfin.getline(line,10000))
  {
     if(line[0]=='1') posid[poscnt++]=idx;
     idx++;
  }
  pfin.close();

  // pInd below is the Index class that contains a set of int type elements
  // pInd->ix: # of elements
  // pInd->idx[i]: access the value of ith elements
  Indx<int> *pInd=new Indx<int>(poscnt,posid,0);

  cout<<"Seed documents: "<<pInd->ix<<endl;

  // Sets up a Thm class
  ThemX<int,float> *Thm=new ThemX<int,float>("SAMPLE","db");
  Thm->pflag=1;
  Thm->gopen_ThemX();
  Thm->Set_Param(20,5); // Use top 20 terms for computing document scores
                        // 5 is a stringency value

  // Creates a theme based on the given set of documents (pInd)
  qWord=Thm->Initiator(pInd);
  // Obtains a theme by iterating the EM step
  pWord=Thm->Moderator(qWord,10);
  // Prints out top theme terms
  if(pWord->ave_sco(10)>0) Thm->show_terms(pWord);

  delete qWord;
  delete pWord;

  Thm->gclose_ThemX();
  delete Thm;
  delete pInd;

  return 0;
}
