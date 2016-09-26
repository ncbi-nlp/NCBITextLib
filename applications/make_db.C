#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <Doc.h>
#include <XPost.h>
#include <Dmap.h>

using namespace std;
using namespace iret;

int gettoken(int i,char *tok,char *line)
{
  int j=0,len=strlen(line);

  if(i>=len) return 0;

  while(!isalnum(line[i])&&i<len) i++;
  while(isalnum(line[i])&&i<len)
  {
     if((line[i]>='A')&&(line[i]<='Z')) tok[j++]=line[i++]+32;
     else                               tok[j++]=line[i++];
  }
  while(!isalnum(line[i])&&i<len) i++;
  tok[j]=0;

  return i;
}

// Other than what tokens to use
// Can use the same routine below for creating a Doc and XPost sets
int main(int argc,char **argv)
{
  char line[10000],tok[1000];
  int i;
  ifstream pfin;
  Dmap<float> *Cl;

  // Reads stopwords and saves in List
  // List is a Btree class for a set of character strings
  Dmap<int> *St=new Dmap<int>;
  pfin.open("./stopwords.txt");
  while(pfin.getline(line,100)) St->add_count(line,1);
  pfin.close();

  cout<<"Stopwords: "<<St->cnt_key<<endl;

  // Creates a Doc set
  // The function below creates the "SAMPLE" Doc set
  // in the path defined in "path_db" (path information should be in this file)
  Doc<float> *Dc=new Doc<float>("SAMPLE","db");
  Dc->gopen_write();

  // Process each document and adds to the Doc set
  pfin.open("./samples.txt");
  while(pfin.getline(line,10000))
  {
     // Adds each token to the Dmap
     // If wants to use feature values other than term frequencies
     // add the other feature values in add_count() below
     Cl=new Dmap<float>;

     i=0;
     while(i=gettoken(i,tok,line+2))
     {
        if(!St->count(tok)) Cl->add_count(tok,1);
     }

     // Adds tokens to the Doc set
     Dc->build(*Cl);
     Dc->write();
     Dc->clear();

     delete Cl;
  }
  pfin.close();
  Dc->gclose_write();

  cout<<"Doc created"<<endl;

  // Creates a XPost set
  // The function below creates the "SAMPLE" XPost set
  // in the path defined in "path_db" (path information should be in this file)
  XPost<int,float> *XP=new XPost<int,float>("SAMPLE","db");
  XP->create_Allz(*Dc);
  delete XP;
  delete Dc;
  delete St;

  cout<<"XPost created"<<endl;

  return 0;
}
