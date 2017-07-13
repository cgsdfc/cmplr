#include "grammar.h"

void init_enum(grammar *gr)
{
  // enum_spfr
  // enumtor_list
  // enumtor
  // 
  // id, left_brace, right_brace,
  // equal, const_expr,
  // 
  def_rule(gr, PROGRAM, ENUM_SPFR);
  def_rule(gr, ENUM_SPFR,
      _ENUM,
      RULE_OPT, ID,
      LEFT_BRACE,
      ENUMTOR_LIST,
      RIGHT_BRACE);
  def_rule(gr, ENUM_SPFR,_ENUM, ID);
  def_sepmore(gr, ENUMTOR_LIST,COMMA, ENUMTOR);
  def_rule(gr, ENUMTOR, ID);
  def_rule(gr, ENUMTOR, ID, EQUAL, CONST_EXPR);

  build_item_set(gr);
}


