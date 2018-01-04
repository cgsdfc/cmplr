#include "Token.h"
#include "Symbol.h"
#include <string.h>

void TokenValue::Print(FILE *file) {
  fprintf(file, "TokenValue(location(%u, %u))", location.row, location.col);
}

StringToken::StringToken(Location const& location, char const *string)
  :TokenValue(location), string(strdup(string)) {}

StringToken::~StringToken() { free(string); }

IntegralToken::IntegralToken(Location const &location, int intValue)
    : TokenValue(location), intValue(intValue) {}

OtherToken::OtherToken(Location const &location, unsigned char code)
    : TokenValue(location), code(code) {}

KeywordToken::KeywordToken(Location const &location, unsigned char code)
    : OtherToken(location, code) {
  switch (code) {
  case SYM_KW_CONST:
    name = "const";
    break;
  case SYM_KW_CHAR:
    name = "char";
    break;
  case SYM_KW_VOID:
    name = "void";
    break;
  case SYM_KW_RETURN:
    name = "return";
    break;
  case SYM_KW_FOR:
    name = "for";
    break;
  case SYM_KW_DEFAULT:
    name = "default";
    break;
  case SYM_KW_IF:
    name = "if";
    break;
  case SYM_KW_ELSE:
    name = "else";
    break;
  case SYM_KW_SWITCH:
    name = "switch";
    break;
  case SYM_KW_CASE:
    name = "case";
    break;
  default:
    UNREACHABLE(code);
  }
}
int Identifier::GetType() { return SYM_IDEN; }

int StringLiteral::GetType() { return SYM_STRING; }

int CharLiteral::GetType() { return SYM_CHAR; }

int IntegerLiteral::GetType() { return SYM_INTEGER; }

void Identifier::Print(FILE *file) {
  TokenValue::Print(file);
  fprintf(file, "Identifier(%s)", string);
}

void StringLiteral::Print(FILE *file) {
  TokenValue::Print(file);
  fprintf(file, "StringLiteral('%s')", string);
}

void IntegerLiteral::Print(FILE *file) {
  TokenValue::Print(file);
  fprintf(file, "IntegerLiteral(%d)", intValue);
}

void CharLiteral::Print(FILE *file) {
  TokenValue::Print(file);
  fprintf(file, "CharLiteral('%c')", static_cast<char>(intValue));
}

void KeywordToken::Print(FILE *file) {
  TokenValue::Print(file);
  fprintf(file, "KeywordToken(code=%u, name=%s)", code, name);
}
