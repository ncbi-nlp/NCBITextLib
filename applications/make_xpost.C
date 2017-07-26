#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <Doc.h>
#include <XPost.h>
#include <Vnab.h>

using namespace std;
using namespace iret;

// Creates a XPost set
int main(int argc,char **argv)
{
  // The function below creates the "SAMPLE" XPost set
  // in the path defined in "path_db" (path information should be in this file)
  Doc<float> *Dc=new Doc<float>("SAMPLE","db");
  XPost<int,float> *XP=new XPost<int,float>("SAMPLE","db");

  XP->create_Allz(*Dc); // XPost with local counts
  // Alternatively, can use XP->create_Allz(*Dc,d_local())
  // e.g. XP->create_Allz(*Dc,d_wilbur_med)
  // This also saves local weights and is used for the find_neighbors program
  // Refer to Vnab.h for available local weight functions

  delete XP;
  delete Dc;

  cout<<"XPost created"<<endl;

  return 0;
}
