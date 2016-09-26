#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <FBase.h>
#include <Dmap.h>
#include <runn.h>

namespace iret {

class Hash : public FBase {
public:
  Hash(void);
  Hash(const char *nm);
  ~Hash();

  void create_htable(strMap &Mp,int excess); 
     // "str" for file of strings
     // "ad" for address file
     // "nm" numbers
     // "ha" hash array
     // Excess is # powers of 2 above size

  void gopen_htable_map(void);      // Creates memory maps
  void gclose_htable_map(void);     // Destroys memory maps and deletes memory
  void gopen_htable_copy(Hash *pH); // Copies memory maps

  long find(const char *str);
      // Return number+1 if present, else 0
      // Number is not lexical order but hash order and then
      // lexical within collesion groups

  // Data
  char *strmap; // Holds the bit map
  long *addr;   // Holds the offsets to strmap
  long nwrds;   // Number of words
  long tnum;    // Truncation number, size of har
  long *harr;   // Holds hash array
  long *farr;   // Holds the hash coefficients
  long *px0;
  long *px1;
  long *px2;
  long *px3;
  long *px4;
  long *px5;
  long *px6;
  long *px7;
  long *px8;
  long *px9;
  long *px10;
  long *px11;
};

class Chash : public Hash {
public:
  Chash(void);
  Chash(const char *nm);
  ~Chash(void);

  void create_ctable(strMap &Mp,int excess); 
     // Adds "ct" for counts
     // Calls create_htable and then prodoces the array of counts
  void create_ctable_STerm(strMap &Mp,int excess);
     // Adds "ct" for counts. Counts are index of term+1
     // Calls create_htable and then prodoces the array of counts

  void gopen_ctable_map(void);
     // Calls gopen_htable_map and also maps "ct" file
  void gclose_ctable_map(void);
     // Calls gclose_htable_map and also unmaps "ct" file

  long count(const char *str); // Returns count if present, else 0.

  // Data
  long *cnt;
};

}
#endif
