#include "struct.h"

void init_struct(grammar *gr) {
  def_rule(gr, struct_union_spfr, struct_or_union, RULE_OPT, id, left_brace,
           struct_dclr_list, right_brace);
  def_rule(gr, struct_union_spfr, struct_or_union, id);

  def_onemore(gr, struct_dclr_list, struct_dclr);
  def_rule(gr, struct_dclr, spfr_qlfr_list, struct_dcltor_list);
  def_rule(gr, spfr_qlfr_list, type_qlfr, RULE_OPT, spfr_qlfr_list);
  def_rule(gr, spfr_qlfr_list, type_spfr, RULE_OPT, spfr_qlfr_list);

  def_onemore(gr, struct_dcltor_list, struct_dcltor);

  def_rule(gr, struct_dcltor, dcltor);
  def_rule(gr, struct_dcltor, RULE_OPT, dcltor, def_punc(gr, ":"), const_expr);
}
