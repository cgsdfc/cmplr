#ifndef EXPERIMENT_IMPL_GRAMMAR_IPP
#define EXPERIMENT_IMPL_GRAMMAR_IPP 1

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/serialization/base_object.hpp>
#include <utility>  // for std::pair

namespace experiment {

template <class Archive>
void grammar::serialize(Archive& ar, const unsigned version) {
  ar& m_item_map;
  ar& m_itemset_map;
  ar& m_graph;
}
inline void grammar::add_edge(itemset_unique_id from, itemset_unique_id to,
                              symbol_unique_id symbol) {
  typedef typename graph_type::vertex_descriptor Vertex;
  boost::add_edge(Vertex(from), Vertex(to), symbol, m_graph);
}
inline item_type grammar::make_item(size_type dot,
                                             rule_unique_id rule) {
  return item_type(dot, rule);
}

inline std::pair<grammar::graph_type::vertex_iterator,
                 grammar::graph_type::vertex_iterator>
grammar::vertices() const {
  return boost::vertices(m_graph);
}
inline std::pair<grammar::graph_type::edge_iterator,
                 grammar::graph_type::edge_iterator>
grammar::edges() const {
  return boost::edges(m_graph);
}
inline grammar::symbol_unique_id grammar::symbol_on_edge(
    graph_type::edge_descriptor E) const {
  auto property_map = boost::get(boost::edge_name_t(), m_graph);
  return property_map[E];
}
inline grammar::itemset_unique_id grammar::target(
    graph_type::edge_descriptor E) const {
  return item_unique_id(boost::target(E, m_graph));
}
inline grammar::itemset_unique_id grammar::source(
    graph_type::edge_descriptor E) const {
  return item_unique_id(boost::source(E, m_graph));
}
inline bool grammar::item_dot_reach_end(const item_type& item) const {
  rule_type const& rule = m_lang.rule(item.rule());
  return rule.body().size() == item.dot();
}
inline grammar::symbol_unique_id grammar::symbol_at_dot(
    const item_type& item) const {
  return m_lang.rule((item.rule())).body().at(item.dot());
}
inline itemset_type const& grammar::itemset_on_vertex(
    graph_type::vertex_descriptor V) const {
  return m_itemset_map[itemset_unique_id(V)];
}
inline item_type const& grammar::item(item_unique_id id) const {
  return m_item_map[id];
}
inline grammar::size_type grammar::num_itemsets() const {
  return m_itemset_map.size();
}
inline rule_type const& grammar::rule(item_type const& item) const {
  return m_lang.rule(item.rule());
}
inline rule_type const& grammar::rule(rule_unique_id id) const {
  return m_lang.rule(id);
}
inline itemset_type const& grammar::itemset(
    itemset_unique_id id) const {
  return m_itemset_map[id];
}
inline std::pair<grammar::item_iterator, grammar::item_iterator>
grammar::items() const {
  return std::make_pair(m_item_map.vbegin(), m_item_map.vend());
}
inline std::pair<grammar::itemset_iterator, grammar::itemset_iterator>
grammar::itemsets() const {
  return std::make_pair(m_itemset_map.vbegin(), m_itemset_map.vend());
}
inline std::pair<grammar::enum_item_iterator, grammar::enum_item_iterator>
grammar::enum_items() const {
  return std::make_pair(m_item_map.ibegin(), m_item_map.iend());
}

inline std::pair<grammar::enum_itemset_iterator, grammar::enum_itemset_iterator>
grammar::enum_itemsets() const {
  return std::make_pair(m_itemset_map.ibegin(), m_itemset_map.iend());
}

}  // namespace experiment
#endif  // EXPERIMENT_IMPL_GRAMMAR_IPP
