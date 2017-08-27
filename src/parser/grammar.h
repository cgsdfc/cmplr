#ifndef GRAMMAR_H
#define GRAMMAR_H 1
#include "item.h"
#include "rules.h"

typedef enum C_symbol {
  PROGRAM,
  INIT_DCLTOR_LIST,
  STCL_SPFR,
  TYPE_SPFR,
  TYPE_QLFR,
  STRUCT_UNION_SPFR,
  ENUM_SPFR,
  STRUCT_OR_UNION,
  STRUCT_DCLR_LIST,
  STRUCT_DCLR,
  INIT,
  INIT_DCLR,
  SPFR_QLFR_LIST,
  STRUCT_DCLTOR_LIST,
  STRUCT_DCLTOR,
  STMT,
  ENUMTOR_LIST,
  ENUMTOR,
  CONST_EXPR,
  LABELED_STMT,
  EXPR_STMT,
  SELEC_STMT,
  ITER_STMT,
  JMP_STMT,
  STMT_LIST,
  BLOCK,

  TRAN_UNIT,
  EXT_DCLR,
  FUNC_DEF,
  DCLR,
  DCLR_SPFR,
  DCLTOR,
  DCLR_LIST,

  ID,
  FLOAT_CONST,
  _CONST,
  STRING_CONST,
  CHAR_CONST,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BKT,
  RIGHT_BKT,
  LEFT_PARE,
  RIGHT_PARE,
  EQUAL,
  _ENUM,
  STAR,
  COMMA,
  DOT,
  COLON,
  SEMI,
  _SIZEOF,
  QUESTION,
  PLUS_PLUS,
  MINUS_MINUS,
  _WHILE,

  ASSIGN_OPER,
  ASSIGN_EXPR,
  COND_EXPR,
  LG_OR_EXPR,
  LG_AND_EXPR,
  INCL_OR_EXPR,
  EXCL_OR_EXPR,
  UNARY_EXPR,
  AND_EXPR,
  EQ_EXPR,
  REL_EXPR,
  SHIFT_EXPR,
  ADD_EXPR,
  MUL_EXPR,
  CAST_EXPR,
  UNARY_OPER,
  POSTFIX,
  PRIMARY,
  ARGLIST,
  REL_OPER,
  MUL_OPER,
  ADD_OPER,
  EQ_OPER,
  SHIFT_OPER,

  TYPE_NAME,
} C_symbol;

void init_enum(grammar *);

#endif
