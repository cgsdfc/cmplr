/* statement.c */
#include "stmt.h"

void init_stmt(grammar *gr)
{

  def_oneof(gr, stmt,
      labeled_stmt,
      expr_stmt,
      block,
      selec_stmt,
      iter_stmt,
      jmp_stmt);

  def_rule(gr, labeled_stmt, id, colon, stmt);
  def_rule(gr, labeled_stmt, 
      def_keyword(gr, "case"),
      const_expr,
      colon,
      stmt);
  def_rule(gr, labeled_stmt,
      def_keyword(gr, "default"),
      colon,
      stmt);

  def_rule(gr, expr_stmt, RULE_OPT, expr, semi);

  def_rule(gr, block,
      left_brace,
      RULE_OPT, dclr_list, 
      RULE_OPT, stmt_list,
      right_brace);

  def_onemore(gr, stmt_list, stmt);

  def_rule(gr, selec_stmt,
      def_keyword(gr, "if"),
      left_pare,
      expr,
      right_pare,
      stmt);

  def_rule(gr, selec_stmt,
      def_keyword(gr, "if"),
      left_pare,
      expr,
      right_pare,
      stmt,
      def_keyword(gr, "else"),
      stmt);

  def_rule(gr, selec_stmt,
      def_keyword(gr, "switch"),
      left_pare,
      expr,
      right_pare,
      stmt);

  def_rule(gr, iter_stmt,
      _while,
      left_pare,
      expr,
      right_pare,
      stmt);

  def_rule(gr, iter_stmt,
      def_keyword(gr, "do"),
      stmt,
      _while,
      left_pare,
      expr,
      right_pare,
      semi);

  def_rule(gr, iter_stmt,
      def_keyword(gr, "for"), left_pare,
      RULE_OPT, expr, semi,
      RULE_OPT, expr, semi,
      RULE_OPT, expr, right_pare,
      stmt);

  def_rule(gr, jmp_stmt,
      def_keyword(gr, "goto"),
      id,
      semi);

  def_rule(gr, jmp_stmt,
      def_keyword(gr, "return"),
      RULE_OPT, expr,
      semi);

  def_rule(gr, jmp_stmt,
      def_keyword(gr, "continue"),
      semi);

  def_rule(gr, jmp_stmt,
      def_keyword(gr, "break"),
      semi);


}



