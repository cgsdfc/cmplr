#ifndef EXPERIMENT_RULE_TYPE_HPP 
#define EXPERIMENT_RULE_TYPE_HPP 1
#include <initializer_list>
#include <vector>
#include "unique_map.hpp"
#include "symbol.hpp"

namespace experiment {
  typedef 
    std::pair<typename subject_traits<symbol>::unique_id,
    std::vector<typename subject_traits<symbol>::unique_id>> 
    rule_type_base;

struct rule_type: public rule_type_base {
  typedef rule_type_base base_t;
  typedef first_type head_type;
  typedef second_type body_type;
  typedef typename 
    subject_traits<symbol>::unique_id
    symbol_unique_id;
  rule_type(symbol_unique_id symbol,
      std::initializer_list<symbol_unique_id> list)
    : base_t(symbol, list) {}
  rule_type() : base_t() {}
  rule_type(head_type const& head, body_type const& body)
    : base_t(head, body) {}
  head_type const& head() const { return first; }
  body_type const& body() const { return second; }
  // overload
  head_type & head()  { return first; }
  body_type & body()  { return second; }
};
typedef subject_traits<rule_type> rule_traits;
} // namespace experiment
#endif // EXPERIMENT_RULE_TYPE_HPP
