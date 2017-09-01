#ifndef EXPERIMENT_GRAMMAR_HPP
#define EXPERIMENT_GRAMMAR_HPP 1
#include "language.hpp"
#include "unique_map.hpp"
#include "symbol.hpp"
#include <boost/iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>

namespace experiment {
  template<>
    struct unique_traits<symbol> {
      typedef std::size_t unique_id;
      typedef std::unordered_map<
        symbol, unique_id, symbol::hash, symbol::equal_to>
        map_type;
    };

class grammar {

  private:
    typedef std::size_t size_type;
    // symbol is the atom of this system
    typedef unique_map<symbol> symbol_unique_map;
    typedef typename symbol_unique_map::unique_id symbol_unique_id;

    // rules are composed by symbols
    typedef std::pair<symbol_unique_id, std::vector<symbol_unique_id>>
      rule_type;
    typedef unique_map<rule_type> rule_unique_map;
    typedef typename rule_unique_map::unique_id rule_unique_id;

    // item formed by adding a dot to a rule
    typedef std::pair<size_type, rule_unique_id> item_type;
    typedef unique_map<item_type> item_unique_map;
    typedef typename item_unique_map::unique_id item_unique_id;

    // itemset is just a set of items
    typedef std::vector<item_unique_id> itemset_type;
    typedef unique_map<itemset_type> itemset_unique_map;
    typedef typename itemset_unique_map::unique_id itemset_unique_id;

    // structure for fast accessing the rules of each nontermials
    typedef std::vector<std::vector<rule_unique_id>> nontermial2rule_map;

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
    symbol_unique_map m_symbol_map;
    rule_unique_map m_rule_map;
    item_unique_map m_item_map;
    itemset_unique_map m_itemset_map;
    nontermial2rule_map m_nonterminal2rule;
    graph_type m_graph;

  private:
    void add_edge(itemset_unique_id from, itemset_unique_id to, symbol_unique_id symbol) {
      typedef typename graph_type::vertex_descriptor Vertex;
      boost::add_edge(Vertex(from), Vertex(to), symbol, m_graph);
    }
  
    void build_itemset();
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

  private:
    void resolve_symbols(const language& lang);
    void resolve_rules(const language& lang);
  public:
    // populate the grammar
    void accept(const language& lang);
  public:
    typedef typename symbol_unique_map::item_iterator symbol_iterator;
    typedef typename rule_unique_map::item_iterator rule_iterator;
    std::pair<symbol_iterator, symbol_iterator>
      symbols() const {
        return std::make_pair(
            m_symbol_map.ibegin(),
            m_symbol_map.iend()
        );
      }
    std::pair<rule_iterator, rule_iterator>
      rules() const {
        return std::make_pair(
            m_rule_map.ibegin(),
            m_rule_map.iend()
        );
      }

};
} // namespace experiment

#endif

