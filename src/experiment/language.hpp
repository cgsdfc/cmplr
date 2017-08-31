#ifndef EXPERIMENT_LANGUAGE_H
#include <unordered_map>
#include "rule.hpp"

namespace experiment {
class language {
  private:
    std::string m_name;
    typedef std::unordered_map<symbol, rule_group, symbol::hash, symbol::equal_to> rule_map_type;
    rule_map_type m_rule_map;

  public:
    rule_adder operator[] (symbol&& sym) {
      if (sym.unknown()) {
        sym.set(symbol_property::nontermial);
      }
      return rule_adder(m_rule_map[sym]);
    }

    language(const char *name)
      : m_name(name) {}
     rule_adder operator[] (const char *str) {
      return operator[] (symbol(str));
    }
     friend std::ostream& operator<< (std::ostream& os, const language& lang) {
       os << lang.m_name;
       for (auto& item: lang.m_rule_map) {
         os << item.first;
         for (auto& rules: item.second) {
           for (auto& symbol: rules) {
             os << symbol << ' ';
           }
           os << '\n';
         }
       }
       return os;
     }
};
} // namespace experiment
#endif

