#ifndef EXPERIMENT_OSTREAM_HPP
#define EXPERIMENT_OSTREAM_HPP 1
#include "grammar.hpp"
#include "language.hpp"
// impl complex print logic
namespace experiment {
class printer {
 private:
  typedef std::ostream ostream_type;
  typedef ostream_type& ostream_reference;
  typedef grammar::symbol_unique_id symbol_unique_id;
  ostream_reference m_os;
 public:
  typedef const grammar& const_grammar;
  typedef const language& const_language;
  // subject
  typedef grammar::rule_type const& const_rule;
  typedef grammar::item_type  const& const_item;
  typedef grammar::itemset_type  const& const_itemset;
  // iterator
  typedef grammar::rule_iterator rule_iterator;
  typedef grammar::item_iterator item_iterator;
  typedef grammar::itemset_iterator itemset_iterator;

 public:
  printer(ostream_reference os) : m_os(os) {}

  void operator()(const_grammar);
  void operator()(const_language);
  // iterator
  void operator()(rule_iterator);
  void operator()(item_iterator);
  void operator()(itemset_iterator);
  // subject
  void operator()(const_rule);
  void operator()(const_item);
  void operator()(const_itemset);
  // grammar + iterator
  void operator()(const_grammar, rule_iterator);
  void operator()(const_grammar, item_iterator);
  void operator()(const_grammar, itemset_iterator);
  // grammar + subject
  void operator()(const_grammar, const_rule);
  void operator()(const_grammar, const_item);
  void operator()(const_grammar, const_itemset);
};
}  // namespace experiment

#endif  // EXPERIMENT_OSTREAM_HPP