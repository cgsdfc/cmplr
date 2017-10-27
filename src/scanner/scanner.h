/* 
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
   02110-1301 USA

*/
#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H
#include "input_buf.h"
#include "typedef.h"
#include <stddef.h>
#include <stdio.h>
#include <utillib/enum.h>
#include <utillib/string.h>        // for utillib_string
#include <utillib/unordered_map.h> // utillib_pair_t, utillib_unordered_map

/* create a table for scanner to match against */
#define SCANNER_MATCH_ENTRY_BEGIN(NAME)                                        \
  static const scanner_match_entry_t NAME[] = {
#define SCANNER_MATCH_ENTRY_END(NAME)                                          \
  { NULL, 0 }                                                                  \
  }                                                                            \
  ;
#define SCANNER_MATCH_ENTRY(FUNC, VAL) {.scm_func = (FUNC), .scm_val = (VAL)},
#define SCANNER_STR_ENTRY_STR(ENT) (UTILLIB_PAIR_FIRST(ENT))
#define SCANNER_STR_ENTRY_VAL(ENT) (UTILLIB_PAIR_SECOND(ENT))

// wrapper around array of utillib_pair_t
#define SCANNER_STR_ENTRY_BEGIN(NAME) static const utillib_pair_t NAME[] = {
#define SCANNER_STR_ENTRY_ELEM(STR, VAL)                                       \
  {(utillib_key_t)(STR), (utillib_value_t)(VAL)},
#define SCANNER_STR_ENTRY_END(NAME)                                            \
  { NULL, NULL }                                                               \
  }                                                                            \
  ;
#define SCANNER_CHAR_BUF(S) ((S)->sc_buf)
#define SCANNER_TO_BASE(S) (&((S)->sc_base))
// currently the error code returned when scanner_yylex
// returns SCANNER_ERROR is implemented as the same of
// self->sc_val. but conceptually they are different.
#define scanner_get_errc(S) (scanner_get_val(S))

/* value return from scanner_yylex */
UTILLIB_ENUM_BEGIN(scanner_retval_t)
UTILLIB_ENUM_ELEM(SCANNER_MATCHED)
/* for match_any_char to return */
UTILLIB_ENUM_ELEM(SCANNER_CHAR)
UTILLIB_ENUM_ELEM(SCANNER_ERROR)
UTILLIB_ENUM_ELEM(SCANNER_EOF)
UTILLIB_ENUM_ELEM(SCANNER_UNMATCHED)
UTILLIB_ENUM_END(scanner_retval_t)

typedef struct scanner_match_entry_t {
  /* the function used to mached this token */
  scanner_match_func_t *scm_func;
  /* the value indicating variety of the token */
  int scm_val;
} scanner_match_entry_t;

typedef struct scanner_base_t {
  /* the underlying buffer providing single char functions */
  scanner_input_buf *sc_buf;

  /* the function pointer provids a little flexibility to filter */
  /* the char return by the input_buf */
  scanner_getc_func_t *sc_getc;

  /* match functions */
  scanner_match_entry_t const *sc_match;

  /* internal buffer */
  utillib_string sc_str;

  /* the text of the matched token */
  char const *sc_text;

  /* the code of the matched token */
  int sc_val;
} scanner_base_t;

typedef struct scanner_t {
  scanner_base_t sc_base;
  scanner_input_buf sc_buf;
} scanner_t;

/* constructor destructor */
void scanner_base_init(scanner_base_t *, scanner_input_buf *,
                       scanner_getc_func_t *getc,
                       scanner_match_entry_t const *);
void scanner_init(scanner_t *, FILE *, char const *,
                  scanner_match_entry_t const *);
void scanner_base_destroy(scanner_base_t *);
void scanner_destroy(scanner_t *);

/* getc and ungetc that modifies the internal str */
int scanner_getc(scanner_base_t *);
void scanner_ungets(scanner_base_t *);
int scanner_ungetc(scanner_base_t *, int);

/* getc and ungetc that does not modifies the internal str */
int scanner_ungetchar(scanner_base_t *, int);
int scanner_getchar(scanner_base_t *);

/* read-and-match loop */
int scanner_yylex(scanner_base_t *);
/* get the c_str of the internal str when scanner_yylex returns SCANNER_MATCHED
 */
char const *scanner_get_text(scanner_base_t *);
/* get the variety of the token when scanner_yylex returns SCANNER_MATCHED */
/* or when it returns SCANNER_ERROR, the variety it once tried */
int scanner_get_val(scanner_base_t *);
/* when error happened, this can be used to skip to the occurance of a specific
 * char */
/* the char is then returned */
int scanner_skip_to(scanner_base_t *, int);
int scanner_read_all(scanner_base_t *, FILE *);
#endif
