#ifndef CLING_ERROR_HXX
#define CLING_ERROR_HXX
#include <stdio.h>
#include <vector>

struct Scanner;
enum {
  CL_EEXPECT = 1,
  CL_EUNEXPECTED,
  CL_EREDEFINED,
  CL_EINCTYPE,
  CL_EUNDEFINED,
  CL_ENOTLVALUE,
  CL_EINCARG,
  CL_EARGCUNMAT,
  CL_EDUPCASE,
  CL_EBADCASE,
  CL_EINVEXPR,
  CL_EBADTOKEN,
};

struct Error {
  unsigned int row;
  unsigned int col;
  char const *context;
  Error(Scanner const *scanner, int context);
  virtual void print(FILE *file);
  virtual ~Error();
};

struct EList {
  std::vector<Error*> elist;
  void AddError(Error *error);
  ~EList();
};

struct NameError: public Error {
  char *name;
  NameError(Scanner const* scanner, int context, char const *name);
  virtual ~NameError();
};

struct SyntaxError: public Error {
  SyntaxError(Scanner const* scanner, int context): Error(scanner, context){}
};

struct TokenError: public Error {
  char *tokenString;
  char const *tokenType;
  TokenError(Scanner const *scanner, int tokenType);
  virtual ~TokenError();
};

struct UnknownTokenError: public TokenError {
  UnknownTokenError(Scanner const *scanner):TokenError(scanner, 0){}
  void print(FILE *file) override;
};

struct UnterminatedTokenError: public TokenError {
  UnterminatedTokenError(Scanner const *scanner, int tokenType):TokenError(scanner, tokenType){}
  void print(FILE *file) override;
};

struct InvalidCharError: public TokenError {
  InvalidCharError(Scanner const *scanner, int tokenType):TokenError(scanner, tokenType){}
  void print(FILE *file) override;
};

struct BadEqualError: public TokenError {
  BadEqualError(Scanner const *scanner):TokenError(scanner, 0){}
  void print(FILE *file) override;
};

struct ExpectedError: public SyntaxError {
  char const *expected;
  char const *actual;
  ExpectedError(Scanner const *scanner, int context, int expected, int actual);
  void print(FILE *file) override;
};

struct UnexpectedError: public SyntaxError {
  char const *unexpected;
  UnexpectedError(Scanner const *scanner, int context, int unexpected);
  void print(FILE *file) override;
};

struct RedefinedError: public NameError {
  void print(FILE *file) override;
};

struct UndefinedError: public NameError {
  void print(FILE *file) override;
};

#endif
