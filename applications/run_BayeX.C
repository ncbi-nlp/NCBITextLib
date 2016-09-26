#include <iostream>
#include "BayeX.h"

using namespace std;
using namespace iret;

int main(int argc,char *argv[])
{
  char line[10000];
  int posid[10000],negid[10000];
  int i,idx=0,poscnt=0,negcnt=0;

  // Reads positive and negative labels for all documents in XPost
  ifstream pfin("./samples.txt");
  while(pfin.getline(line,10000))
  {
     if(line[0]=='1')      posid[poscnt++]=idx;
     else if(line[0]=='0') negid[negcnt++]=idx;
     idx++;
  }
  pfin.close();

  // pGTR is a positive Index. pBTR is a negative Index
  // pxTR->ix: # of elements
  // pxTR->idx[i]: access the value of ith elements
  Indx<int> *pGTR=new Indx<int>(poscnt,posid,0);
  Indx<int> *pBTR=new Indx<int>(negcnt,negid,0);

  cout<<"Positives: "<<pGTR->ix<<endl;
  cout<<"Negatives: "<<pBTR->ix<<endl;

  BayeX<int,float> *BX=new BayeX<int,float>("SAMPLE","db");
  BX->pflag=0;       // No display for progress
  BX->gopen_BayeX(); // Initializes
  BX->zerot();
  BX->zeros();
  BX->countBX(pGTR); // Counts positives
  BX->countTX(pBTR); // Counts negatives
  BX->Set_All(1);    // Use all terms in XPost. Set mrk to 1
  BX->weightSall();  // Calculates Bayes weights for all terms
  BX->ScoreAll();    // Scores all documents. Saved in sco

  // Prints out 20 highest Bayes weight terms
  int *ord=new int [BX->nwrd];     // For document IDs
  float *weg=new float [BX->nwrd]; // For Bayes weights

  // Two variables: ndoc for # documents, nwrd for # terms in XPost
  for(i=0;i<BX->nwrd;i++)
  {
     ord[i]=i;
     weg[i]=BX->weg[i];
  }
  // Sorts by descending order
  // After hRort, weg is ordered and ord has their corresponding document IDs
  hRort(BX->nwrd,weg,ord);

  // Prints out terms with their weights
  // Returns term names: Call gopen_lexos() first and use show(term ID)
  // Returns term IDs:   Call gopen_hash() first and use find(term name)
  BX->gopen_lexos();
  for(i=0;i<20;i++) cout<<weg[i]<<" "<<BX->show(ord[i])<<endl;

  delete[] ord;
  delete[] weg;

  delete pGTR;
  delete pBTR;
  BX->gclose_BayeX();
  delete BX;

  return 0;
}
