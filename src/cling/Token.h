#ifndef CLING_TOKEN_HXX
#define CLING_TOKEN_HXX
#include "Utility.h"

/*
 * Virtual Base Class
 */
struct TokenValue {
  Location location;
  TokenValue(Location const &location) : location(location) {}
  virtual ~TokenValue()=default;
  virtual void Print(FILE *file) = 0;
  virtual int GetType() = 0;
  /* virtual int GetSubClassType()=0; */
};

/*
 * Virtual Base Class
 */
struct StringToken: public TokenValue {
  char *string;
  StringToken(Location const& location, char const *string);
  virtual ~StringToken();
};

/*
 * Virtual Base Class
 */
struct IntegralToken : public TokenValue {
  int intValue;
  IntegralToken(Location const &location, int val);
  ~IntegralToken() = default;
};

/*
 * Virtual Base Class
 */
struct OtherToken : public TokenValue {
  unsigned char code;
  OtherToken(Location const &location, unsigned char code);
  virtual ~OtherToken() = default;
  int GetType() override { return code; }
};

struct KeywordToken : public OtherToken {
  char const *name;
  KeywordToken(Location const &location, unsigned char code);
  ~KeywordToken() = default;
  void Print(FILE *file) override;
  using OtherToken::GetType;
};

struct OperatorToken : public OtherToken {};

struct Punctuation : public OtherToken {};

/*
 * Impl Class
 */
struct Identifier: public StringToken {
  Identifier(Location const& location, char const* iden): StringToken(location, iden){}
  ~Identifier()=default;
  int GetType() override;
  void Print(FILE *file) override;
};

/*
 * Impl Class
 */
struct StringLiteral: public StringToken {
  StringLiteral(Location const& location, char const *string): StringToken(location, string){}
  ~StringLiteral()=default;
  int GetType() override;
  void Print(FILE *file) override;
};

/*
 * Impl Class
 */
struct CharLiteral : public IntegralToken {
  CharLiteral(Location const &location, int ch) : IntegralToken(location, ch) {}
  ~CharLiteral()=default;
  int GetType() override;
  void Print(FILE *file) override;
};

/*
 * Impl Class
 */
struct IntegerLiteral : public IntegralToken {
  IntegerLiteral(Location const &location, int intValue)
      : IntegralToken(location, intValue) {}
  ~IntegerLiteral()=default;
  int GetType() override;
  void Print(FILE *file) override;
};

#endif
