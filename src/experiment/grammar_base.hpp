#ifndef EXPERIMENT_GRAMMAR_BASE_HPP
#define EXPERIMENT_GRAMMAR_BASE_HPP 1
#include <boost/iterator/filter_iterator.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <initializer_list>
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
  struct rule_type
      : public std::pair<symbol_unique_id, std::vector<symbol_unique_id>> {
    typedef std::pair<symbol_unique_id, std::vector<symbol_unique_id>> base_t;
    typedef second_type body_type;
    /* typedef typename base_t::first_type first_type; */
    rule_type(symbol_unique_id symbol,
              std::initializer_list<symbol_unique_id> list)
        : base_t(symbol, list) {}
    rule_type() : base_t() {}
    symbol_unique_id head() const {
      return first;
    }
    body_type const& body() const {
      return second;
    }
  };
  typedef unique_map<rule_type> rule_unique_map;
  typedef typename rule_unique_map::unique_id rule_unique_id;
  // structure for fast accessing the rules of each nonterminals
  typedef std::vector<std::vector<rule_unique_id>> nonterminal2rule_map;
  // iterator
  typedef typename symbol_unique_map::value_iterator symbol_iterator;
  typedef typename rule_unique_map::value_iterator rule_iterator;
  typedef boost::filter_iterator<is_terminal, symbol_iterator>
      terminal_iterator;
  typedef boost::filter_iterator<is_nonterminal, symbol_iterator>
      nonterminal_iterator;

 protected:
  symbol_unique_id m_start_symbol_id;
  symbol_unique_id m_eof_symbol_id;
  symbol_unique_map m_symbol_map;
  rule_unique_map m_rule_map;
  nonterminal2rule_map m_nonterminal2rule;
  rule_unique_id m_principle_rule;

 public:
  rule_unique_id principle_ruleid() const { return m_principle_rule; }
  size_type num_rules() const { return m_rule_map.size(); }
  size_type num_nonterminals() const { return m_nonterminal2rule.size(); }
  size_type num_symbols() const { return m_symbol_map.size(); }
  symbol const& get_symbol(symbol_unique_id id) const { return m_symbol_map[id]; }
  rule_type const& rule(rule_unique_id id) const { return m_rule_map[id]; }
  size_type num_terminals() const { return num_symbols() - num_nonterminals(); }
  grammar_base(const language& lang);

 private:
  symbol make_start() const;
  symbol make_eof()const;
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
 private:
  friend class boost::serialization::access;
  template <class Archive>
    void serialize(Archive& ar, const unsigned version) {
      ar& m_start_symbol_id;
      ar& m_eof_symbol_id;
      ar& m_symbol_map;
      ar& m_rule_map;
      ar& m_nonterminal2rule;
      ar& m_principle_rule;
    }

  // throws bad_symbol
};
}  // namespace experiment
#endif  // EXPERIMENT_GRAMMAR_BASE_HPP
