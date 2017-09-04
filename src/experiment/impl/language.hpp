#ifndef EXPERIMENT_LANGUAGE_HPP
#define EXPERIMENT_LANGUAGE_HPP 1
#include <boost/format.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <unordered_map>
#include "indent.hpp"
#include "rule.hpp"

namespace experiment {
class language {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned version) {
    ar& m_name;
    ar& m_rule_map;
  }

 private:
  typedef std::unordered_map<symbol, rule_group, symbol::hash, symbol::equal_to>
      rule_map_type;
  std::string m_name;
  rule_map_type m_rule_map;

 public:
  rule_adder operator[](symbol&& sym);
  language(const char* name) : m_name(name) {}
  language() {}
  void name(const char* str) { m_name = std::string(str); }
  rule_adder operator[](const char* str) { return operator[](symbol(str)); }

 public:
  friend std::ostream& operator<<(std::ostream& os, const language& lang);

 public:
  typedef rule_map_type::const_iterator const_iterator;
  typedef rule_map_type::size_type size_type;
  const_iterator begin() const { return m_rule_map.begin(); }
  const_iterator end() const { return m_rule_map.end(); }
  size_type num_nonterminals() const { return m_rule_map.size(); }
  size_type num_rules() const;
};

// helper for rule construction
inline symbol reserved(const char* str) {
  return symbol(str, symbol_property::reserved);
}

inline symbol terminal(const char* str) {
  return symbol(str, symbol_property::terminal);
}

inline symbol optional(const char* str) {
  return symbol(str, symbol_property::optional);
}

}  // namespace experiment
#endif  // EXPERIMENT_LANGUAGE_HPP
