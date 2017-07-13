#include "grammar.h"
#include "rules.h"
#include "item.h"
#include "stmt.h"

int main(int argc, char **argv)
{
  grammar gr;
  init_enum(&gr);
  show_rules(&gr);
  print_item_set_all(&gr);
  print_core(&gr);
}
