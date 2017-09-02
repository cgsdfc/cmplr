#ifndef EXPERIMENT_SYMBOL_HPP
#define EXPERIMENT_SYMBOL_HPP 1
#include <boost/serialization/access.hpp>
#include <functional>
#include <iostream>
#include <string>

namespace experiment {
enum class symbol_property {
  terminal,    // general terminal.
  nontermial,  // general nontermial.
  optional,    // nontermial that has a nullable body.
  start,       // nontermial that serves as the head of first rule.
  eof,         // terminal that serves as end of file.
  reserved,    // terminal that is a reserved word.
  unknown,     // illegal value.
};

class symbol {
 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned version) {
    ar &m_prop;
    ar &m_str;
  }
  symbol_property m_prop;
  std::string m_str;

 public:
  symbol(const char *str, symbol_property prop) : m_str(str), m_prop(prop) {}

  symbol(const char *str) : m_str(str), m_prop(symbol_property::unknown) {}
  symbol() : m_str(), m_prop(symbol_property::unknown) {}
  operator std::string() const { return m_str; }

 public:
  void set(symbol_property prop) { m_prop = prop; }
  bool unknown() const { return m_prop == symbol_property::unknown; }
  bool terminal() const {
    return m_prop == symbol_property::terminal ||
           m_prop == symbol_property::reserved ||
           m_prop == symbol_property::eof;
  }
  bool nontermial() const {
    return m_prop == symbol_property::nontermial ||
           m_prop == symbol_property::optional ||
           m_prop == symbol_property::start;
  }
  bool start() const { return m_prop == symbol_property::start; }
  bool eof() const { return m_prop == symbol_property::eof; }

 public:
  struct hash : public std::hash<std::string> {};
  struct equal_to : public std::equal_to<std::string> {};
  friend std::ostream &operator<<(std::ostream &os, symbol const &s) {
    return os << s.m_str;
  }
};
inline symbol reserved(const char *str) {
  return symbol(str, symbol_property::reserved);
}

inline symbol terminal(const char *str) {
  return symbol(str, symbol_property::terminal);
}

inline symbol optional(const char *str) {
  return symbol(str, symbol_property::optional);
}

inline symbol start(const char *str) {
  return symbol(str, symbol_property::start);
}

inline symbol eof(const char *str) { return symbol(str, symbol_property::eof); }
}
#endif  // EXPERIMENT_SYMBOL_HPP
