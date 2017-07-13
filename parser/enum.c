#include "enum.h"

void init_enum(grammar *gr)
{
  language lang={
    .name="enum",
    .num_symbol=11,
    .num_nonterm=4,
  };

  init_grammar(gr, &lang);
  int start=def_nonterm(gr, "start");
  int enum_spfr=def_nonterm(gr, "enum_spfr");
  int enumtor_list=def_nonterm(gr, "enumtor_list");
  int enumtor=def_nonterm(gr, "enumtor");

  int id=def_terminal(gr,"id");
  int left_brace=def_terminal(gr,"{");
  int right_brace=def_terminal(gr,"}");
  int equal=def_terminal(gr,"=");
  int const_expr=def_terminal(gr,"const_expr");
  int _enum=def_terminal(gr, "enum");
  int comma=def_terminal(gr, ",");

  // enum_spfr
  // enumtor_list
  // enumtor
  // 
  // id, left_brace, right_brace,
  // equal, const_expr,
  // 
  def_rule(gr, start, enum_spfr);
  def_rule(gr, enum_spfr,
      _enum,
      RULE_OPT, id,
      left_brace,
      enumtor_list,
      right_brace);
  def_rule(gr, enum_spfr,_enum, id);
  def_sepmore(gr, enumtor_list,comma, enumtor);
  def_rule(gr, enumtor, id);
  def_rule(gr, enumtor, id, equal, const_expr);

  build_item_set(gr);
}


