#include "rule.h"
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

