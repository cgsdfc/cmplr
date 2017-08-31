#ifndef EXPERIMENT_LANGUAGE_H
#define EXPERIMENT_LANGUAGE_H 1
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/access.hpp>
#include <unordered_map>
#include <boost/format.hpp>
#include "rule.hpp"

namespace experiment {

  struct indent {
    unsigned & m_level;

    indent(std::ostream& os, unsigned & level)
      : m_level(++level) {
        for (int i=m_level; i--; os << ' ');
      }
    ~indent() { --m_level; }
  };

  class language {
    private:
      friend class boost::serialization::access;
      template<class Archive>
        void serialize(Archive& ar, const unsigned version) {
          ar & m_name;
          ar & m_rule_map;
        }
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
      language() {}

      void name(const char * str) {
        m_name=std::string(str);
      }

      rule_adder operator[] (const char *str) {
        return operator[] (symbol(str));
      }
      friend std::ostream& operator<< (std::ostream& os, const language& lang) {
        unsigned level=0;
        os << boost::format("language<%1%>\n") % lang.m_name;
        indent i_(os, level);
        for (auto& item: lang.m_rule_map) {
          os << boost::format("%1% ::=\n") % item.first;
          for (auto& rules: item.second) {
            indent i__(os, level); 
            for (auto& symbol: rules) {
              os << symbol << ' ';
            }
            os << '\n';
          }
        }
        return os;
      }
  };

  template<class Lang>
    struct lang_traits {
      typedef Lang lang_type;
      static const unsigned num_symbols = lang_type::num_symbols;
      static const unsigned num_terminals = lang_type::num_terminals;
      static const unsigned num_nonterminals = lang_type::num_nonterminals;
      static language& definition(language& lang) {
        return lang_type::definition(lang);
      }
    };
} // namespace experiment
#endif

