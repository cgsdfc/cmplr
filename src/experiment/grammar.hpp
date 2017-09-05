#ifndef EXPERIMENT_GRAMMAR_HPP
#define EXPERIMENT_GRAMMAR_HPP 1
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <vector>
#include "language.hpp"
#include "detail/grammar.hpp"

namespace experiment {
class grammar{
 public:
  friend class printer;
 private:
  const language& m_lang;
  item_unique_map m_item_map;
  itemset_unique_map m_itemset_map;
  graph_type m_graph;

 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned version);
  void build_itemset();
  void add_edge(itemset_unique_id from, itemset_unique_id to,
      symbol_unique_id symbol);
  void do_closure(itemset_type & itemset);
  item_type make_item(size_type dot, rule_unique_id rule);

 public:
  grammar(const language& lang):m_lang(lang) { build_itemset(); }
  std::pair<graph_type::vertex_iterator, graph_type::vertex_iterator>
    vertices() const;
  std::pair<graph_type::edge_iterator, graph_type::edge_iterator> edges()
    const;
  symbol_unique_id symbol_on_edge(graph_type::edge_descriptor E) const;
  itemset_unique_id target(graph_type::edge_descriptor E) const;
  itemset_unique_id source(graph_type::edge_descriptor E) const;
  bool item_dot_reach_end(const item_type& item) const;
  symbol_unique_id symbol_at_dot(const item_type& item) const;
  itemset_type const& itemset_on_vertex(graph_type::vertex_descriptor V)
    const;
  item_type const& item(item_unique_id id) const;
  size_type num_itemsets() const;
  symbol_unique_id eof() const;
  symbol_unique_id start() const;
  rule_type const& rule(item_type const& item) const;
  rule_type const& rule(rule_unique_id id) const;
  itemset_type const& itemset(itemset_unique_id id) const;
  std::pair<item_iterator, item_iterator> items() const;
  std::pair<itemset_iterator, itemset_iterator> itemsets() const;
  std::pair<enum_item_iterator, enum_item_iterator> enum_items() const;
  std::pair<enum_itemset_iterator, enum_itemset_iterator> enum_itemsets()
    const;
};
}  // namespace experiment
#include "impl/grammar.ipp"
#endif  // EXPERIMENT_GRAMMAR_HPP
