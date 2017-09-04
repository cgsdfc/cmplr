#ifndef EXPERIMENT_DETAIL_GRAMMAR_HPP
#define EXPERIMENT_DETAIL_GRAMMAR_HPP 1
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <utility>  // for std::pair
#include <vector>
#include "language.hpp"

namespace experiment {
namespace detail {
struct grammar_base : public language_base {
  typedef std::size_t size_type;
  struct item_type : public std::pair<size_type, rule_unique_id> {
    typedef std::pair<size_type, rule_unique_id> base_t;
    item_type(size_type dot, rule_unique_id ruleid) : base_t(dot, ruleid) {}
    size_type dot() const { return first; }
    rule_unique_id rule() const { return second; }
  };
  typedef unique_map<item_type> item_unique_map;
  typedef typename item_unique_map::unique_id item_unique_id;
  // itemset is just a set of items
  struct itemset_type : public std::vector<item_unique_id> {};
  typedef unique_map<itemset_type> itemset_unique_map;
  typedef typename itemset_unique_map::unique_id itemset_unique_id;
  // structure that helps visiting each group of items
  // divided by the symbol at dot(ASD)
  typedef std::map<symbol_unique_id, itemset_type> symbol2itemset_map;
  // for the state diagram to be generated
  // since the graph should be mutable and sparse,
  // use adjacency_list.
  typedef boost::adjacency_list<
      boost::vecS, boost::vecS, boost::directedS, boost::no_property,
      boost::property<boost::edge_name_t, symbol_unique_id>>
      graph_type;
  // iterator
  typedef item_unique_map::value_iterator item_iterator;
  typedef itemset_unique_map::value_iterator itemset_iterator;
  // iterator with an id binded
  typedef item_unique_map::item_iterator enum_item_iterator;
  typedef itemset_unique_map::item_iterator enum_itemset_iterator;
};
}  // namespace detail
}  // namespace experiment

#endif  // EXPERIMENT_DETAIL_GRAMMAR_HPP
