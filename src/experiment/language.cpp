#include "language.hpp"
// speed up compilation
namespace experiment {
std::ostream& operator<<(std::ostream& os, const language& lang) {
  unsigned level = 0;
  os << boost::format("language \"%1%\"\n\n") % lang.m_name;
  for (auto& item : lang.m_rule_map) {
    indent4 i_(os, level);
    os << boost::format("\"%1%\"\n") % item.first;
    for (auto& rules : item.second) {
      indent4 i__(os, level);
      for (auto& symbol : rules) {
        os << boost::format("\"%1%\"") % symbol << ' ';
      }
    }
  }
  return os;
}
rule_adder language::operator[](symbol&& sym) {
  if (sym.unknown()) {
    sym.set(symbol_property::nontermial);
  }
  return rule_adder(m_rule_map[sym]);
}
language::size_type language::num_rules() const {
  size_type result = 0;
  for (const auto& _ : m_rule_map) {
    result += _.second.size();
  }
  return result;
}

}  // experiment
