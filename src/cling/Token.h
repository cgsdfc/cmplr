#ifndef CLING_TOKEN_HXX
#define CLING_TOKEN_HXX
#include "Utility.h"

struct TokenValue {
  Location location;
  TokenValue(Location const& location):location(location){}
  virtual ~TokenValue()=default;
  virtual void print(FILE *file);
};

struct StringToken: public TokenValue {
  char *string;
  StringToken(Location const& location, char const *string);
  virtual ~StringToken();
};

struct IntegerToken: public TokenValue {
  int intValue;
  IntegerToken(Location const& location, int val);
  ~IntegerToken()=default;
};

struct Identifier: public StringToken {
  Identifier(Location const& location, char const* iden): StringToken(location, iden){}
  ~Identifier()=default;
};

struct StringLiteral: public StringToken {
  StringLiteral(Location const& location, char const *string): StringToken(location, string){}
  ~StringLiteral()=default;
};

struct CharLiteral: public IntegerToken {
  CharLiteral(Location const& location, int ch): IntegerToken(location, ch){}
  ~CharLiteral()=default;
};

struct IntegerLiteral: public IntegerToken {
  IntegerLiteral(Location const& location, int intValue): IntegerToken(location, intValue) {}
  ~IntegerLiteral()=default;
};

#endif
