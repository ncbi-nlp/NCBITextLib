#include <iostream>
#include "HubeX.h"

using namespace std;
using namespace iret;

int main(int argc, char *argv[])
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

  HubeX<int,float> *HX=new HubeX<int,float>("SAMPLE","db");
  HX->pflag=0;             // No display for progress
  HX->gopen_HubeX();       // Initializes
  HX->zerot();
  HX->zeros();
  HX->Set_Docs(pGTR,pBTR); // Set positives and negatives for SVM learning
  HX->countBX(pGTR);       // Counts positives
  HX->countTX(pBTR);       // Counts negatives
  HX->Set_Term_freq(0);    // Sets mrk to 1 for all terms in XPost
  HX->Set_Lambda_Norm(0.00001); // Set lambda
  HX->Learn();             // Learns
  HX->ScoreAll();          // Scores all documents. Saved in sco

  // Prints out 20 highest SVM weight terms
  int *ord=new int [HX->nwrd];     // For document IDs
  float *weg=new float [HX->nwrd]; // For SVM weights

  // Two variables: ndoc for # documents, nwrd for # terms in XPost
  for(i=0;i<HX->nwrd;i++)
  {
     ord[i]=i;
     weg[i]=HX->wt[i];
  }
  // Sorts by descending order
  // After hRort, weg is ordered and ord has their corresponding document IDs
  hRort(HX->nwrd,weg,ord);

  // Prints out terms with their weights
  // Returns term names: Call gopen_lexos() first and use show(term ID)
  // Returns term IDs:   Call gopen_hash() first and use find(term name)
  HX->gopen_lexos();
  for(i=0;i<20;i++) cout<<weg[i]<<" "<<HX->show(ord[i])<<endl;

  delete[] ord;
  delete[] weg;

  delete pGTR;
  delete pBTR;
  HX->gclose_HubeX();
  delete HX;

  return 0;
}
