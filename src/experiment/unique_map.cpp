#include "unique_map.hpp"
#include "symbol.hpp"
#include "rule_type.hpp"
#include "itemset.hpp"
#include "rule_tree.hpp"
#include "language.hpp"

using namespace experiment;
typedef experiment::unique_map<std::string> string_map;
typedef experiment::subject_traits<experiment::symbol> symbol_traits;

int main(int ac,char**av){ 
  language lang;
  lang["fucK"]("11");
}
