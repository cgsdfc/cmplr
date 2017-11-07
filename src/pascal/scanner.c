#include "scanner.h"
#include "symbols.h"
#include <ctype.h>


static const struct utillib_keyword_pair pascal_keywords[] = {
/* This table is sorted */
    {"begin", SYM_KW_BEGIN},
    {"call", SYM_KW_CALL},
    {"const", SYM_KW_CONST},
    {"do", SYM_KW_DO},
    {"end", SYM_KW_END},
    {"if", SYM_KW_IF},
    {"odd", SYM_KW_ODD},
    {"procedure", SYM_KW_PROC}, 
    {"read", SYM_KW_READ},
    {"then", SYM_KW_THEN},
    {"var", SYM_KW_VAR},
    {"while", SYM_KW_WHILE},
    {"write", SYM_KW_WRITE},
};
static const size_t pascal_keywords_size=sizeof pascal_keywords / sizeof pascal_keywords[0];

struct utillib_scanner_op const parser_scanner_op = {
    .lookahead = (void *)pascal_scanner_lookahead,
    .shiftaway = (void *)pascal_scanner_shiftaway,
    .getsymbol = (void *)pascal_scanner_getsymbol,
};

static bool is_idenbegin(int ch) { return ch == '_' || isalpha(ch); }

static bool is_idmiddle(int ch) { return ch == '_' || isalnum(ch); }

static void scanner_collect_char(struct pascal_scanner *self) {
  char ch=utillib_char_scanner_lookahead(&self->char_scanner);
  utillib_string_append_char(&self->char_buffer, ch);
  utillib_char_scanner_shiftaway(&self->char_scanner);
}

#define scanner_test_return(CHAR, SYM) if (ch == CHAR) { \
  self->code=(SYM);\
  return;\
}
static void scanner_getnext(struct pascal_scanner *self) {
  struct utillib_string *char_buffer = &self->char_buffer;
  struct utillib_char_scanner *char_scanner=&self->char_scanner;
  int ch = utillib_char_scanner_lookahead(char_scanner);

  if (is_idenbegin(ch)) {
    scanner_collect_char(self);
    while (is_idmiddle(utillib_char_scanner_lookahead(char_scanner))) {
      scanner_collect_char(self);
    }
    char const * iden=utillib_string_c_str(&self->char_buffer);
    size_t code=utillib_keyword_bsearch(iden, pascal_keywords, pascal_keywords_size);
    self->code=code == UT_SYM_NULL ? SYM_IDEN : code;
    return;
  }
  if (isdigit(ch)) {
    scanner_collect_char(self);
    while (isdigit(utillib_char_scanner_lookahead(char_scanner))) {
      scanner_collect_char(self);
    }
    self->code=SYM_UINT;
    return;
  }
  scanner_test_return('(', SYM_LP)
  scanner_test_return(')', SYM_RP)
  scanner_test_return('+', SYM_ADD)
  scanner_test_return('-', SYM_MINUS)
  scanner_test_return('*', SYM_MUL)
  scanner_test_return('/', SYM_DIV)
  scanner_test_return(',', SYM_COMMA)
  scanner_test_return(';', SYM_SEMI)


}
#undef scanner_test_return

void pascal_scanner_init(struct pascal_scanner *self, FILE * file) {
  utillib_string_init(&self->char_buffer);
  utillib_char_scanner_init(&self->char_scanner, file);
  self->code = 0;
}

size_t pascal_scanner_lookahead(struct pascal_scanner *self) {}
void pascal_scanner_shiftaway(struct pascal_scanner *self) {}
void *pascal_scanner_getsymbol(struct pascal_scanner *self) {}
