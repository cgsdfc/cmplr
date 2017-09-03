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
}
}

int main(int, char**) { 
  experiment::debug::symbol_stream_debug();
}
