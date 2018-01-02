#include "Scanner.h"
#include "Utility.h"
#include "Error.h"
#include "Symbol.h"
#include "Option.h"
#include <stdio.h>

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

CharStream::CharStream(FILE *file):file(file), row(0), col(0){}

int CharStream::GetChar(void) {
  int ch=fgetc(file);
  switch(ch) {
    case '\n':
      ++row;
      break;
    case EOF:
      return EOF;
    default:
      ++col;
      break;
  }
  return ch;
}

bool CharStream::ReachEOF(void) {
  return feof(file);
}

Scanner::Scanner(Option const *option, FILE *file, EList *elist)
  :option(option), input(file), elist(elist) {
    this->ch=input.GetChar();
  }

char const* Scanner::GetString(void) const {
  return buffer.CStr();
}

bool Scanner::IsValidCharInChar(int ch) {
  return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_' ||
      isalnum(ch);
}

bool Scanner::IsValidCharInString(int ch) {
  return ch == 32 || ch == 33 || 35 <= ch && ch <= 126;
}

int Scanner::ReadChar(void) {
  int ch = input.GetChar();
  if (IsValidCharInChar(ch))  {
    buffer.AppendChar(ch);
    if ((ch=input.GetChar()) != '\'') {
      elist->AddError(new InvalidCharError(this, SYM_CHAR));
      return -1;
    }
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
  this->ch=ch;
  return SYM_INTEGER;
}

int Scanner::ReadString(void) {
  int ch;
  for (; (ch = input.GetChar()) != '\"';) {
    if (ch == EOF) {
      elist->AddError(new UnterminatedTokenError(this, SYM_STRING));
      return -1;
    }
    if (IsValidCharInString(ch)) {
      buffer.AppendChar(ch);
      continue;
    }
    elist->AddError(new InvalidCharError(this, SYM_STRING));
    return -1;
  }
  return SYM_STRING;
}

int Scanner::ReadToken(void) {
  char ch=this->ch;
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
      elist->AddError(new BadEqualError(this));
      goto level2;
    default:
      goto level3;
  }

level2:
  if ((ch=input.GetChar()) == '=') {
    return two_chars;
  }
  this->ch=ch;
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
    this->ch=ch;
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
  elist->AddError(new UnknownTokenError(this));
  return -1;
}

void Scanner::SkipComment(int ch) {
  while (ch == '#') {
    ch=input.GetChar();
    while (ch != '\n')
      ch=input.GetChar();
  }
  this->ch=ch;
}

void Scanner::SkipSpace(int ch) {
  while (isspace(ch)) {
    ch=input.GetChar();
  }
  this->ch=ch;
}

int Scanner::GetToken(void) {
  SkipSpace(this->ch);
  if (option->allow_comment)
    SkipComment(this->ch);
  if (this->ch == EOF)
    return 0;
  buffer.Clear();
  int code=ReadToken();
  if (code < 0) {
    return 0;
  }
  return code;
}

