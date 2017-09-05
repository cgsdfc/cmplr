#ifndef EXPERIMENT_LANGUAGE_HPP
#define EXPERIMENT_LANGUAGE_HPP 1
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/filter_iterator.hpp>  // for filter_iterator
#include <boost/serialization/access.hpp>
#include <utility>  // for std::pair
#include <vector>
#include "exception.hpp"
#include "rule_type.hpp"
#include "symbol.hpp"

namespace experiment {
class rule_tree;
class rule_node;
class printer;
class rule_adder;
class language {
 public:
  friend class printer;
  typedef symbol_traits::unique_id symbol_unique_id;
  typedef rule_traits::unique_id rule_unique_id;
  typedef symbol_traits::map_type symbol_unique_map;
  typedef rule_traits::map_type rule_unique_map;
  typedef std::size_t size_type;
  typedef symbol_traits::value_iterator symbol_iterator;
  typedef symbol_traits::item_iterator enum_symbol_iterator;
  typedef rule_traits::value_iterator rule_iterator;
  typedef rule_traits::item_iterator enum_rule_iterator;
  typedef boost::filter_iterator<is_terminal, symbol_iterator>
      terminal_iterator;
  typedef boost::filter_iterator<is_nonterminal, symbol_iterator>
      nonterminal_iterator;
  // named constants
  static const symbol_unique_id start = 0;
  static const symbol_unique_id eof = 1;
  static const symbol_unique_id epsilon = 2;
  static const rule_unique_id principle_rule = 0;

 private:
  typedef std::map<symbol_unique_id, std::vector<rule_unique_id>>
      nonterminal2rule_map;
  typedef boost::adjacency_list<> symbol_ref_graph;
  typedef typename symbol_ref_graph::vertex_descriptor
    vertex_descriptor;

 private:
  symbol_unique_map m_symbol_map;
  rule_unique_map m_rule_map;
  nonterminal2rule_map m_nonterminal2rule;
  symbol_ref_graph m_symbol_graph;
  symbol_unique_id m_list_count;
  symbol_unique_id m_optional_count;

 public:
  rule_tree&& operator[](const char* str);
  size_type num_rules() const;
  size_type num_nonterminals() const;
  size_type num_symbols() const;
  size_type num_terminals() const;
  symbol const& get_symbol(symbol_unique_id) const;
  rule_type const& rule(rule_unique_id) const;
  language();

 private:
  friend class boost::serialization::access;
  friend class rule_tree;
  friend class rule_node;
  friend class rule_adder;
  template <class Archive>
  void serialize(Archive& ar, const unsigned version);
  symbol_unique_id register_symbol(
      std::string const&, symbol_property prop = symbol_property::unknown);
  symbol_unique_id register_symbol(
      const char*, symbol_property prop = symbol_property::unknown);
  rule_unique_id register_rule(rule_type const&);
  void resolve_symbols();
  void resolve_rules();
  void sanity_check() const;
  symbol_unique_id make_list();
  symbol_unique_id make_optional();
  void add_edge(symbol_unique_id, symbol_unique_id);

  // iterator
 public:
  std::pair<symbol_iterator, symbol_iterator> symbols() const;
  std::pair<nonterminal_iterator, nonterminal_iterator> nonterminals() const;
  std::pair<terminal_iterator, terminal_iterator> terminals() const;
  std::pair<rule_iterator, rule_iterator> rules() const;
  symbol_unique_id find_symbol(std::string const& str) const;
};
}  // namespace experiment
#include "impl/language.ipp"
#endif  // EXPERIMENT_LANGUAGE_HPP
