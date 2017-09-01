#ifndef EXPERIMENT_SYMBOL_H
#define EXPERIMENT_SYMBOL_H 1
#include <string>
#include <functional>
#include <iostream>
#include <boost/serialization/access.hpp>

namespace experiment {
  enum class symbol_property {
    start,
    eof,
    terminal,
    nontermial,
    optional,
    reserved,
    unknown,
  };

  class symbol {
    private:
      friend class boost::serialization::access;
      template<class Archive>
        void serialize(Archive& ar, const unsigned version) {
          ar & m_prop;
          ar & m_str;
        }
      symbol_property m_prop;
      std::string m_str;
    public:
      symbol(const char *str, symbol_property prop)
        : m_str(str),
        m_prop(prop) {}

      symbol(const char *str)
        : m_str(str),
        m_prop(symbol_property::unknown) {}
      symbol()
        : m_str(),
        m_prop(symbol_property::unknown) {}
      operator std::string() const {
        return m_str;
      }

      void set(symbol_property prop) {
        m_prop=prop;
      }

      bool unknown() const {
        return m_prop == symbol_property::unknown;
      }

      bool terminal() const {
        return m_prop == symbol_property::terminal;
      }

      bool nontermial() const {
        return m_prop == symbol_property::nontermial;
      }

      struct hash: public std::hash<std::string> {};
      struct equal_to: public std::equal_to<std::string> {};
      friend std::ostream& operator<< (std::ostream& os, symbol const& s) {
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

  inline symbol eof(const char *str) {
    return symbol(str, symbol_property::eof);
  }
}
#endif
