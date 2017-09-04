#ifndef EXPERIMENT_ITEMSET_HPP
#define EXPERIMENT_ITEMSET_HPP 1
#include "rule_type.hpp"

namespace experiment {
  typedef std::size_t size_type;
  typedef std::pair<size_type, typename rule_traits::unique_id>
    item_type_base;

  struct item_type : public item_type_base {
    typedef item_type_base base_t;
    typedef rule_traits::unique_id rule_unique_id;
    item_type(size_type dot, rule_unique_id ruleid) : base_t(dot, ruleid) {}
    size_type dot() const { return first; }
    rule_unique_id rule() const { return second; }
  };
  typedef subject_traits<item_type> item_traits;
  struct itemset_type : public std::vector<typename item_traits::unique_id> {};
  typedef subject_traits<itemset_type> itemset_traits;
}  // namespace experiment

#endif // EXPERIMENT_ITEMSET_HPP
