#ifndef TYPE_DESCRIPTION_H
#define TYPE_DESCRIPTION_H 1
typedef int type_des[TYPE_DESCRIPTOR_MAX_LEN];

typedef enum meta_type 
{
  TKMT_OPERATOR,
  TKMT_CONSTANT,
  TKMT_NAME,
  TKMT_PUNCTUATION,
} token_meta;

// use a global type_des tab 
// to record and alloc these info
typedef enum oper_order
{
  OPMT_ASS11,
  OPMT_TRI10
  OPMT_LGOR9,
  OPMT_LGAND8,
  OPMT_CMP7,
  OPMT_BWOR6,
  OPMT_BWXOR5,
  OPMT_BWAND4,
  OPMT_BWSHIFT3,
  OPMT_ADD2,
  OPMT_MUL1,
} oper_order ;

typedef enum oper_kind
{

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


} oper_kind;

#define META_TYPE(des) ( des[0] )
#define OPER_ORDER(des) ( des[1] )




#endif

