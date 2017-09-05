#ifndef EXPERIMENT_IMPL_LANGUAGE_IPP
#define EXPERIMENT_IMPL_LANGUAGE_IPP 1
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
namespace experiment {
inline void
language::add_edge(symbol_unique_id source, symbol_unique_id target,rule_unique_id rule_on_edge) {
  boost::add_edge(
      vertex_descriptor(source),
      vertex_descriptor(target),
      rule_on_edge,
      m_symbol_graph
    );
}
inline language::size_type language::num_rules() const {
  return m_rule_map.size();
}

inline language::size_type language::num_nonterminals() const {
  return m_nonterminal2rule.size();
}

inline language::size_type language::num_symbols() const {
  return m_symbol_map.size();
}

inline symbol const& language::get_symbol(symbol_unique_id id) const {
  return m_symbol_map[id];
}

inline rule_type const& language::rule(rule_unique_id id) const {
  return m_rule_map[id];
}

inline language::size_type language::num_terminals() const {
  return num_symbols() - num_nonterminals();
}

inline std::pair<language::symbol_iterator, language::symbol_iterator>
language::symbols() const {
  return std::make_pair(m_symbol_map.vbegin(), m_symbol_map.vend());
}
inline std::pair<language::nonterminal_iterator, language::nonterminal_iterator>
language::nonterminals() const {
  return std::make_pair(boost::make_filter_iterator<is_nonterminal>(
                            m_symbol_map.vbegin(), m_symbol_map.vend()),
                        boost::make_filter_iterator<is_nonterminal>(
                            m_symbol_map.vend(), m_symbol_map.vend()));
}
inline std::pair<language::terminal_iterator, language::terminal_iterator>
language::terminals() const {
  return std::make_pair(boost::make_filter_iterator<is_terminal>(
                            m_symbol_map.vbegin(), m_symbol_map.vend()),
                        boost::make_filter_iterator<is_terminal>(
                            m_symbol_map.vend(), m_symbol_map.vend()));
}
inline std::pair<language::rule_iterator, language::rule_iterator>
language::rules() const {
  return std::make_pair(m_rule_map.vbegin(), m_rule_map.vend());
}
inline language::symbol_unique_id language::find_symbol(
    std::string const& str) const {
  auto id = m_symbol_map.find(symbol(str));
  if (id == symbol_unique_map::npos) throw bad_symbol();
  return id;
}

template <class Archive>
void language::serialize(Archive& ar, const unsigned version) {
  ar& m_symbol_map;
  ar& m_rule_map;
  ar& m_nonterminal2rule;
}
}  // namespace experiment
#endif  // EXPERIMENT_IMPL_LANGUAGE_IPP
