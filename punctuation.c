#include "punctuation.h"

// TODO make mapping from accepted state
// to token_type use less space
// but it is not ness
static const token_type _char2punc [] =
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
  ['.']=TKT_DOT,
  ['?']=TKT_QUESTION,

};

token_type char2punctuation(char ch)
{
  token_type tkt=_char2punc[ch];
  assert (tkt);
  return tkt;
}

void init_punctuation(void)
{
  add_accepted(TK_INIT, TK_PUNCTUATION_END, CHAR_CLASS_PUNCTUATION_NON_DOT);
  add_intermedia(TK_INIT,TK_DOT,CHAR_CLASS_DOT);
  add_accepted_rev(TK_DOT,TK_DOT_END,CHAR_CLASS_DEC_DIGITS);
}


