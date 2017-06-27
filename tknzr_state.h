#ifndef TKNZR_STATE_H
#define TKNZR_STATE_H 1
#include <stdbool.h>
#include <stdio.h>
#define N_MARKER_STATES 5
#define N_TOKENIZER_ROWS (_TK_NON_ACCEPTED_END-TK_INIT)
#define N_TOKENIZER_COLS (TK_NULL - TK_INIT-N_MARKER_STATES)
#include "char_class.h"


typedef enum tknzr_state
{
  TK_INIT=0,

  _TK_VARLEN_INIT_BEGIN,
  TK_CHAR_LITERAL_BEGIN,
  TK_STRING_LITERAL_BEGIN,
  TK_IDENTIFIER_BEGIN,
  TK_INT_ZERO,
  TK_INT_DEC_BEGIN,
  _TK_VARLEN_INIT_END,

  /* coments */
  TK_SINGLE_LINE_COMENT_BEGIN,
  TK_MULTI_LINE_COMENT_BEGIN,
  TK_SINGLE_LINE_COMENT_END,
  TK_MULTI_LINE_COMENT_END,
  /*!coments */

  /* 2 state */
  TK_TILDE,
  TK_PERIOD,

  /* 3 states */
  TK_EXCLAIM,
  TK_EQUAL,
  TK_STAR,
  TK_PERCENT,
  TK_CARET,
  TK_SLASH,

  /* 4 states */
  TK_AMPERSAND,
  TK_VERTICAL_BAR,
  TK_POSITIVE,
  TK_NEGATIVE,

  /* 6 states */
  TK_LESS,
  TK_GREATER,
  TK_GREATER_GREATER,
  TK_LESS_LESS,


  TK_INT_HEX_PREFIX,
  TK_INT_HEX_BEGIN,
  TK_INT_OCT_BEGIN,
  TK_INT_SUFFIX_0,
  TK_INT_SUFFIX_1,

  TK_FLOAT_FRACTION,
  TK_FLOAT_EXPONENT_BEGIN,
  TK_FLOAT_EXPONENT,
  TK_FLOAT_SIGN,
  TK_FLOAT_SUFFIX_0,

  TK_CHAR_LITERAL_ESCAPED,
  TK_CHAR_LITERAL_PART,

  TK_STRING_LITERAL_ESCAPED,

  _TK_NON_ACCEPTED_END,


  _TK_VARLEN_ACCEPT_BEGIN,
  TK_IDENTIFIER_END,
  TK_INT_END,
  TK_FLOAT_END,
  TK_CHAR_LITERAL_END,
  TK_STRING_LITERAL_END,
  _TK_VARLEN_ACCEPT_END,


  _TK_OPERATOR_ACCEPT_BEGIN,
  /* 1 state */
  TK_TILDE_END,

  /* 3 states */
  TK_EXCLAIM_END,
  TK_EQUAL_END,
  TK_STAR_END,
  TK_PERCENT_END,
  TK_CARET_END,
  TK_SLASH_END,

  /* 4 states */
  TK_AMPERSAND_END,
  TK_VERTICAL_BAR_END,
  TK_POSITIVE_END,
  TK_NEGATIVE_END,

  /* 6 states */
  TK_GREATER_END,
  TK_GREATER_GREATER_END,
  TK_LESS_END,
  TK_LESS_LESS_END,

  TK_AMPERSAND_AMPERSAND,
  TK_LESS_LESS_EQUAL,
  TK_GREATER_GREATER_EQUAL,
  TK_VERTICAL_BAR_BAR,
  TK_POSITIVE_POSITIVE,
  TK_NEGATIVE_NEGATIVE,
  TK_NEGATIVE_GREATER,

  TK_EXCLAIM_EQUAL,
  TK_PERCENT_EQUAL,
  TK_CARET_EQUAL,
  TK_AMPERSAND_EQUAL,
  TK_STAR_EQUAL,
  TK_POSITIVE_EQUAL,
  TK_NEGATIVE_EQUAL,
  TK_NEGATIVE_LESS,
  TK_LESS_EQUAL,
  TK_GREATER_EQUAL,
  TK_EQUAL_EQUAL,
  TK_SLASH_EQUAL,
  TK_VERTICAL_BAR_EQUAL,
  _TK_OPERATOR_ACCEPT_END,

  TK_PERIOD_END,
  TK_NULL 
} tknzr_state;

// todo: entry_t and node 
// usage is too easily to mix
// but one is signed another is
// unsigned.
// especially after I changed st_do_transfer
// to report error by returning a -1 node
// check this!
bool is_varlen_accept (tknzr_state state);
bool is_varlen_init (tknzr_state state);
bool is_operator_accept (tknzr_state state);
bool is_operator_char(char ch);
bool is_punctuation_accept (char ch);
bool is_punctuation_char (char ch);
char *tknzr_state_string(tknzr_state state);

extern const char *tknzr_state_tab[];
#endif

