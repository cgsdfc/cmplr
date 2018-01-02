#ifndef CLING_SCANNER_HXX
#define CLING_SCANNER_HXX
#include "Utility.h"

/*
 * Remember kid, don't play with C++ trick.
 * Use plain old C as a whole.
 */
struct Option;
struct EList;

struct CharStream {
  FILE *file;
  unsigned int row;
  unsigned int col;
  CharStream(FILE *file);
  int GetChar(void);
  bool ReachEOF(void);
};

struct Scanner {
  int ch;
  String buffer;
  CharStream input;
  Option const *option;
  EList *elist;

  Scanner(Option const *option, FILE *file);
  int GetToken(void);
  char const *GetString(void);
  int ReadChar(void);
  int ReadString(void);
  int ReadNumber(void);
  void SkipComment(void);
  bool IsValidChar(int ch);
};

#endif
