#ifndef EXPERIMENT_IMPL_LANGUAGE_IPP
#define EXPERIMENT_IMPL_LANGUAGE_IPP 1
namespace experiment {
inline rule_tree language::operator[](const char* str) {
  return rule_tree(register_symbol(str, symbol_property::unknown), *this);
}
inline language::rule_unique_id language::principle_ruleid() const {
  return m_principle_rule;
}
inline language::symbol_unique_id language::eof() const {
  return m_eof_symbol_id;
}
inline language::symbol_unique_id language::start() const {
  return m_start_symbol_id;
}
inline language::symbol_unique_id language::epsilon() const {
  return m_epsilon_symbol_id;
}

inline language::size_type language::num_rules() const {
  return m_rule_map.size();
}
inline void language::register_rule(rule_type&& rule) {
  auto rule_id = m_rule_map[rule];
  m_nonterminal2rule[rule.head()].push_back(rule_id);
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

inline language::rule_type const& language::rule(rule_unique_id id) const {
  return m_rule_map[id];
}

inline language::size_type language::num_terminals() const {
  return num_symbols() - num_nonterminals();
}

inline std::pair<language::symbol_iterator, language::symbol_iterator>
language::symbols() const {
  return std::make_pair(m_symbol_map.vbegin(), m_symbol_map.vend());
}
inline language::symbol_unique_id language::register_symbol(
    const char* str, symbol_property property) {
  return m_symbol_map[symbol(str, property)];
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
  ar& m_start_symbol_id;
  ar& m_eof_symbol_id;
  ar& m_epsilon_symbol_id;
  ar& m_principle_rule;
  ar& m_symbol_map;
  ar& m_rule_map;
  ar& m_nonterminal2rule;
}
}  // namespace experiment
#endif  // EXPERIMENT_IMPL_LANGUAGE_IPP
