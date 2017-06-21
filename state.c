#include "state.h"

const char *token_state_tab[]=
{
  
  [TK_INIT]="TK_INIT",
  [TK_IDENTIFIER_BEGIN]="TK_IDENTIFIER_BEGIN",
  [TK_IDENTIFIER_END]="TK_IDENTIFIER_END",
  [TK_PUNCTUATION_BEGIN]="TK_PUNCTUATION_BEGIN",
  [TK_PUNCTUATION_END]="TK_PUNCTUATION_END",

  [TK_INT_DEC_BEGIN]="TK_INT_DEC_BEGIN",
  [TK_INT_HEX_OCT_BEGIN]="TK_INT_HEX_OCT_BEGIN",
  [TK_INT_HEX_BEGIN]="TK_INT_HEX_BEGIN",
  [TK_INT_OCT_BEGIN]="TK_INT_OCT_BEGIN",
  [TK_INT_END]="TK_INT_END",
  [TK_INT_LONG]="TK_INT_LONG",
  [TK_INT_UNSIGNED]="TK_INT_UNSIGNED",
  
  [TK_SINGLE_LINE_COMENT_BEGIN]="TK_SINGLE_LINE_COMENT_BEGIN",
  [TK_SINGLE_LINE_COMENT_END]="TK_SINGLE_LINE_COMENT_END",

  [TK_MULTI_LINE_COMENT_BEGIN]="TK_MULTI_LINE_COMENT_BEGIN",
  [TK_BAD_MULTI_LINE_COMENT]="TK_BAD_MULTI_LINE_COMENT",
  [TK_MULTI_LINE_COMENT_END]="TK_MULTI_LINE_COMENT_END",


  [TK_TILDE]="TK_TILDE",
  [TK_EXCLAIM]="TK_EXCLAIM",
  [TK_PERCENT]="TK_PERCENT",
  [TK_CARET]="TK_CARET",
  [TK_AMPERSAND]="TK_AMPERSAND",
  [TK_STAR]="TK_STAR",
  [TK_POSITIVE]="TK_POSITIVE",
  [TK_NEGATIVE]="TK_NEGATIVE",
  [TK_LESS]="TK_LESS",
  [TK_GREATER]="TK_GREATER",
  [TK_EQUAL]="TK_EQUAL",
  [TK_SLASH]="TK_SLASH",
  [TK_VERTICAL_BAR]="TK_VERTICAL_BAR",


  
  [TK_EXCLAIM_EQUAL]="TK_EXCLAIM_EQUAL",
  [TK_PERCENT_EQUAL]="TK_PERCENT_EQUAL",
  [TK_CARET_EQUAL]="TK_CARET_EQUAL",
  [TK_AMPERSAND_EQUAL]="TK_AMPERSAND_EQUAL",
  [TK_STAR_EQUAL]="TK_STAR_EQUAL",
  [TK_POSITIVE_EQUAL]="TK_POSITIVE_EQUAL",
  [TK_NEGATIVE_EQUAL]="TK_NEGATIVE_EQUAL",
  [TK_LESS_EQUAL]="TK_LESS_EQUAL",
  [TK_GREATER_EQUAL]="TK_GREATER_EQUAL",
  [TK_EQUAL_EQUAL]="TK_EQUAL_EQUAL",
  [TK_SLASH_EQUAL]="TK_SLASH_EQUAL",
  [TK_VERTICAL_BAR_EQUAL]="TK_VERTICAL_BAR_EQUAL",


  [TK_CHAR_LITERAL_BEGIN]="TK_CHAR_LITERAL_BEGIN",
  [TK_CHAR_LITERAL_PART]="TK_CHAR_LITERAL_PART",
  [TK_CHAR_LITERAL_END]="TK_CHAR_LITERAL_END",
  [TK_BAD_CHAR_LITERAL]="TK_BAD_CHAR_LITERAL",
  [TK_CHAR_LITERAL_ESCAPED]="TK_CHAR_LITERAL_ESCAPED",
  [TK_CHAR_LITERAL_OCT_BEGIN]="TK_CHAR_LITERAL_OCT_BEGIN",
  [TK_CHAR_LITERAL_OCT_END]="TK_CHAR_LITERAL_OCT_END",
  [TK_CHAR_LITERAL_HEX_BEGIN]="TK_CHAR_LITERAL_HEX_BEGIN",
  [TK_CHAR_LITERAL_HEX_END]="TK_CHAR_LITERAL_HEX_END",
  [TK_CHAR_LITERAL_ZERO]="TK_CHAR_LITERAL_ZERO",

    [TK_NULL]="TK_NULL"
};

