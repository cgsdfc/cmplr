#include "Error.h"
#include "Utility.h"
#include "Scanner.h"
#include "Symbol.h"
#include <string.h>

Error::Error(Scanner const *scanner, int context)
  :location(scanner->input.location), context(SymbolString(context)){}

void Error::print(FILE *file) {
  fprintf(file, "ERROR at %u:%u in %s: ", location.row, location.col, context);
}

void ErrorManager::AddError(Error *error) {
  errors.push_back(error);
}

bool ErrorManager::HasError(void) {
  return !errors.empty();
}

void ErrorManager::Print(FILE *file) {
  for (auto e: errors) {
    e->print(file);
  }
  fprintf(file, "%lu errors generated\n", errors.size());
}

ErrorManager::~ErrorManager() {
  for (auto e:errors) {
    e->~Error();
  }
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

TokenError::TokenError(Scanner const *scanner)
  :Error(scanner, SYM_PROGRAM), 
  tokenString(strdup(scanner->GetString())){}

TokenError::~TokenError() { free(tokenString); }

TypedTokenError::TypedTokenError(Scanner const* scanner, int tokenType)
    :TokenError(scanner), tokenTypeName(SymbolString(tokenType)) {}

void UnterminatedTokenError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "unterminated %s '%s'\n", tokenTypeName, tokenString);
}
void UnknownTokenError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "unknown token '%s'\n", tokenString);
}
void InvalidCharError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "invalid char '%s' in %s\n", tokenString, tokenTypeName);
}
void BadEqualError::print(FILE *file) {
  TokenError::print(file);
  fprintf(file, "incompleted '!=' token\n");
}
