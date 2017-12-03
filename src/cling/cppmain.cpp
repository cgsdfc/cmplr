#include <cling/cling.hpp>

int main (int argc, char ** argv) {
  FILE *file=fopen(argv[1], "r");
  cling::scanner scanner(file);
  cling::symbol_table symbol_table;
  cling::parser parser(symbol_table);
  auto tree=parser.parse(scanner);
  tree.pretty_print();
  tree.destroy();
  return 0;
}
