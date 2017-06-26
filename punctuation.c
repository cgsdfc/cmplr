#include "punctuation.h"

const token_type state2punctuation [] =
{
  
  /* PUNCTUATIONS */
  ['(']=TKT_LEFT_PARENTHESIS,
  [')']=TKT_RIGHT_PARENTHESIS,
  ['[']=TKT_LEFT_BRACKET,
  [']']=TKT_RIGHT_BRACKET,
  ['{']=TKT_LEFT_BRACE,
  ['}']=TKT_RIGHT_BRACE,
  [':']=TKT_COLON,
  [';']=TKT_SEMICOLON,
  [',']=TKT_COMMA,
  ['.']=TKT_PERIOD,
  ['?']=TKT_QUESTION,

};

