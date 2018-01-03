#ifndef CLING_ERROR_HXX
#define CLING_ERROR_HXX
#include <stdio.h>
#include <vector>
#include "Utility.h"
struct Scanner;

struct Error {
  Location location;
  char const *context;
  Error(Scanner const *scanner, int context);
  virtual void print(FILE *file);
  virtual ~Error(){}
};

struct ErrorManager {
  std::vector<Error*> errors;
  void AddError(Error *error);
  bool HasError(void);
  void Print(FILE *file);
  ~ErrorManager();
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
  TokenError(Scanner const *scanner);
  virtual ~TokenError();
};

struct TypedTokenError: public TokenError {
  char const *tokenTypeName;
  TypedTokenError(Scanner const* scanner, int tokenType);
};

struct UnterminatedTokenError: public TypedTokenError {
  UnterminatedTokenError(Scanner const *scanner, int tokenType):TypedTokenError(scanner, tokenType){}
  void print(FILE *file) override;
};

struct InvalidCharError: public TypedTokenError {
  InvalidCharError(Scanner const *scanner, int tokenType):TypedTokenError(scanner, tokenType){}
  void print(FILE *file) override;
};

struct UnknownTokenError: public TokenError {
  UnknownTokenError(Scanner const *scanner):TokenError(scanner){}
  void print(FILE *file) override;
};

struct BadEqualError: public TokenError {
  BadEqualError(Scanner const *scanner):TokenError(scanner){}
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
