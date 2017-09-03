#include "debug.hpp"

namespace experiment {
namespace debug {

void serial_clang() {
  language lang;
  string filen("/home/cong/download/gitrepo/tutor/cmplr/debug/clang.txt");
  ofstream ofs(filen);
  text_oarchive toa(ofs);
  toa << clang::definition(lang);
  /////////////////////////////
  language lang2;
  ifstream ifs(filen);
  text_iarchive tia(ifs);
  tia >> lang2;
  cout << lang2;
}

void debug_unique_map() {
  typedef unique_map<std::string> unique_map_string;
  unique_map_string string_map;
  auto id0 = string_map["sss"];
  auto id1 = string_map["sss"];
  auto s0 = string_map[id0];
  auto id2 = string_map["aaa"];
  auto s1 = string_map[id2];
}

void print_lang(which_lang which) {
  language lang;
  switch (which) {
    case which_lang::loop:
      cout << loop::definition(lang);
      break;
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
  printer p(cout);
  for (auto it = g.rules(); it.first != it.second; ++it.first) {
    p(it.first);
  }
  cout << lang;
}
void print_lang_all() {
  print_lang(which_lang::simple);
  print_lang(which_lang::quick);
  print_lang(which_lang::easy);
}
}  // namespace debug
}  // namespac experiment
