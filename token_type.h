#ifndef TOKEN_TYPE_H 
#define TOKEN_TYPE_H 1

#define N_KEYWORDS ( 32 )
#define FIRST_KEYWORD TKT_KW_FOR 
#define LAST_KEYWORD TKT_KW_SIZEOF


extern const char *token_tab[];
extern const char *keywords_tab[];


typedef enum token_type 
{

  TKT_KW_FOR=0,
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
  

  /* specifier */
  _TKT_TSP_BEGIN,
  TKT_KW_DOUBLE,
  TKT_KW_SIGNED ,
  TKT_KW_INT,
  TKT_KW_LONG,
  TKT_KW_CHAR,
  TKT_KW_FLOAT ,
  TKT_KW_SHORT ,
  TKT_KW_UNSIGNED ,
  TKT_KW_VOID ,
  /* compose type */
  TKT_KW_STRUCT,
  TKT_KW_ENUM,
  TKT_KW_UNION ,
  _TKT_TSP_END,

  /* storage */ 
  // can has at most one
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
  TKT_DOT,
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

