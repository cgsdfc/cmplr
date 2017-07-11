#include "enum.h"

void init_enum(grammar *gr)
{
  def_rule(gr, enum_spfr,
      _enum,
      RULE_OPT, id,
      left_brace,
      enumtor_list,
      right_brace);
  def_rule(gr, enum_spfr,_enum, id);
  def_onemore(gr, enumtor_list,enumtor);
  def_rule(gr, enumtor, id);
  def_rule(gr, enumtor, id, equal, const_expr);
}


