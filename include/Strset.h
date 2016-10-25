#ifndef STRSET_H
#define STRSET_H

#include <fstream>
#include <FBase.h>
#include <Dmap.h>

#define MAP_F 1 // Stands for the Strset files .n, .a, and .s

using namespace std;
namespace iret {

class Strset : public FBase {
public:
   Strset(const char *nm);
   ~Strset();

   // Writing functions
   void gopen_write(void); 
   void add_str(const char *pch); // Call to add a string to the set
   void gclose_write(void);       // Files are .n, .a, and .s

   // Reading functions
   void gopen_map(void);
   char *show_str(long n); // Returns pointer at the nth string
   void gclose_map(void);
  
   // Data
   long num;      // Number of strings in the set (file .n)
   long *addr;    // Array of offsets to strings (file .a)
   char *str;     // Pointer at string map (file .s)

   ofstream *pfa; // Used for file creation
   ofstream *pfs; // Used for file creation
};

class Lexos : public Strset {
public:
   Lexos(const char *nm);
   ~Lexos();

   void create_Lexos(strMap &Mp);
   long find(const char *str);
      // If finds string returns its number+1, else returns 0
   int stc_my(const char *,const char *);
      // Function used to compare two strings

   // Data
   int a; // Variable used for comparison of strings
   int b; // Variable used for comparison of strings
};

}
#endif

