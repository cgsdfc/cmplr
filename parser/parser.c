#include "rules.h"

void init_simple(void)
{
  grammar gr;
  language lang={
    .name="Simple",
    .num_nonterm=10,
  };

  init_grammar(&gr,&lang);
  int Goal=def_nonterm(&gr,"Goal");
  int Sum=def_nonterm(&gr,"Sum");
  int Product=def_nonterm(&gr, "Product");
  int Value=def_nonterm(&gr,"Value");
  int Int=def_terminal(&gr,"Int");
  int Id=def_terminal(&gr,"Id");
  int eof=def_terminal(&gr,"eof");
  int plus=def_terminal(&gr,"+");
  int mult=def_terminal(&gr, "*");

  // r0 Goal := Sum eof
  def_rule(&gr, Goal, Sum, eof, -1);
  // r1 Sum := Product
  def_rule(&gr, Sum, Product, -1);
  // r2 Sum := Sum + Product
  def_rule(&gr, Sum, Sum, plus, Product , -1);
  // r3 Product := Value
  def_rule(&gr, Product, Value, -1);
  // r4 Product := Product * Value
  def_rule(&gr, Product, Product, mult, Value, -1);
  // r5 Value := Int
  def_rule(&gr, Value, Int,-1);
  // r5 Value := Id
  def_rule(&gr, Value, Id, -1);

  show_rules(&gr);
}

void init_clang(void)
{
  DEF_GRAMMAR();
  int program=DEF_NONTERM("program");
  int tran_unit=DEF_NONTERM( "translation-unit");
  int ext_dclr=DEF_NONTERM( "external-declaration");
  int func_def=DEF_NONTERM("function-definition");
  int dclr=DEF_NONTERM("declaration");
  int dclr_spfr=DEF_NONTERM("declaration-specifier");
  int dcltor=DEF_NONTERM("declarator");
  int dclist=DEF_NONTERM("declaration-list");
  int block=DEF_NONTERM( "composite-statement");
  int init_dcltor_list=DEF_NONTERM( "init-declarator-list");
  int stcl_spfr=DEF_NONTERM( "storage-class-specifier");
  int type_spfr=DEF_NONTERM("type-specifier");
  int type_qlfr=DEF_NONTERM("type-qualifier");

  int eof=DEF_TERMINAL("eof");

  /* def_rule(&gr, program, tran_unit, eof,-1); */
  DEF_RULE(program, tran_unit, eof);
  DEF_RULE(tran_unit, ext_dclr);
  DEF_RULE(tran_unit,tran_unit,ext_dclr);
  DEF_RULE(ext_dclr, func_def);
  DEF_RULE(ext_dclr, dclr);
  DEF_RULE(func_def,RULE_OPT,dclr_spfr,dcltor,RULE_OPT,dclist,block);


  DEF_RULE(dclr, dclr_spfr, RULE_OPT, init_dcltor_list); 
  DEF_RULE(dclist, dclr);
  DEF_RULE(dclist, dclist, dclr);

  DEF_RULE(dclr_spfr, stcl_spfr, RULE_OPT, dclr_spfr);
  DEF_RULE(dclr_spfr, type_spfr, RULE_OPT, dclr_spfr);
  DEF_RULE(dclr_spfr, type_qlfr, RULE_OPT, dclr_spfr);
  

  SHOW_RULES();

}


int main(int argc, char **argv)
{
  init_clang();


}

