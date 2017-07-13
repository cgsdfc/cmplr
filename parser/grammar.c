#include "grammar.h"

static const grammar_info C_info=
{
  [N_SYMBOLS_IDX]=250,
  [N_NONTERM_IDX]=100,
  [N_NONTERM_IDX]=40,
};

int C_init_symbol(grammar *gr)
{

  init_grammar(gr, C_info);
  def_symbol(gr, PROGRAM);
  def_symbol(gr, INIT_DCLTOR_LIST);
  def_symbol(gr, STCL_SPFR);
  def_symbol(gr, TYPE_SPFR);
  def_symbol(gr, TYPE_QLFR);
  def_symbol(gr, STRUCT_UNION_SPFR);
  def_symbol(gr, ENUM_SPFR);
  def_symbol(gr, STRUCT_OR_UNION);
  def_symbol(gr, STRUCT_DCLR_LIST);
  def_symbol(gr, STRUCT_DCLR);
  def_symbol(gr, INIT);
  def_symbol(gr, INIT_DCLR);
  def_symbol(gr, SPFR_QLFR_LIST);
  def_symbol(gr, STRUCT_DCLTOR_LIST);
  def_symbol(gr, STRUCT_DCLTOR);


  def_symbol(gr, TRAN_UNIT);
  def_symbol(gr, EXT_DCLR);
  def_symbol(gr, FUNC_DEF);
  def_symbol(gr, DCLR);
  def_symbol(gr, DCLR_SPFR);
  def_symbol(gr, DCLTOR);
  def_symbol(gr, DCLR_LIST);


  def_symbol(gr, ID);
  def_symbol(gr, FLOAT_CONST);
  def_symbol(gr, _CONST);
  def_symbol(gr, STRING_CONST);
  def_symbol(gr, CHAR_CONST);
  def_symbol(gr, LEFT_BRACE);
  def_symbol(gr, RIGHT_BRACE);
  def_symbol(gr, LEFT_BKT);
  def_symbol(gr, RIGHT_BKT);
  def_symbol(gr, LEFT_PARE);
  def_symbol(gr, RIGHT_PARE);
  def_symbol(gr, EQUAL);
  def_symbol(gr, _ENUM);
  def_symbol(gr, STAR);
  def_symbol(gr, COMMA);
  def_symbol(gr, DOT);
  def_symbol(gr, COLON);
  def_symbol(gr, SEMI);
  def_symbol(gr, _SIZEOF);
  def_symbol(gr, QUESTION);
  def_symbol(gr, PLUS_PLUS);
  def_symbol(gr, MINUS_MINUS);
  def_symbol(gr, _WHILE);
  def_symbol(gr, STMT);
  def_symbol(gr, LABELED_STMT);
  def_symbol(gr, EXPR_STMT);
  def_symbol(gr, SELEC_STMT);
  def_symbol(gr, ITER_STMT);
  def_symbol(gr, JMP_STMT);
  def_symbol(gr, STMT_LIST);
  def_symbol(gr, _WHILE);
  def_symbol(gr, BLOCK);

  def_symbol(gr, _SIZEOF);
  def_symbol(gr, COMMA);
  def_symbol(gr, DOT);
  def_symbol(gr, COLON);
  def_symbol(gr, FLOAT_CONST);
  def_symbol(gr, STRING_CONST);
  def_symbol(gr, CHAR_CONST);

  def_symbol(gr, ASSIGN_OPER);
  def_symbol(gr, ASSIGN_EXPR);
  def_symbol(gr, COND_EXPR);
  def_symbol(gr, LG_OR_EXPR);
  def_symbol(gr, LG_AND_EXPR);
  def_symbol(gr, INCL_OR_EXPR);
  def_symbol(gr, EXCL_OR_EXPR);
  def_symbol(gr, UNARY_EXPR);
  def_symbol(gr, AND_EXPR);
  def_symbol(gr, EQ_EXPR);
  def_symbol(gr, REL_EXPR);
  def_symbol(gr, SHIFT_EXPR);
  def_symbol(gr, ADD_EXPR);
  def_symbol(gr, MUL_EXPR);
  def_symbol(gr, CAST_EXPR);
  def_symbol(gr, UNARY_OPER);
  def_symbol(gr, POSTFIX);
  def_symbol(gr, PRIMARY);
  def_symbol(gr, ARGLIST);
  def_symbol(gr, REL_OPER);
  def_symbol(gr, MUL_OPER);
  def_symbol(gr, ADD_OPER);
  def_symbol(gr, EQ_OPER);
  def_symbol(gr, SHIFT_OPER);
  def_symbol(gr, PLUS_PLUS);
  def_symbol(gr, MINUS_MINUS);
  def_symbol(gr, QUESTION);
  def_symbol(gr, CONST_EXPR);
  def_symbol(gr, EXPR);
  def_symbol(gr, ENUMTOR_LIST);
  def_symbol(gr, ENUMTOR);
  def_symbol(gr, _ENUM);
  def_symbol(gr, ENUM_SPFR);
  def_symbol(gr, TYPE_NAME);
}
