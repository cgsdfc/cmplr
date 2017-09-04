#ifndef EXPERIMENT_GRAMMAR_BASE_HPP
#define EXPERIMENT_GRAMMAR_BASE_HPP 1
#include <boost/serialization/access.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include "exception.hpp"
#include "matrix.hpp"
#include "printer_fwd.hpp"
#include "symbol.hpp"
#include "unique_map.hpp"
#include "rule_tree.hpp"
#include "detail/language.hpp"  // must come last

namespace experiment {
class language : public detail::language_base {
 public:
  friend class printer;

 private:
  symbol_unique_id m_start_symbol_id;
  symbol_unique_id m_eof_symbol_id;
  symbol_unique_id m_epsilon_symbol_id;
  rule_unique_id m_principle_rule;
  symbol_unique_map m_symbol_map;
  rule_unique_map m_rule_map;
  nonterminal2rule_map m_nonterminal2rule;
  symbol_ref_graph m_symbol_graph;
 public:
  rule_tree operator[] (const char *str) const {
    return rule_tree(m_symbol_map[symbol(str)], *this);
  }
 public:
  rule_unique_id principle_ruleid() const;
  symbol_unique_id eof() const;
  symbol_unique_id start() const;
  symbol_unique_id epsilon() const;
  size_type num_rules() const;
  size_type num_nonterminals() const;
  size_type num_symbols() const;
  size_type num_terminals() const;
  symbol const& get_symbol(symbol_unique_id) const;
  rule_type const& rule(rule_unique_id) const;
  language();

 private:
  friend class boost::serialization::access;
  friend class rule_node;
  friend class rule_tree;
  template <class Archive>
  void serialize(Archive& ar, const unsigned version);
  symbol_unique_id register_symbol(const char*, symbol_property prop=symbol_property::unknown);
  void register_rule(rule_type&&);
  void resolve_symbols(const language& lang);
  void resolve_rules(const language& lang);
  void sanity_check() const;

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
#endif  // EXPERIMENT_GRAMMAR_BASE_HPP
