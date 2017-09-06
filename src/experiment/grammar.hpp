#ifndef EXPERIMENT_GRAMMAR_HPP
#define EXPERIMENT_GRAMMAR_HPP 1
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator.hpp>
#include <vector>
#include "itemset.hpp"
#include "language.hpp"

namespace experiment {
class grammar{
 public:
  friend class printer;
  typedef std::size_t size_type;
  // item
  typedef typename item_traits::map_type item_unique_map;
  typedef typename item_traits::unique_id item_unique_id;
  typedef typename item_traits::value_iterator item_iterator;
  typedef typename item_traits::item_iterator enum_item_iterator;
  // itemset
  typedef typename itemset_traits::map_type itemset_unique_map;
  typedef typename itemset_traits::unique_id itemset_unique_id;
  typedef typename itemset_traits::value_iterator itemset_iterator;
  typedef typename itemset_traits::item_iterator enum_itemset_iterator;
  // rule
  typedef rule_traits::unique_id rule_unique_id;
  // symbol
  typedef symbol_traits::unique_id symbol_unique_id;
 private:
  typedef std::map<symbol_unique_id, itemset_type> symbol2itemset_map;
  typedef boost::adjacency_list<
      boost::vecS, boost::vecS, boost::directedS, boost::no_property,
      boost::property<boost::edge_name_t, symbol_unique_id>>
      graph_type;
  typedef boost::graph_traits<graph_type>::edge_iterator edge_iterator;
  typedef boost::graph_traits<graph_type>::edge_descriptor edge_descriptor;
  typedef boost::graph_traits<graph_type>::vertex_iterator vertex_iterator;
  typedef boost::graph_traits<graph_type>::vertex_descriptor vertex_descriptor;

 private:
  const language& m_lang;
  typedef std::ostream& ostream_reference;
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
  std::pair<vertex_iterator, vertex_iterator>
    vertices() const;
  std::pair<edge_iterator, edge_iterator> edges()
    const;
  symbol_unique_id symbol_on_edge(edge_descriptor E) const;
  itemset_unique_id target(edge_descriptor E) const;
  itemset_unique_id source(edge_descriptor E) const;
 public:
  bool item_dot_reach_end(const item_type& item) const;
  symbol_unique_id symbol_at_dot(const item_type& item) const;
  itemset_type const& itemset_on_vertex(vertex_descriptor V)
    const;
 public:
  item_type const& item(item_unique_id id) const;
  size_type num_itemsets() const;
  rule_type const& rule(item_type const& item) const;
  rule_type const& rule(rule_unique_id id) const;
  itemset_type const& itemset(itemset_unique_id id) const;
 public:
  std::pair<item_iterator, item_iterator> items() const;
  std::pair<itemset_iterator, itemset_iterator> itemsets() const;
  std::pair<enum_item_iterator, enum_item_iterator> enum_items() const;
  std::pair<enum_itemset_iterator, enum_itemset_iterator> enum_itemsets()
    const;
 public:
  void print(ostream_reference, item_type const&)const;
  void print(ostream_reference, itemset_type const&)const;
};
}  // namespace experiment
#include "impl/grammar.ipp"
#endif  // EXPERIMENT_GRAMMAR_HPP
