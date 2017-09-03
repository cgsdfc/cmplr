#ifndef EXPERIMENT_GRAMMAR_HPP
#define EXPERIMENT_GRAMMAR_HPP 1
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <boost/serialization/base_object.hpp>
#include "grammar_base.hpp"

namespace experiment {
class grammar : public grammar_base {
 public:
  friend class printer;
  // item formed by adding a dot to a rule
  struct item_type : public std::pair<size_type, rule_unique_id> {
    typedef std::pair<size_type, rule_unique_id> base_t;
    item_type(size_type dot, rule_unique_id ruleid) : base_t(dot, ruleid) {}
    size_type dot() const {
      return first;
    }
    rule_unique_id rule() const {
      return second;
    }
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

 private:
  item_unique_map m_item_map;
  itemset_unique_map m_itemset_map;
  graph_type m_graph;

 private:
  void build_itemset();
  void add_edge(itemset_unique_id from, itemset_unique_id to,
                symbol_unique_id symbol) {
    typedef typename graph_type::vertex_descriptor Vertex;
    boost::add_edge(Vertex(from), Vertex(to), symbol, m_graph);
  }
  void do_closure(itemset_type& itemset);
  item_type make_item(size_type dot, rule_unique_id rule) {
    return item_type(dot, rule);
  }

 private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned version) {
    ar& boost::serialization::base_object<grammar_base> (*this);
    ar& m_item_map;
    ar& m_itemset_map;
    ar& m_graph;
  }
 public:
  grammar(const language& lang) : grammar_base(lang) { build_itemset(); }

 public:
  std::pair<graph_type::vertex_iterator, graph_type::vertex_iterator> vertices()
      const {
    return boost::vertices(m_graph);
  }
  std::pair<graph_type::edge_iterator, graph_type::edge_iterator> edges()
      const {
    return boost::edges(m_graph);
  }
  symbol_unique_id symbol_on_edge(graph_type::edge_descriptor E) const {
    auto property_map = boost::get(boost::edge_name_t(), m_graph);
    return property_map[E];
  }
  itemset_unique_id target(graph_type::edge_descriptor E) const {
    return item_unique_id(boost::target(E, m_graph));
  }
  itemset_unique_id source(graph_type::edge_descriptor E) const {
    return item_unique_id(boost::source(E, m_graph));
  }
  bool item_dot_reach_end(const item_type& item) const {
    rule_type const& rule = m_rule_map[item.rule()];
    return rule.body().size() == item.dot();
  }
  symbol_unique_id symbol_at_dot(const item_type& item) const {
    return m_rule_map.at(item.rule()).body().at(item.dot());
  }
  itemset_type const& itemset_on_vertex(graph_type::vertex_descriptor V) const {
    return m_itemset_map[itemset_unique_id(V)];
  }
  item_type const& item(item_unique_id id) const { return m_item_map[id]; }
  size_type num_itemsets() const { return m_itemset_map.size(); }
  symbol_unique_id eof() const { return m_eof_symbol_id; }
  symbol_unique_id start() const { return m_start_symbol_id; }
  rule_type const& rule(item_type const& item)const { return m_rule_map[item.rule()]; }
  rule_type const& rule(rule_unique_id id)const { return m_rule_map[id]; }
  itemset_type const& itemset(itemset_unique_id id)const{return m_itemset_map[id]; }
  std::pair<item_iterator, item_iterator>
    items() const {
      return std::make_pair(m_item_map.vbegin(), m_item_map.vend());
    }
  std::pair<itemset_iterator, itemset_iterator>
    itemsets() const {
      return std::make_pair(m_itemset_map.vbegin(), m_itemset_map.vend());
    }
};
}  // namespace experiment
#endif  // EXPERIMENT_GRAMMAR_HPP
