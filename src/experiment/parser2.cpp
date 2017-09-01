#include "grammar.hpp"
#include "unique_map.hpp"
#include "language.hpp"
#include "clang.hpp"
#include "small_lang.hpp"
#include <iostream>

using std::cout;
using std::string;

namespace experiment {
namespace debug {
  enum class which_lang {
    clang,
    simple,
    quick,
    easy,
  };
void print_lang(which_lang which) {
  language lang;
  switch(which) {
    case which_lang::clang:
      cout << clang::definition(lang);
      break;
    case which_lang::simple:
      cout << simple::definition(lang);
      break;
    case which_lang::quick:
      cout << quick::definition(lang);
      break;
    case which_lang::easy:
      cout << easy::definition(lang);
      break;
  }
}
void debug_grammar(const language& lang) {
  grammar g(lang);
  for (auto it=g.rules(); it.first!=it.second;++it.first) {
    g.print(cout, it.first);
  }
  cout << lang;
}
void print_lang_all() {
 print_lang(which_lang::simple);
  print_lang(which_lang::quick);
  print_lang(which_lang::easy);
}
} // namespace debug
} // namespac experiment

using namespace experiment::debug;
using namespace experiment;

int main(int,char**) {
  language lang;
  simple::definition(lang);
  debug_grammar(lang);
}
