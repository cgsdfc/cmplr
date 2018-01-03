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
  Location location;
  CharStream(FILE *file);
  int GetChar(void);
  bool ReachEOF(void);
};

struct Scanner {
  int next_char;
  String buffer;
  CharStream input;
  Option const *option;
  EList *elist;

  Scanner(Option const *option, FILE *file, EList *elist);
  int GetToken(void);
  char const *GetString(void)const ;
  int ReadChar(void);
  int ReadString(void);
  int ReadNumber(int ch);
  int ReadToken(void);
  void SkipComment(int ch);
  void SkipSpace(int ch);
  bool IsIdenBegin(int ch);
  bool IsStringBreaker(int ch);
  bool IsValidCharInChar(int ch);
  bool IsValidCharInString(int ch);
};

#endif
