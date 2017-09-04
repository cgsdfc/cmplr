#ifndef EXPERIMENT_DETAIL_LANGUAGE_HPP
#define EXPERIMENT_DETAIL_LANGUAGE_HPP 1
#include <boost/iterator/filter_iterator.hpp>  // for filter_iterator
#include <initializer_list>
#include <utility>  // for std::pair

namespace experiment {
template <>
struct unique_traits<symbol> {
  typedef std::size_t unique_id;
  typedef std::unordered_map<symbol, unique_id, symbol::hash, symbol::equal_to>
      map_type;
};

namespace detail {
// the types needed by class language
struct language_base {
  typedef int symbol_ref_graph; // temporary
  typedef std::size_t size_type;
  typedef unique_map<symbol, unique_traits<symbol>> symbol_unique_map;
  typedef typename symbol_unique_map::unique_id symbol_unique_id;
  struct rule_type
      : public std::pair<symbol_unique_id, std::vector<symbol_unique_id>> {
    typedef std::pair<symbol_unique_id, std::vector<symbol_unique_id>> base_t;
    typedef first_type head_type;
    typedef second_type body_type;
    rule_type(symbol_unique_id symbol,
              std::initializer_list<symbol_unique_id> list)
        : base_t(symbol, list) {}
    rule_type() : base_t() {}
    head_type const& head() const { return first; }
    body_type const& body() const { return second; }
  };
  typedef unique_map<rule_type> rule_unique_map;
  typedef typename rule_unique_map::unique_id rule_unique_id;
  struct nonterminal2rule_map : public matrix<rule_unique_id> {
    typedef matrix<rule_unique_id> base_t;
    typedef typename base_t::size_type size_type;
    nonterminal2rule_map(size_type size) : base_t(size) {}
  };
  typedef typename symbol_unique_map::value_iterator symbol_iterator;
  typedef typename rule_unique_map::value_iterator rule_iterator;
  typedef boost::filter_iterator<is_terminal, symbol_iterator>
      terminal_iterator;
  typedef boost::filter_iterator<is_nonterminal, symbol_iterator>
      nonterminal_iterator;
};
}  // namespace detail
}  // namespace experiment
#endif  // EXPERIMENT_DETAIL_LANGUAGE_HPP
