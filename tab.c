#include "token_type.h"
#include "state.h"

const token_type char2type [] =
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

// TODO: map accepted operator state to operator type's
const token_type state2operator []=
{
  [TK_SLASH_END]=TKT_BINARY_OP_DIV,
  [TK_SLASH_EQUAL]=TKT_BINARY_OP_DIV_ASSIGN,

};

