#ifndef EXPERIMENT_SYMBOL_HPP
#define EXPERIMENT_SYMBOL_HPP 1
#include <boost/serialization/access.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <functional>
#include "subject_traits.hpp"

namespace experiment {
enum class symbol_property {
  terminal,     // general terminal.
  nonterminal,  // general nonterminal.
  optional,     // nonterminal that has a nullable body.
  unknown,      // illegal value.
};

class symbol {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned version);
  symbol_property m_prop;
  const std::string m_str;

 public:
  symbol(const char *str, symbol_property prop) : m_str(str), m_prop(prop) {}
  symbol(std::string const& str, symbol_property prop) : m_str(str), m_prop(prop) {}
  symbol(std::string const &str) : m_str(str) {}
  symbol(const char *str) : m_str(str), m_prop(symbol_property::unknown) {}
  symbol() : m_str(), m_prop(symbol_property::unknown) {}

 public:
  void set(symbol_property prop);
  bool unknown() const;
  bool terminal() const;
  bool nonterminal() const;
  bool optional() const;

 public:
  template<class T> friend struct std::hash;
  friend std::ostream &operator<<(std::ostream &os, symbol const &s);
  friend bool operator< (symbol const& left, symbol const& right);
  friend bool operator== (symbol const& left, symbol const& right);
};
struct is_terminal {
  bool operator()(symbol const &s) const { return s.terminal(); }
};
struct is_nonterminal {
  bool operator()(symbol const &s) const { return s.nonterminal(); }
};
typedef subject_traits<symbol> symbol_traits;
}  // namespace experiment
#include "impl/symbol.ipp"
#endif  // EXPERIMENT_SYMBOL_HPP
