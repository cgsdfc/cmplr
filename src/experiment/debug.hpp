#ifndef EXPERIMENT_DEBUG_HPP
#define EXPERIMENT_DEBUG_HPP 1
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "clang.hpp"
#include "grammar.hpp"
#include "language.hpp"
#include "parser.hpp"
#include "printer.hpp"
#include "small_lang.hpp"
#include "unique_map.hpp"

using std::cout;
using std::endl;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::ofstream;
using std::ifstream;
using boost::archive::text_iarchive;
using boost::archive::text_oarchive;

namespace experiment {
namespace debug {
enum class which_lang {
  loop,
  clang,
  simple,
  quick,
  easy,
};
void serial_clang();
void debug_unique_map();
void print_lang(which_lang);
void debug_grammar(const language&);
void print_lang_all();

}  // namespace debug
}  // namespac experiment

#endif  // EXPERIMENT_DEBUG_HPP
