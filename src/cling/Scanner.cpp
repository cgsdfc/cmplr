#include "Scanner.h"
#include "Symbol.h"

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

bool ReachEOF(void) {
  return feof(file);
}

Scanner::Scanner(Option const *option, FILE *file):option(option), buffer(file) {}

char const* Scanner::GetString(void) {
  return buffer.CStr();
}

bool Scanner::IsValidChar(int ch) {
  return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_' ||
      isalnum(ch);
}
int Scanner::ReadChar(void) {
  char ch;
  ch = input.GetChar();
  if (IsValidChar(ch))  {
    buffer.AppendChar(ch);
    if ((ch=input.GetChar()) != '\'')
      this->ch=ch;
      return -CL_EUNTCHAR;
    }
    return SYM_CHAR;
  }
  return -CL_ECHRCHAR;
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



}

int Scanner::GetToken(void) {
  char ch=input.GetChar();
  int code = SYM_ERR;
  char const *keyword;
  int two_chars, one_char;
  switch (ch) {
    /*
     * Level 1 dispatch: single char.
     */
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
      one_char = -CL_EBADNEQ;
      goto level2;
    default:
      goto level3;
  }

level2:
  if ((ch=input.GetChar()) == '=') {
    return two_chars;
  }
  return one_char;

level3:
  /*
   * Level 3 : keyword and iden.
   */
  if (IsIdenBegin(ch) {
    utillib_token_scanner_collect_identifier(&self->input, &self->buffer);
    keyword = utillib_string_c_str(&self->buffer);
    code = utillib_keyword_bsearch(keyword, cling_keywords, CLING_KW_SIZE);
    if (code != UT_SYM_NULL)
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
    return read_char();
  }
  if (isdigit(ch) || ch == '+' || ch == '-')
    return ReadNumber(ch);
  return -CL_EUNKNOWN;
}



