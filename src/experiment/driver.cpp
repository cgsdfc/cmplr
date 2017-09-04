#include "debug.hpp"
namespace experiment {
namespace debug {
  void symbol_stream_debug() {
    language lang;
    grammar gr(loop::definition(lang));
    istringstream is("a a a");
    symbol_stream sym_begin(is, gr);
    symbol_stream sym_end(gr);
    for (;;++sym_begin) {
      if (*sym_begin == gr.eof())
        break;
      cout << *sym_begin << endl;
    }
  }
  void print_symbol(grammar const& g) {
    print("grammar.symbols");
    for (auto iter = g.symbols();
        iter.first!=iter.second;
        ++iter.first) {
      print(*(iter.first));
    }
  }

  void print_info(grammar const& g) {
    print("grammar.itemsets");
    for (auto iter = g.enum_itemsets();
        iter.first != iter.second;
        ++iter.first) {
      print(g, *(iter.first));
    }
    print();
    print("grammar.edges");
    print(g, printer::core_item_diagram_tag());
    print();
  }
}
}
using namespace experiment;
using namespace experiment::debug;

int main(int, char**) { 
  language lang;
  clang::definition(lang);
  grammar g(lang);
  print_symbol(g);
}
