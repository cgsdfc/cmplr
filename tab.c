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
  /* slash */
  [TK_SLASH_END]=TKT_BINARY_OP_DIV,
  [TK_SLASH_EQUAL]=TKT_BINARY_OP_DIV_ASSIGN,

  /* exlaim */
  [TK_EXCLAIM_END]=TKT_UNARY_OP_LOGICAL_NOT,
  [TK_EXCLAIM_EQUAL]=TKT_BINARY_OP_CMP_NOT_EQUAL,

  /* equal */
  [TK_EQUAL_END]=TKT_BINARY_OP_ASSIGN,
  [TK_EQUAL_EQUAL]=TKT_BINARY_OP_CMP_EQUAL,

  /* ampersand */
  [TK_AMPERSAND_END]=TKT_AMPERSAND,
  [TK_AMPERSAND_EQUAL]=TKT_BINARY_OP_BIT_AND_ASSIGN,
  [TK_AMPERSAND_AMPERSAND]=TKT_BINARY_OP_LOGICAL_AND,

  /* vertical bar */
  [TK_VERTICAL_BAR_END]=TKT_BINARY_OP_BIT_OR,
  [TK_VERTICAL_BAR_EQUAL]=TKT_BINARY_OP_BIT_OR_ASSIGN,
  [TK_VERTICAL_BAR_BAR]=TKT_BINARY_OP_LOGICAL_OR,

  /* percent */
  [TK_PERCENT_END]=TKT_BINARY_OP_MOD,
  [TK_PERCENT_EQUAL]=TKT_BINARY_OP_MOD_ASSIGN,

  /* caret */
  [TK_CARET_END]=TKT_BINARY_OP_BIT_XOR,
  [TK_CARET_EQUAL]=TKT_BINARY_OP_BIT_XOR_ASSIGN,

  /* star */
  [TK_STAR_END]=TKT_STAR,
  [TK_STAR_EQUAL]=TKT_BINARY_OP_MUL_ASSIGN,

  /* less */ 
  [TK_LESS_END]=TKT_BINARY_OP_CMP_LESS,
  [TK_LESS_EQUAL]=TKT_BINARY_OP_CMP_LESS_EQUAL,
  [TK_LESS_LESS]=TKT_BINARY_OP_BIT_LEFT_SHIFT,
  [TK_LESS_LESS_EQUAL]=TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN,

  /* greater */ 
  [TK_GREATER_END]=TKT_BINARY_OP_CMP_GREATER,
  [TK_GREATER_EQUAL]=TKT_BINARY_OP_CMP_GREATER_EQUAL,
  [TK_GREATER_GREATER]=TKT_BINARY_OP_BIT_RIGHT_SHIFT,
  [TK_GREATER_GREATER_EQUAL]=TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN,

  /* positive */
  [TK_POSITIVE_END]=TKT_PLUS,
  [TK_POSITIVE_POSITIVE]=TKT_UNARY_OP_PLUS_PLUS,
  [TK_POSITIVE_EQUAL]=TKT_BINARY_OP_ADD_ASSIGN,


  /* negative */
  [TK_NEGATIVE_END]=TKT_MINUS,
  [TK_NEGATIVE_NEGATIVE]=TKT_UNARY_OP_MINUS_MINUS,
  [TK_NEGATIVE_EQUAL]=TKT_BINARY_OP_SUB_ASSIGN,



};

