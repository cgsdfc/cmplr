#include "Error.h"
#include "Utility.h"
#include "Scanner.h"
#include "Symbol.h"
#include <string.h>

Error::Error(Scanner const *scanner, int context)
  :row(scanner->input.row), col(scanner->input.col), context(SymbolString(context)){}

void Error::print(FILE *file) {
  fprintf(file, "ERROR at %u:%u in %s: ", positive_number(row),
          positive_number(col), context);
}

void EList::AddError(Error *error) {
  elist.push_back(error);
}

EList::~EList() {
  for (auto e: elist) {
    GenericDestroy(e);
  }
  GenericDestroy(&elist);
}
  
NameError::NameError(Scanner const *scanner, int context, char const *name)
  : Error(scanner, context), name(strdup(name)) {}

NameError::~NameError() { free(name); }

void RedefinedError::print(FILE *file) {
  NameError::print(file);
  fprintf(file, "name '%s' was redefined\n", name);
}

void UndefinedError::print(FILE *file) {
  NameError::print(file);
  fprintf(file, "undefined reference to '%s'\n", name);
}

ExpectedError::ExpectedError(Scanner const *scanner, int context, int expected, int actual)
  :SyntaxError(scanner, context), expected(SymbolString(expected)), actual(SymbolString(actual)) {}

void ExpectedError::print(FILE *file) {
  SyntaxError::print(file);
  fprintf(file, "expects '%s', but actually gets '%s'\n", expected, actual);
}

UnexpectedError::UnexpectedError(Scanner const *scanner, int context, int unexpected)
  :SyntaxError(scanner, context), unexpected(SymbolString(unexpected)) {}

void UnexpectedError::print(FILE *file) {
  SyntaxError::print(file);
  fprintf(file, "unexpected toke '%s'\n", unexpected);
}

TokenError::TokenError(Scanner const *scanner, int tokenType)
  :Error(scanner, SYM_PROGRAM), tokenType(SymbolString(tokenType)) {
  tokenString=strdup(scanner->GetString());
}

TokenError::~TokenError() { free(tokenString); }

void UnterminatedTokenError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "unterminated %s '%s'\n", tokenType, tokenString);
}
void UnknownTokenError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "unknown token '%s'\n", tokenString);
}
void InvalidCharError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "invalid char '%s' in %s\n", tokenString, tokenType);
}
void BadEqualError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "incompleted '!=' token\n");
}
