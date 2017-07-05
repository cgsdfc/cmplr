#include "rules.h"

void init_simple(void)
{
  grammar gr;
  init_grammar(&gr,"Simple", 10);
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
  grammar gr;
  init_grammar(&gr,"C programing language", 100);
  int program=def_nonterm(&gr,"program");
  int tran_unit=def_nonterm(&gr, "translation-unit");
  int ext_dclr=def_nonterm(&gr, "external-declaration");
  int func_def=def_nonterm(&gr,"function-definition");
  int dclr=def_nonterm(&gr,"declaration");
  int dclr_spfr=def_nonterm(&gr,"declaration-specifier");
  int dcltor=def_nonterm(&gr,"declarator");
  int dclist=def_nonterm(&gr,"declaration-list");
  int block=def_nonterm(&gr, "composite-statement");

  int eof=def_terminal(&gr,"eof");

  def_rule(&gr, program, tran_unit, eof,-1);
  def_rule(&gr, tran_unit, ext_dclr, -1);
  def_rule(&gr, tran_unit, tran_unit, ext_dclr,-1);
  def_rule(&gr, ext_dclr, func_def, -1);
  def_rule(&gr, ext_dclr, dclr,-1);

  def_rule(&gr, func_def, -2, dclr_spfr, dcltor, -2, dclist, block,-1);

  show_rules(&gr);

}


int main(int argc, char **argv)
{
  init_clang();


}

