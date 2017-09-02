#ifndef EXPERIMENT_GRAMMAR_BASE_HPP
#define EXPERIMENT_GRAMMAR_BASE_HPP 1
#include "exception.hpp"
#include "language.hpp"
#include "printer_fwd.hpp"
#include "symbol.hpp"
#include "unique_map.hpp"

namespace experiment {
template <>
struct unique_traits<symbol> {
  typedef std::size_t unique_id;
  typedef std::unordered_map<symbol, unique_id, symbol::hash, symbol::equal_to>
      map_type;
};

class grammar_base {
  public:
  friend class printer;
  typedef std::size_t size_type;
  // symbol is the atom of this system
  typedef unique_map<symbol, unique_traits<symbol>> symbol_unique_map;
  typedef typename symbol_unique_map::unique_id symbol_unique_id;
  // rules are composed by symbols
  struct rule_type : public std::pair<symbol_unique_id, std::vector<symbol_unique_id>> {};
  typedef unique_map<rule_type> rule_unique_map;
  typedef typename rule_unique_map::unique_id rule_unique_id;
  // structure for fast accessing the rules of each nontermials
  typedef std::vector<std::vector<rule_unique_id>> nontermial2rule_map;
  // iterator
  typedef typename symbol_unique_map::value_iterator symbol_iterator;
  typedef typename rule_unique_map::value_iterator rule_iterator;


 protected:
  symbol_unique_id m_start_symbol_id;
  symbol_unique_id m_eof_symbol_id;
  symbol_unique_map m_symbol_map;
  rule_unique_map m_rule_map;
  nontermial2rule_map m_nonterminal2rule;

 public:
  rule_unique_id principle_ruleid() const {
    return m_nonterminal2rule[m_start_symbol_id][0];
  }
  size_type num_rules() const { return m_rule_map.size(); }
  size_type num_nonterminals() const { return m_nonterminal2rule.size(); }
  size_type num_symbols() const { return m_symbol_map.size(); }
 grammar_base(const language& lang);

 private:
  void resolve_symbols(const language& lang);
  void resolve_rules(const language& lang);
  void sanity_check() const;

  // iterator 
 public:
    std::pair<symbol_iterator, symbol_iterator> symbols() const {
    return std::make_pair(m_symbol_map.vbegin(), m_symbol_map.vend());
  }
  std::pair<rule_iterator, rule_iterator> rules() const {
    return std::make_pair(m_rule_map.vbegin(), m_rule_map.vend());
  }
};
}  // namespace experiment
#endif  // EXPERIMENT_GRAMMAR_BASE_HPP
