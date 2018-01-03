#include "Scanner.h"
#include "Utility.h"
#include "Error.h"
#include "Symbol.h"
#include "Option.h"
#include <stdio.h>
#include <string.h>

#define CLING_KW_SIZE 14
static const StringIntPair KeywordPairs [] = {
    {"case", SYM_KW_CASE},     {"char", SYM_KW_CHAR},
    {"const", SYM_KW_CONST},   {"default", SYM_KW_DEFAULT},
    {"else", SYM_KW_ELSE},     {"for", SYM_KW_FOR},
    {"if", SYM_KW_IF},         {"int", SYM_KW_INT},
    {"main", SYM_KW_MAIN},     {"printf", SYM_KW_PRINTF},
    {"return", SYM_KW_RETURN}, {"scanf", SYM_KW_SCANF},
    {"switch", SYM_KW_SWITCH}, {"void", SYM_KW_VOID},
};

CharStream::CharStream(FILE *file):file(file), location(){} 

int CharStream::GetChar(void) {
  int ch=fgetc(file);
  switch(ch) {
    case '\n':
      ++location.row;
      location.col=0;
      break;
    case EOF:
      return EOF;
    default:
      ++location.col;
      break;
  }
  return ch;
}

bool CharStream::ReachEOF(void) {
  return feof(file);
}

void TokenValue::print(FILE *file) {
  fprintf(file, "TokenValue(location(%u, %u))\n", location.row, location.col);
}

StringToken::StringToken(Location const& location, char const *string)
  :TokenValue(location), string(strdup(string)) {}

StringToken::~StringToken() { free(string); }

IntegerToken::IntegerToken(Location const& location, int intValue)
  :TokenValue(location), intValue(intValue) {}


Scanner::Scanner(Option const *option, FILE *file, EList *elist)
  :option(option), input(file), elist(elist) {
    this->next_char=input.GetChar();
  }

char const* Scanner::GetString(void) const {
  return buffer.CStr();
}

bool Scanner::IsValidCharInChar(int ch) {
  return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_' ||
      isalnum(ch);
}

bool Scanner::IsValidCharInString(int ch) {
  return ch == 32 || ch == 33 || (35 <= ch && ch <= 126);
}

int Scanner::ReadChar(void) {
  int ch = input.GetChar();
  buffer.AppendChar(ch);
  if (!IsValidCharInChar(ch))  {
    elist->AddError(new InvalidCharError(this, SYM_CHAR));
    return -1;
  }
  if ((ch=input.GetChar()) == '\'') {
    this->next_char=input.GetChar();
    return SYM_CHAR;
  }
  elist->AddError(new UnterminatedTokenError(this, SYM_CHAR));
  return -1;
}

int Scanner::ReadNumber(int ch) {
  while (isdigit(ch)) {
    buffer.AppendChar(ch);
    ch=input.GetChar();
  }
  this->next_char=ch;
  return SYM_INTEGER;
}

bool Scanner::IsStringBreaker(int ch) {
  return ch == EOF || ch == '\r' || ch == '\n';
}

int Scanner::ReadString(void) {
  int ch;
  for (; (ch = input.GetChar()) != '\"';) {
    if (IsStringBreaker(ch)) {
      elist->AddError(new UnterminatedTokenError(this, SYM_STRING));
      return -1;
    }
    buffer.AppendChar(ch);
    if (IsValidCharInString(ch)) {
      continue;
    }
    elist->AddError(new InvalidCharError(this, SYM_STRING));
    return -1;
  }
  this->next_char=input.GetChar();
  return SYM_STRING;
}

int Scanner::ReadToken(void) {
  char ch=this->next_char;
  int code = SYM_ERR;
  char const *keyword;
  int two_chars, one_char;
  switch (ch) {
    /*
     * Level 1 dispatch: single char.
     */
    case '+':
      code = SYM_ADD;
      break;
    case '-':
      code=SYM_MINUS;
      break;
    case '[':
      code = SYM_LK;
      break;
    case ']':
      code = SYM_RK;
      break;
    case '{':
      code = SYM_LB;
      break;
    case '}':
      code = SYM_RB;
      break;
    case '(':
      code = SYM_LP;
      break;
    case ')':
      code = SYM_RP;
      break;
    case ';':
      code = SYM_SEMI;
      break;
    case ',':
      code = SYM_COMMA;
      break;
    case '/':
      code = SYM_DIV;
      break;
    case '*':
      code = SYM_MUL;
      break;
    case ':':
      code = SYM_COLON;
      break;
    default:
      break;
  }
  if (code != SYM_ERR) {
    this->next_char=input.GetChar();
    return code;
  }
  switch (ch) {
    /*
     * Level 2 dispatch: two chars.
     */
    case '=':
      two_chars = SYM_DEQ;
      one_char = SYM_EQ;
      goto level2;
    case '<':
      two_chars = SYM_LE;
      one_char = SYM_LT;
      goto level2;
    case '>':
      two_chars = SYM_GE;
      one_char = SYM_GT;
      goto level2;
    case '!':
      two_chars = SYM_NE;
      one_char = -1;
      goto level2;
    default:
      goto level3;
  }

level2:
  if ((ch=input.GetChar()) == '=') {
    this->next_char=input.GetChar();
    return two_chars;
  }
  this->next_char=ch;
  if (one_char < 0)
    elist->AddError(new BadEqualError(this));
  return one_char;

level3:
  /*
   * Level 3 : keyword and iden.
   */
  if (IsIdenBegin(ch)) {
    while(ch == '_' || isalnum(ch)) {
      buffer.AppendChar(ch);
      ch=input.GetChar();
    }
    this->next_char=ch;
    code = KeywordBseach(KeywordPairs, CLING_KW_SIZE, buffer.CStr());
    if (code > 0)
      return code;
    return SYM_IDEN;
  }
  /*
   * Level 4 : char, string and number
   */
  if (ch == '\"') {
    return ReadString();
  }
  if (ch == '\'') {
    return ReadChar();
  }
  if (isdigit(ch) || ch == '+' || ch == '-')
    return ReadNumber(ch);
  /*
   * Last straw
   */
  buffer.AppendChar(ch);
  elist->AddError(new UnknownTokenError(this));
  return -1;
}

bool Scanner::IsIdenBegin(int ch) {
  return ch == '_' || isalpha(ch);
}
void Scanner::SkipComment(int ch) {
  while (ch == '#') {
    ch=input.GetChar();
    while (ch != '\n')
      ch=input.GetChar();
    while (isspace(ch))
      ch = input.GetChar();
  }
  this->next_char=ch;
}

void Scanner::SkipSpace(int ch) {
  while (isspace(ch)) {
    ch=input.GetChar();
  }
  this->next_char=ch;
}

int Scanner::GetToken(void) {
  SkipSpace(this->next_char);
  if (option->allowComment)
    SkipComment(this->next_char);
  if (this->next_char == EOF)
    return 0;
  buffer.Clear();
  int code=ReadToken();
  if (code < 0) {
    return 0;
  }
  return code;
}

TokenValue *Scanner::GetTokenValue(int type) {
  int intValue;
  char charValue;
  switch(type) {
    case SYM_IDEN:
      return new Identifier(input.location, GetString());
    case SYM_STRING:
      return new StringLiteral(input.location, GetString());
    case SYM_INTEGER:
      sscanf(GetString(), "%d", &intValue);
      return new IntegerLiteral(input.location, intValue);
    case SYM_CHAR:
      sscanf(GetString(), "%c", &charValue);
      return new CharLiteral(input.location, charValue);
    default:
      UNREACHABLE(type);
  }
}
