#ifndef EXPERIMENT_RULS_H
#define EXPERIMENT_RULS_H 1

#include <vector>
#include "symbol.hpp"
namespace experiment {

struct rule_group {
  typedef std::vector<std::vector<symbol>> container_type;
   container_type m_bodies;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;

  const_iterator begin() const {
    return m_bodies.begin();
  }
  const_iterator end() const {
    return m_bodies.end();
  }
};

struct rule_adder {
  std::vector<symbol> m_body;
  rule_group& m_rules;
  
  rule_adder(rule_group & rules)
    : m_rules(rules), m_body() {}

  template<class... Symbol>
    rule_adder& operator() (Symbol&&... syms);
  template<class Head, class... Tail>
    rule_adder& operator() (Head&& head, Tail&&... tail) {
      m_body.push_back(head);
      return operator() (tail...);
    }

  rule_adder& operator() () {
    m_rules.m_bodies.push_back(m_body);
    return *this;
  }
  ~rule_adder() {}

};
} // namespace experiment
#endif

