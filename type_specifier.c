#include "type_specifier.h"

static state_table *tsp_table;
static token_buffer tsp_token_buffer;

static bool
tsp_is_in_class(entry_t cond, char ch)
{
  return _TKT_TSP_BEGIN < ch && ch < _TKT_TSP_END;
}

void init_tsp_table(token_buffer *buf)
{
  assert(buf);
  tsp_token_buffer=buf;
  tsp_table = alloc_table();
  assert(tsp_table);
  int r;

  r= init_table(tsp_table,
      "type specifier's table",
      N_TSP_TABLE_ROWS,
      N_TSP_TABLE_COLS,
      TSP_NULL,
      tsp_is_in_class);

  /* signed and unsigned */
  add_intermedia(TSP_INIT,TSP_SIGNED, TKT_KW_SIGNED);
  add_intermedia(TSP_INIT,TSP_UNSIGNED, TKT_KW_UNSIGNED);

  /* no specifier mean for int */
  add_accepted_rev(TSP_INIT,TSP_INT_END,TKT_TSP_EMPTY);

  /* char,short,int,long */
  add_accepted(TSP_INIT,TSP_CHAR_END,TKT_KW_CHAR);
  add_accepted(TSP_INIT,TSP_SHORT,TKT_KW_SHORT);
  add_intermedia(TSP_INIT,TSP_INT,TKT_KW_INT);
  add_intermedia(TSP_INIT,TSP_LONG,TKT_KW_LONG);

  /* unsigned or signed char , short, int , long */
  add_accepted(TSP_SIGNED,TSP_CHAR_END,TKT_KW_CHAR);
  add_accepted(TSP_SIGNED,TSP_SHORT,TKT_KW_SHORT);
  add_intermedia(TSP_UNSIGNED,TSP_INT,TKT_KW_INT);
  add_intermedia(TSP_UNSIGNED,TSP_LONG,TKT_KW_LONG);



  add_accepted(TSP_LONG,TSP_LONG_LONG_end,TKT_KW_LONG);
  add_accepted(TSP_INIT,TSP_CHAR_end,TKT_KW_CHAR);

  add_intermedia(TSP_UNSIGNED,TSP_CHAR,TKT_KW_CHAR);
  add_intermedia(TSP_UNSIGNED,TSP_INT,TKT_KW_INT);
  add_intermedia(TSP_UNSIGNED,TSP_SHORT,TKT_KW_SHORT);
  add_intermedia(TSP_UNSIGNED,TSP_LONG,TKT_KW_LONG);

  add_intermedia(TSP_SIGNED,TSP_CHAR,TKT_KW_CHAR);
  add_intermedia(TSP_SIGNED,TSP_INT,TKT_KW_INT);
  add_intermedia(TSP_SIGNED,TSP_SHORT,TKT_KW_SHORT);
  add_intermedia(TSP_SIGNED,TSP_LONG,TKT_KW_LONG);
  add_intermedia(TSP_SIGNED,TSP_LONG_LONG,TKT_KW_LONG);

  add_accepted_rev(TSP_SIGNED,TSP_INT,TKT_TSP_EMPTY);
  add_accepted_rev(TSP_UNSIGNED,TSP_INT,TKT_TSP_EMPTY);

} 





#if 0
#ifndef TOKEN_TYPE_H 
#define TOKEN_TYPE_H 1

#define N_KEYWORDS ( 32 )
#define FIRST_KEYWORD TKT_KW_FOR 
#define LAST_KEYWORD TKT_KW_INT


extern const char *token_tab[];
extern const char *keywords_tab[];


typedef enum token_type 
{
  /* keywords must come first */
   /* KEYWORDS */

  /* used in control */
  TKT_KW_FOR=1,
  TKT_KW_WHILE,
  TKT_KW_CASE,
  TKT_KW_RETURN,
  TKT_KW_SWITCH,
  TKT_KW_IF,
  TKT_KW_DEFAULT ,
  TKT_KW_CONTINUE ,
  TKT_KW_ELSE,
  TKT_KW_DO ,
  TKT_KW_GOTO,
  TKT_KW_BREAK,

  /* qualifiers */
  TKT_KW_CONST ,
  TKT_KW_VOLATILE ,
  
  /* compose type */
  TKT_KW_STRUCT,
  TKT_KW_ENUM,
  TKT_KW_UNION ,

  /* specifier */
  TKT_KW_DOUBLE,
  TKT_KW_SIGNED ,
  TKT_KW_INT,
  TKT_KW_LONG,
  TKT_KW_CHAR,
  TKT_KW_FLOAT ,
  TKT_KW_SHORT ,
  TKT_KW_UNSIGNED ,
  TKT_KW_VOID ,

  /* storage */ 
  TKT_KW_TYPEDEF,
  TKT_KW_EXTERN,
  TKT_KW_STATIC ,
  TKT_KW_AUTO,
  TKT_KW_REGISTER,

  /* sizeof operator */
  TKT_KW_SIZEOF ,


  /* PUNCTUATIONS */
  TKT_LEFT_PARENTHESIS,
  TKT_RIGHT_PARENTHESIS,
  TKT_LEFT_BRACKET,
  TKT_RIGHT_BRACKET,
  TKT_LEFT_BRACE,
  TKT_RIGHT_BRACE,
  TKT_COLON,
  TKT_SEMICOLON,
  TKT_COMMA,
  TKT_PERIOD,
  TKT_QUESTION,

  /* binary arithmetic operator */
  TKT_BINARY_OP_ADD,
  TKT_BINARY_OP_SUB,
  TKT_BINARY_OP_DIV,
  TKT_BINARY_OP_MUL,
  TKT_BINARY_OP_MOD  ,

  /* binary arithmetic assignment operator */
  TKT_BINARY_OP_MOD_ASSIGN  ,
  TKT_BINARY_OP_ADD_ASSIGN,
  TKT_BINARY_OP_SUB_ASSIGN,
  TKT_BINARY_OP_DIV_ASSIGN,
  TKT_BINARY_OP_MUL_ASSIGN,
  TKT_BINARY_OP_ASSIGN,

  /* binary bitwise assignment operator */
  TKT_BINARY_OP_BIT_AND_ASSIGN,
  TKT_BINARY_OP_BIT_OR_ASSIGN,
  TKT_BINARY_OP_BIT_XOR_ASSIGN,
  TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN,
  TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN,

  /* binary bitwise operator */
  TKT_BINARY_OP_BIT_AND,
  TKT_BINARY_OP_BIT_OR,
  TKT_BINARY_OP_BIT_XOR,
  TKT_BINARY_OP_BIT_LEFT_SHIFT,
  TKT_BINARY_OP_BIT_RIGHT_SHIFT,

  /* binary logical operator */
  TKT_BINARY_OP_LOGICAL_AND,
  TKT_BINARY_OP_LOGICAL_OR ,

  /* binary comparation operator */
  TKT_BINARY_OP_CMP_LESS  ,
  TKT_BINARY_OP_CMP_LESS_EQUAL  ,
  TKT_BINARY_OP_CMP_GREATER  ,
  TKT_BINARY_OP_CMP_GREATER_EQUAL ,

  /* binary comparation operator */
  TKT_BINARY_OP_CMP_EQUAL ,
  TKT_BINARY_OP_CMP_NOT_EQUAL  ,

  /* binary member operator */
  TKT_BINARY_OP_MEMBER_DOT  ,
  TKT_BINARY_OP_MEMBER_ARROW,

  /* UNARY OPERATORS */
  TKT_UNARY_OP_LOGICAL_NOT  ,
  TKT_UNARY_OP_BIT_NOT  ,
  TKT_UNARY_OP_PLUS_PLUS  ,
  TKT_UNARY_OP_MINUS_MINUS  ,
  TKT_UNARY_OP_NEGATIVE  ,
  TKT_UNARY_OP_ADDRESS  ,
  TKT_UNARY_OP_DEREFERENCE  ,
    /* IDENTIFIERS */
  TKT_IDENTIFIER,

  /* ITERALS */
  TKT_INTEGER_LITERAL,
  TKT_CHARACTER_LITERAL,
  TKT_STRING_LITERAL,
  TKT_AMPERSAND,
  TKT_STAR,
  TKT_PLUS,
  TKT_MINUS,
  TKT_FLOAT_LITERAL,
  TKT_UNKNOWN


} token_type;

extern const token_type state2punctuation [];
extern const token_type state2operator[];
#endif


#ifndef TYPE_SPECIFIER_H
#define TYPE_SPECIFIER_H 1

typedef enum tsp_state
{
  TSP_INIT,
  TSP_SIGNED,
  TSP_UNSIGNED,
  TSP_CHAR,
  TSP_INT,
  TSP_SHORT,
  TSP_LONG,
  TSP_VOID,
} tsp_state;



#endif

#endif

