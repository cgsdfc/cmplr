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
  void print_info(grammar const& g) {
    print("grammar.items");
    auto iter = g.items();
    std::for_each(iter.first /* begin */,
        iter.second /* end */,
        [&](printer::const_item item) {
        print(g, item);
        });
    print();
    print("grammar.rules");
    auto rules=g.rules();
    std::for_each(rules.first,
        rules.second,
        [&](printer::const_rule rule) {
        print(g, rule);
        });
    print();
    print("grammar.itemsets");
    for (auto iter = g.itemsets();
        iter.first != iter.second;
        ++iter.first) {
      print(g, iter.first);
    }
    print();
  }
}
}
using namespace experiment;
using namespace experiment::debug;

int main(int, char**) { 
  language lang;
  simple::definition(lang);
  grammar g(lang);
  print_info(g);
}
