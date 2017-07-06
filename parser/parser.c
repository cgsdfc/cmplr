#include "rules.h"

void init_clang(void)
{
  DEF_GRAMMAR();
  int program=DEF_NONTERM("program");
  int tran_unit=DEF_NONTERM("translation-unit");
  int ext_dclr=DEF_NONTERM("external-declaration");
  int func_def=DEF_NONTERM("function-definition");
  int dclr=DEF_NONTERM("declaration");
  int dclr_spfr=DEF_NONTERM("declaration-specifier");
  int dcltor=DEF_NONTERM("declarator");
  int dclr_list=DEF_NONTERM("declaration-list");
  int block=DEF_NONTERM("composite-statement");
  int init_dcltor_list=DEF_NONTERM("init-declarator-list");
  int stcl_spfr=DEF_NONTERM("storage-class-specifier");
  int type_spfr=DEF_NONTERM("type-specifier");
  int type_qlfr=DEF_NONTERM("type-qualifier");
  int struct_union_spfr=DEF_NONTERM("struct-or-union-specifier");
  int enum_spfr=DEF_NONTERM("enum-specifier");
  int struct_or_union=DEF_NONTERM("struct-or-union");
  int struct_dclr_list=DEF_NONTERM("struct-declaration-list");
  int struct_dclr=DEF_NONTERM("struct-declaration");
  int init=DEF_NONTERM("initializer");
  int init_dclr=DEF_NONTERM("init-declarator");
  int spfr_qlfr_list=DEF_NONTERM("specifier-qualifier-list");
  int struct_dcltor_list=DEF_NONTERM("struct-declarator-list");
  int struct_dcltor=DEF_NONTERM("struct-declarator");
  int const_expr=DEF_NONTERM("constant-expression");
  int enumtor_list=DEF_NONTERM("enumerator-list");
  int enumtor=DEF_NONTERM("enumerator");
  int ptr=DEF_NONTERM("pointer");
  int dir_dcltor=DEF_NONTERM("direct-declarator");
  int param_type_list=DEF_NONTERM("parameter-type-list");
  int id_list=DEF_NONTERM("identifier-list");
  int type_qlfr_list=DEF_NONTERM("type-qualifier-list");
  int para_list=DEF_NONTERM("parameter-list");
  int para_dclr=DEF_NONTERM("parameter-declarator");
  int abs_dcltor=DEF_NONTERM("abstract-declarator");
  int dir_abs_dcltor=DEF_NONTERM("direct-abstract-declarator");
  int init_list=DEF_NONTERM("initializer-list");
  int type_name=DEF_NONTERM("type-name");
  int stmt=DEF_NONTERM("statement");
  int labeled_stmt=DEF_NONTERM("labeled-statement");
  int expr_stmt=DEF_NONTERM("expression-statement");
  int expr=DEF_NONTERM("expression");
  int selec_stmt=DEF_NONTERM("seletion-statement");
  int iter_stmt=DEF_NONTERM("iteration-statement");
  int jmp_stmt=DEF_NONTERM("jump-statement");
  int stmt_list=DEF_NONTERM("statement-list");
  int assign_oper=DEF_NONTERM("assignment-operator");
  int assign_expr=DEF_NONTERM("assignment-expression");
  int cond_expr=DEF_NONTERM("condition-expression");
  int lg_or_expr=DEF_NONTERM("logical-OR-expression");
  int lg_and_expr=DEF_NONTERM("logical-AND-expression");
  int incl_or_expr=DEF_NONTERM("inclusive-OR-expression");
  int excl_or_expr=DEF_NONTERM("exclusive-OR-expression");
  int unary_expr=DEF_NONTERM("unary-expression");
  int and_expr=DEF_NONTERM("AND-expression");
  int eq_expr=DEF_NONTERM("equality-expression");
  int rel_expr=DEF_NONTERM("relational-expression");
  int shift_expr=DEF_NONTERM("shift-expression");
  int add_expr=DEF_NONTERM("additive-expression");
  int mul_expr=DEF_NONTERM("multiplicative-expression");
  int cast_expr=DEF_NONTERM("cast-expression");
  int unary_oper=DEF_NONTERM("unary-operator");
  int postfix=DEF_NONTERM("postfix-expression");
  int primary=DEF_NONTERM("primary-expression");
  int arglist=DEF_NONTERM("argument-expression-list");
  int rel_oper=DEF_NONTERM("relational-operator");
  int mul_oper=DEF_NONTERM("multiplicative-operator");
  int add_oper=DEF_NONTERM("additive-operator");
  int eq_oper=DEF_NONTERM("equality-operator");
  int shift_oper=DEF_NONTERM("shift-operator");





  int id=DEF_TERMINAL("identifier");
  int float_const=DEF_TERMINAL("float-const");
  int int_const=DEF_TERMINAL("int-const");
  int string_const=DEF_TERMINAL("string-const");
  int char_const=DEF_TERMINAL("char-const");
  int eof=DEF_TERMINAL("eof");
  int left_brace=DEF_PUNC("{");
  int right_brace=DEF_PUNC("}");
  int left_bkt=DEF_PUNC("[");
  int right_bkt=DEF_PUNC("]");
  int left_pare=DEF_PUNC("(");
  int right_pare=DEF_PUNC(")");
  int equal=DEF_OPER("=");
  int _enum=DEF_KEYWORD("enum");
  int star=DEF_PUNC("*");
  int comma=DEF_PUNC(",");
  int dot=DEF_PUNC(".");
  int colon=DEF_PUNC(":");
  int semi=DEF_PUNC(";");
  int _sizeof=DEF_KEYWORD("sizeof");
  int question=DEF_PUNC("?");
  int plus_plus=DEF_OPER("++");
  int minus_minus=DEF_OPER("--");
  int _while=DEF_KEYWORD("while");
  /* def_rule(&gr, program, tran_unit, eof,-1); */
  DEF_RULE(program, tran_unit, eof);
  DEF_ONEMORE(tran_unit, ext_dclr);
  DEF_ONEOF(ext_dclr, func_def, dclr);
  DEF_RULE(func_def,RULE_OPT,dclr_spfr,dcltor,RULE_OPT,dclr_list,block);


  DEF_RULE(dclr, dclr_spfr, RULE_OPT, init_dcltor_list); 
  DEF_ONEMORE(dclr_list, dclr);

  DEF_RULE(dclr_spfr, stcl_spfr, RULE_OPT, dclr_spfr);
  DEF_RULE(dclr_spfr, type_spfr, RULE_OPT, dclr_spfr);
  DEF_RULE(dclr_spfr, type_qlfr, RULE_OPT, dclr_spfr);

  DEF_ONEOF(stcl_spfr, 
      DEF_KEYWORD("auto"),
      DEF_KEYWORD("register"),
      DEF_KEYWORD("static"),
      DEF_KEYWORD("extern"),
      DEF_KEYWORD("typedef")
      );

  DEF_ONEOF(type_spfr,
      DEF_KEYWORD("void"),
      DEF_KEYWORD("char"),
      DEF_KEYWORD("short"),
      DEF_KEYWORD("int"),
      DEF_KEYWORD("long"),
      DEF_KEYWORD("float"),
      DEF_KEYWORD("double"),
      DEF_KEYWORD("signed"),
      DEF_KEYWORD("unsigned"),
      struct_union_spfr,
      enum_spfr,
      id);

 DEF_ONEOF(type_qlfr,
    DEF_KEYWORD("const"),
    DEF_KEYWORD("volatile"));
 DEF_ONEOF(struct_or_union,
     DEF_KEYWORD("struct"),
     DEF_KEYWORD("union"));

 DEF_RULE(struct_union_spfr,
     struct_or_union,
     RULE_OPT, id,
     left_brace, struct_dclr_list, right_brace); 
 DEF_RULE(struct_union_spfr, struct_or_union, id);

 DEF_ONEMORE(struct_dclr_list,struct_dclr);
 DEF_ONEMORE(init_dcltor_list, init_dclr);

 DEF_RULE(init_dclr, dcltor);
 DEF_RULE(init_dclr,
     dcltor, 
     equal,
     init);

 DEF_RULE(struct_dclr,spfr_qlfr_list,struct_dcltor_list);
 DEF_RULE(spfr_qlfr_list,type_qlfr, RULE_OPT, spfr_qlfr_list);
 DEF_RULE(spfr_qlfr_list,type_spfr, RULE_OPT, spfr_qlfr_list);

 DEF_ONEMORE(struct_dcltor_list, struct_dcltor);

 DEF_RULE(struct_dcltor, dcltor);
 DEF_RULE(struct_dcltor,
     RULE_OPT, dcltor, 
     DEF_PUNC(":"),
     const_expr);

 DEF_RULE(enum_spfr,
     _enum,
     RULE_OPT, id,
     left_brace,
     enumtor_list,
     right_brace);

 DEF_RULE(enum_spfr,_enum, id);
 DEF_ONEMORE(enumtor_list,enumtor);
 DEF_RULE(enumtor, id);
 DEF_RULE(enumtor, id, equal, const_expr);
 
 DEF_RULE(dcltor, RULE_OPT, ptr, dir_dcltor);
 DEF_RULE(dir_dcltor,id);
 DEF_RULE(dir_dcltor,left_pare, dcltor, right_pare);
 DEF_RULE(dir_dcltor,dir_dcltor,left_bkt,RULE_OPT, const_expr, right_bkt);
 DEF_RULE(dir_dcltor,dir_dcltor,left_pare, param_type_list,right_pare);
 DEF_RULE(dir_dcltor, dir_dcltor, left_pare, RULE_OPT,id_list, right_pare);

 DEF_RULE(ptr, star, RULE_OPT,type_qlfr_list, RULE_OPT, ptr);
 DEF_ONEMORE(type_qlfr_list, type_qlfr);
 DEF_RULE(param_type_list, para_list);
 DEF_RULE(param_type_list, para_list, comma, dot,dot,dot);
 // TODO let tknzr recogize ...

 DEF_SEPMORE(para_list,comma, para_dclr);

 DEF_RULE(para_dclr, dclr_spfr, dcltor);
 DEF_RULE(para_dclr, dclr_spfr, RULE_OPT, abs_dcltor);
 DEF_SEPMORE(id_list, comma, id);

 DEF_RULE(init, assign_expr);
 DEF_RULE(init, left_brace, init_list, right_brace);
 DEF_RULE(init, left_brace, init_list,comma, right_brace);

 DEF_SEPMORE(init_list, comma, init);

 DEF_RULE(type_name, spfr_qlfr_list, RULE_OPT, abs_dcltor);
 DEF_RULE(abs_dcltor, ptr);
 DEF_RULE(abs_dcltor, RULE_OPT, ptr, dir_abs_dcltor);

 DEF_RULE(dir_abs_dcltor,left_pare,  abs_dcltor, right_pare);
 DEF_RULE(dir_abs_dcltor, 
     RULE_OPT,
     dir_abs_dcltor,
     left_bkt,
     RULE_OPT, const_expr,
     right_bkt);
 DEF_RULE(dir_abs_dcltor, 
     RULE_OPT,
     dir_abs_dcltor,
     left_pare,
     RULE_OPT, param_type_list,
     right_pare);
 
 DEF_ONEOF(stmt,
     labeled_stmt,
     expr_stmt,
     block,
     selec_stmt,
     iter_stmt,
     jmp_stmt);

 DEF_RULE(labeled_stmt, id, colon, stmt);
 DEF_RULE(labeled_stmt, 
     DEF_KEYWORD("case"),
     const_expr,
     colon,
     stmt);
DEF_RULE(labeled_stmt,
    DEF_KEYWORD("default"),
    colon,
    stmt);

DEF_RULE(expr_stmt, RULE_OPT, expr, semi);

DEF_RULE(block,
    left_brace,
   RULE_OPT, dclr_list, 
   RULE_OPT, stmt_list,
   right_brace);

DEF_ONEMORE(stmt_list, stmt);

DEF_RULE(selec_stmt,
    DEF_KEYWORD("if"),
    left_pare,
    expr,
    right_pare,
    stmt);

DEF_RULE(selec_stmt,
    DEF_KEYWORD("if"),
    left_pare,
    expr,
    right_pare,
    stmt,
    DEF_KEYWORD("else"),
    stmt);

DEF_RULE(selec_stmt,
    DEF_KEYWORD("switch"),
    left_pare,
    expr,
    right_pare,
    stmt);

DEF_RULE(iter_stmt,
    _while,
    left_pare,
    expr,
    right_pare,
    stmt);

DEF_RULE(iter_stmt,
    DEF_KEYWORD("do"),
    stmt,
    _while,
    left_pare,
    expr,
    right_pare,
    semi);

DEF_RULE(iter_stmt,
    DEF_KEYWORD("for"), left_pare,
    RULE_OPT, expr, semi,
    RULE_OPT, expr, semi,
    RULE_OPT, expr, right_pare,
    stmt);

DEF_RULE(jmp_stmt,
    DEF_KEYWORD("goto"),
    id,
    semi);

DEF_RULE(jmp_stmt,
    DEF_KEYWORD("return"),
    RULE_OPT, expr,
    semi);

DEF_RULE(jmp_stmt,
    DEF_KEYWORD("continue"),
    semi);

DEF_RULE(jmp_stmt,
    DEF_KEYWORD("break"),
    semi);

DEF_SEPMORE(expr, comma, assign_expr);
DEF_RULE(assign_expr,cond_expr);
DEF_RULE(assign_expr, unary_expr, assign_oper, assign_expr);
DEF_ONEOF(assign_oper,
    equal,
    DEF_OPER("*="),
    DEF_OPER("/="),
    DEF_OPER("%="),
    DEF_OPER("+="),
    DEF_OPER("-="),
    DEF_OPER("<<="),
    DEF_OPER(">>="),
    DEF_OPER("&="),
    DEF_OPER("^="),
    DEF_OPER("|="));

DEF_RULE(cond_expr,lg_or_expr);
DEF_RULE(cond_expr,
    lg_or_expr,
    question,
    expr,
    colon,
    cond_expr);

DEF_RULE(const_expr, cond_expr);

DEF_SEPMORE(lg_or_expr, DEF_OPER("||"), lg_and_expr);

DEF_SEPMORE(lg_and_expr, DEF_OPER("&&"), incl_or_expr);

DEF_SEPMORE(incl_or_expr, DEF_OPER("|"), excl_or_expr);

DEF_SEPMORE(excl_or_expr, DEF_OPER("^"), and_expr);

DEF_SEPMORE(and_expr, DEF_OPER("&"), eq_expr);

DEF_SEPMORE(eq_expr, eq_oper, rel_expr);

DEF_ONEOF(eq_oper,
    DEF_OPER("=="),
    DEF_OPER("!="));

DEF_SEPMORE(rel_expr, rel_oper, shift_expr);

DEF_ONEOF(rel_oper,
    DEF_OPER("<"),
    DEF_OPER(">"),
    DEF_OPER("<="),
    DEF_OPER(">="));

DEF_SEPMORE(shift_expr, shift_oper, add_expr);

DEF_ONEOF(shift_oper, 
    DEF_OPER("<<"),
    DEF_OPER(">>"));

DEF_SEPMORE(add_expr,add_oper, mul_expr);

DEF_ONEOF(add_oper,
    DEF_OPER("+"),
    DEF_OPER("-"));

DEF_SEPMORE(mul_expr, mul_oper, cast_expr);

DEF_ONEOF(mul_oper,
    DEF_OPER("*"),
    DEF_OPER("%"),
    DEF_OPER("/"));

DEF_RULE(cast_expr, unary_expr);
DEF_RULE(cast_expr,
    left_pare,
    type_name,
    right_pare,
    cast_expr);

DEF_RULE(unary_expr, postfix);
DEF_RULE(unary_expr, plus_plus, unary_expr);
DEF_RULE(unary_expr, minus_minus, unary_expr);
DEF_RULE(unary_expr, unary_oper, cast_expr);
DEF_RULE(unary_expr, _sizeof, unary_expr);
DEF_RULE(unary_expr, _sizeof, left_pare, type_name, right_pare);

DEF_ONEOF(unary_oper,
    DEF_OPER("&"),
    DEF_OPER("*"),
    DEF_OPER("+"),
    DEF_OPER("-"),
    DEF_OPER("~"),
    DEF_OPER("!"));

DEF_RULE(postfix, primary);
DEF_RULE(postfix, postfix, left_bkt, expr, right_bkt);
DEF_RULE(postfix, postfix, left_pare, RULE_OPT, arglist, right_pare);
DEF_RULE(postfix, postfix, dot, id);
DEF_RULE(postfix, postfix, DEF_OPER("->"), id);
DEF_RULE(postfix, postfix, plus_plus);
DEF_RULE(postfix, postfix, minus_minus);

DEF_ONEOF(primary,
    id,
    char_const,
    float_const,
    string_const,
    int_const);
DEF_RULE(primary, left_pare, expr, right_pare);

DEF_SEPMORE(arglist, comma, assign_expr);

  SHOW_RULES();

}


int main(int argc, char **argv)
{
  init_clang();


}

