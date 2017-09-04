#include "unique_map.hpp"
#include "symbol.hpp"
#include "rule_type.hpp"
#include "itemset.hpp"

using namespace experiment;
typedef experiment::unique_map<std::string> string_map;
typedef experiment::subject_traits<experiment::symbol> symbol_traits;

int main(int ac,char**av){ 
  string_map map;
  symbol_traits::map_type symbol_unique_map;
  symbol_traits::unique_id id;
  symbol s("aaa");
  symbol ss("aa", symbol_property::terminal);
  symbol sss(std::string("aa"));
  rule_type rule;
  rule_traits::unique_id ruleid;
  rule_traits::map_type rule_unique_map;
}
