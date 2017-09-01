#ifndef EXPERIMENT_GRAMMAR_HPP
#define EXPERIMENT_GRAMMAR_HPP 1
#include "grammar_base.hpp"
#include <boost/iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>

namespace experiment {
 class grammar: public grammar_base {
  private:
    // item formed by adding a dot to a rule
    typedef std::pair<size_type, rule_unique_id> item_type;
    typedef unique_map<item_type> item_unique_map;
    typedef typename item_unique_map::unique_id item_unique_id;

    // itemset is just a set of items
    typedef std::vector<item_unique_id> itemset_type;
    typedef unique_map<itemset_type> itemset_unique_map;
    typedef typename itemset_unique_map::unique_id itemset_unique_id;

    // structure that helps visiting each group of items
    // divided by the symbol at dot(ASD)
    typedef std::map<symbol_unique_id, itemset_type> symbol2itemset_map;

    // for the state diagram to be generated
    // since the graph should be mutable and sparse,
    // use adjacency_list.
    typedef boost::adjacency_list <boost::vecS, boost::vecS,
            boost::directedS,
            boost::no_property,
            boost::property<boost::edge_name_t, symbol_unique_id>>
              graph_type;
  private:
    item_unique_map m_item_map;
    itemset_unique_map m_itemset_map;
    graph_type m_graph;

  private:
    void build_itemset();
    void add_edge(itemset_unique_id from, itemset_unique_id to, symbol_unique_id symbol) {
      typedef typename graph_type::vertex_descriptor Vertex;
      boost::add_edge(Vertex(from), Vertex(to), symbol, m_graph);
    }
    void do_closure(itemset_type& itemset);
    bool item_dot_reach_end(const item_type& item){
      rule_type const& rule = m_rule_map[item.second];
      return rule.second.size() == item.first;
    }
    symbol_unique_id symbol_at_dot(const item_type& item){
      return m_rule_map[item.second].second[item.first];
    }
    item_type make_item(size_type dot, rule_unique_id rule) {
      return std::make_pair(dot, rule);
    }
  public:
    grammar(const language& lang)
      :grammar_base(lang) {
        build_itemset();
      }
  friend std::ostream& operator<< (std::ostream& os, const grammar& g);
};
} // namespace experiment
#endif // EXPERIMENT_GRAMMAR_HPP

