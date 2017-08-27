#include "expr.h"

void init_expr(grammar *gr) {
  def_sepmore(gr, expr, comma, assign_expr);
  def_rule(gr, assign_expr, cond_expr);
  def_rule(gr, assign_expr, unary_expr, assign_oper, assign_expr);
  def_oneof(gr, assign_oper, equal, def_oper(gr, "*="), def_oper(gr, "/="),
            def_oper(gr, "%="), def_oper(gr, "+="), def_oper(gr, "-="),
            def_oper(gr, "<<="), def_oper(gr, ">>="), def_oper(gr, "&="),
            def_oper(gr, "^="), def_oper(gr, "|="));

  def_rule(gr, cond_expr, lg_or_expr);
  def_rule(gr, cond_expr, lg_or_expr, question, expr, colon, cond_expr);

  def_rule(gr, const_expr, cond_expr);

  def_sepmore(gr, lg_or_expr, def_oper(gr, "||"), lg_and_expr);

  def_sepmore(gr, lg_and_expr, def_oper(gr, "&&"), incl_or_expr);

  def_sepmore(gr, incl_or_expr, def_oper(gr, "|"), excl_or_expr);

  def_sepmore(gr, excl_or_expr, def_oper(gr, "^"), and_expr);

  def_sepmore(gr, and_expr, def_oper(gr, "&"), eq_expr);

  def_sepmore(gr, eq_expr, eq_oper, rel_expr);

  def_oneof(gr, eq_oper, def_oper(gr, "=="), def_oper(gr, "!="));

  def_sepmore(gr, rel_expr, rel_oper, shift_expr);

  def_oneof(gr, rel_oper, def_oper(gr, "<"), def_oper(gr, ">"),
            def_oper(gr, "<="), def_oper(gr, ">="));

  def_sepmore(gr, shift_expr, shift_oper, add_expr);

  def_oneof(gr, shift_oper, def_oper(gr, "<<"), def_oper(gr, ">>"));

  def_sepmore(gr, add_expr, add_oper, mul_expr);

  def_oneof(gr, add_oper, def_oper(gr, "+"), def_oper(gr, "-"));

  def_sepmore(gr, mul_expr, mul_oper, cast_expr);

  def_oneof(gr, mul_oper, def_oper(gr, "*"), def_oper(gr, "%"),
            def_oper(gr, "/"));

  def_rule(gr, cast_expr, unary_expr);
  def_rule(gr, cast_expr, left_pare, type_name, right_pare, cast_expr);

  def_rule(gr, unary_expr, postfix);
  def_rule(gr, unary_expr, plus_plus, unary_expr);
  def_rule(gr, unary_expr, minus_minus, unary_expr);
  def_rule(gr, unary_expr, unary_oper, cast_expr);
  def_rule(gr, unary_expr, _sizeof, unary_expr);
  def_rule(gr, unary_expr, _sizeof, left_pare, type_name, right_pare);

  def_oneof(gr, unary_oper, def_oper(gr, "&"), def_oper(gr, "*"),
            def_oper(gr, "+"), def_oper(gr, "-"), def_oper(gr, "~"),
            def_oper(gr, "!"));

  def_rule(gr, postfix, primary);
  def_rule(gr, postfix, postfix, left_bkt, expr, right_bkt);
  def_rule(gr, postfix, postfix, left_pare, RULE_OPT, arglist, right_pare);
  def_rule(gr, postfix, postfix, dot, id);
  def_rule(gr, postfix, postfix, def_oper(gr, "->"), id);
  def_rule(gr, postfix, postfix, plus_plus);
  def_rule(gr, postfix, postfix, minus_minus);

  def_oneof(gr, primary, id, char_const, float_const, string_const, int_const);
  def_rule(gr, primary, left_pare, expr, right_pare);

  def_sepmore(gr, arglist, comma, assign_expr);
}
