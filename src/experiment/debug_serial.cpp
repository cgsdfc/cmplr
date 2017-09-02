#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>
#include <iostream>
#include "clang.hpp"
#include "grammar.hpp"
#include "language.hpp"
#include "small_lang.hpp"
#include "unique_map.hpp"

using std::cout;
using std::string;
using std::ofstream;
using std::ifstream;
using boost::archive::text_iarchive;
using boost::archive::text_oarchive;

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

}  // namespace debug
}  // namespac experiment
