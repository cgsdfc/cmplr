#include "grammar.h"

int init_dcltor_list;
int stcl_spfr;
int type_spfr;
int type_qlfr;
int struct_union_spfr;
int enum_spfr;
int struct_or_union;
int struct_dclr_list;
int struct_dclr;
int init;
int init_dclr;
int spfr_qlfr_list;
int struct_dcltor_list;
int struct_dcltor;


int program;
int tran_unit;
int ext_dclr;
int func_def;
int dclr;
int dclr_spfr;
int dcltor;
int dclr_list;
 

int id;
int float_const;
int int_const;
int string_const;
int char_const;
int eof;
int left_brace;
int right_brace;
int left_bkt;
int right_bkt;
int left_pare;
int right_pare;
int equal;
int _enum;
int star;
int comma;
int dot;
int colon;
int semi;
int _sizeof;
int question;
int plus_plus;
int minus_minus;
int _while;
int stmt;
int labeled_stmt;
int expr_stmt;
int selec_stmt;
int iter_stmt;
int jmp_stmt;
int stmt_list;
int _while;
int block;

int _sizeof;
int comma;
int dot;
int colon;
int float_const;
int string_const;
int char_const;

int assign_oper;
int assign_expr;
int cond_expr;
int lg_or_expr;
int lg_and_expr;
int incl_or_expr;
int excl_or_expr;
int unary_expr;
int and_expr;
int eq_expr;
int rel_expr;
int shift_expr;
int add_expr;
int mul_expr;
int cast_expr;
int unary_oper;
int postfix;
int primary;
int arglist;
int rel_oper;
int mul_oper;
int add_oper;
int eq_oper;
int shift_oper;
int plus_plus;
int minus_minus;
int question;
int const_expr;
int expr;
int enumtor_list;
int enumtor;
int _enum;
int enum_spfr;
int type_name;


void init_all(grammar *gr)
{
  stmt=def_nonterm(gr, "statement");
  labeled_stmt=def_nonterm(gr, "labeled-statement");
  expr_stmt=def_nonterm(gr, "expression-statement");
  selec_stmt=def_nonterm(gr, "seletion-statement");
  iter_stmt=def_nonterm(gr, "iteration-statement");
  jmp_stmt=def_nonterm(gr, "jump-statement");
  stmt_list=def_nonterm(gr, "statement-list");
  _while=def_keyword(gr, "while");
  block=def_nonterm(gr, "composite-statement");

  _sizeof=def_keyword(gr, "sizeof");
  comma=def_punc(gr, ",");
  dot=def_punc(gr, ".");
  colon=def_punc(gr, ":");
  float_const=def_terminal(gr, "float-const");
  int_const=def_terminal(gr, "int-const");
  string_const=def_terminal(gr, "string-const");
  char_const=def_terminal(gr, "char-const");

  assign_oper=def_nonterm(gr, "assignment-operator");
  assign_expr=def_nonterm(gr, "assignment-expression");
  cond_expr=def_nonterm(gr, "condition-expression");
  lg_or_expr=def_nonterm(gr, "logical-or-expression");
  lg_and_expr=def_nonterm(gr, "logical-and-expression");
  incl_or_expr=def_nonterm(gr, "inclusive-or-expression");
  excl_or_expr=def_nonterm(gr, "exclusive-or-expression");
  unary_expr=def_nonterm(gr, "unary-expression");
  and_expr=def_nonterm(gr, "and-expression");
  eq_expr=def_nonterm(gr, "equality-expression");
  rel_expr=def_nonterm(gr, "relational-expression");
  shift_expr=def_nonterm(gr, "shift-expression");
  add_expr=def_nonterm(gr, "additive-expression");
  mul_expr=def_nonterm(gr, "multiplicative-expression");
  cast_expr=def_nonterm(gr, "cast-expression");
  unary_oper=def_nonterm(gr, "unary-operator");
  postfix=def_nonterm(gr, "postfix-expression");
  primary=def_nonterm(gr, "primary-expression");
  arglist=def_nonterm(gr, "argument-expression-list");
  rel_oper=def_nonterm(gr, "relational-operator");
  mul_oper=def_nonterm(gr, "multiplicative-operator");
  add_oper=def_nonterm(gr, "additive-operator");
  eq_oper=def_nonterm(gr, "equality-operator");
  shift_oper=def_nonterm(gr, "shift-operator");
  plus_plus=def_oper(gr, "++");
  type_name=def_nonterm(gr, "type-name");
  minus_minus=def_oper(gr, "--");
  question=def_punc(gr,"?");
  const_expr=def_nonterm(gr, "constant-expression");
  expr=def_nonterm(gr, "expression");

  id=def_terminal(gr, "identifier");
  eof=def_terminal(gr, "eof");
  left_brace=def_punc(gr, "{");
  right_brace=def_punc(gr, "}");
  left_bkt=def_punc(gr, "[");
  right_bkt=def_punc(gr, "]");
  left_pare=def_punc(gr, "(");
  right_pare=def_punc(gr, ")");
  equal=def_oper(gr, "=");
  _enum=def_keyword(gr, "enum");
  star=def_punc(gr, "*");
  comma=def_punc(gr, ",");
  dot=def_punc(gr, ".");
  colon=def_punc(gr, ":");
  semi=def_punc(gr, ";");
  _sizeof=def_keyword(gr, "sizeof");
  question=def_punc(gr, "?");
  plus_plus=def_oper(gr, "++");
  minus_minus=def_oper(gr, "--");
  _while=def_keyword(gr, "while");
  enumtor_list=def_nonterm(gr, "enumerator-list");
  enumtor=def_nonterm(gr, "enumerator");
  _enum=def_keyword(gr, "enum");
  enum_spfr=def_nonterm(gr, "enum-specifier");


}
