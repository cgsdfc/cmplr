#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H
#include "typedef.h"
#include "utillib/char_buf.h"
#include "utillib/enum.h"
#include "utillib/string.h"
#include <stddef.h>

#define SCANNER_MATCH_ENTRY_BEGIN(NAME)                                        \
  static const scanner_match_entry_t NAME[] = {
#define SCANNER_MATCH_ENTRY_END(NAME)                                          \
  { NULL, 0 }                                                                  \
  }                                                                            \
  ;
#define SCANNER_MATCH_ENTRY(FUNC, VAL) {.scm_func = (FUNC), .scm_val = (VAL)},

#define SCANNER_STR_ENTRY_BEGIN(NAME)                                          \
  static const scanner_str_entry_t NAME[] = {
#define SCANNER_STR_ENTRY_ELEM(STR, VAL) {.se_str = (STR), .se_val = (VAL)},
#define SCANNER_STR_ENTRY_END(NAME)                                            \
  { NULL, 0 }                                                                  \
  }                                                                            \
  ;
#define SCANNER_CHAR_BUF(S) ((S)->sc_char_buf)

UTILLIB_ENUM_BEGIN(scanner_retval_t)
UTILLIB_ENUM_ELEM(SCANNER_MATCHED)
UTILLIB_ENUM_ELEM(SCANNER_ERROR)
UTILLIB_ENUM_ELEM(SCANNER_EOF)
UTILLIB_ENUM_ELEM(SCANNER_UNMATCHED)
UTILLIB_ENUM_ELEM(SCANNER_ANY_CHAR)
UTILLIB_ENUM_END(scanner_retval_t)

UTILLIB_ENUM_BEGIN(scanner_flag_t)
UTILLIB_ENUM_ELEM_INIT(SCANNER_MATCH_STR_AS_ID, 1)
UTILLIB_ENUM_ELEM_INIT(SCANNER_MATCH_ANY_CHAR, 2)
UTILLIB_ENUM_END(scanner_flag_t)

typedef struct scanner_str_entry_t {
  char const *se_str;
  int se_val;
} scanner_str_entry_t;

typedef struct scanner_match_entry_t {
  scanner_match_func_t *scm_func;
  int scm_val;
} scanner_match_entry_t;

typedef struct scanner_base_t {
  /* the underlying buffer providing single char functions */
  void *sc_char_buf;
  utillib_char_buf_ft const *sc_cb_ft;

  /* match functions */
  scanner_match_entry_t const *sc_match;

  /* string tab */
  scanner_str_entry_t const *sc_tab;

  /* internal buffer */
  utillib_string sc_str;

  /* the text of the matched token */
  char const *sc_text;

  /* the code of the matched token */
  int sc_val;

  int sc_flags;
} scanner_base_t;

int scanner_ungetchar(scanner_base_t *, int);
int scanner_getchar(scanner_base_t *);
void scanner_init(scanner_base_t *, void *, struct utillib_char_buf_ft const *,
                 scanner_match_entry_t const *, scanner_str_entry_t const *,
                 int);
int scanner_getc(scanner_base_t *);
void scanner_ungets(scanner_base_t *);
int scanner_ungetc(scanner_base_t *, int);
int scanner_yylex(scanner_base_t *);
char const *scanner_get_text(scanner_base_t *);
int scanner_get_val(scanner_base_t *);
void scanner_destroy(scanner_base_t *);
void scanner_skip_to(scanner_base_t *, int);
#endif
