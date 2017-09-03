#include "grammar_base.hpp"

namespace experiment {
using std::cout;
using std::endl;
grammar_base::grammar_base(const language& lang)
    : m_start_symbol_id(symbol_unique_map::npos),
      m_eof_symbol_id(symbol_unique_map::npos),
      m_nonterminal2rule(lang.num_nonterminals()+1) { /* __start__ */
  m_rule_map.reserve(lang.num_rules()+1); /* principle_rule */
  resolve_symbols(lang);
  resolve_rules(lang);
  sanity_check();
}

symbol
grammar_base::make_start() const {
  return symbol("__start__", symbol_property::nonterminal);
}

symbol
grammar_base::make_eof() const {
  return symbol("__eof__", symbol_property::terminal);
}
// populate m_symbol_map
// eliminate unknown property
void grammar_base::resolve_symbols(const language& lang) {
  m_start_symbol_id = m_symbol_map[make_start()];
  assert(m_start_symbol_id == 0);
  for (auto bundle : lang) {
    symbol head = bundle.first;
    if (bundle.second.size() == 0) head.set(symbol_property::optional);
    m_symbol_map[head];
  }  // make nonterminals have the
  // lower part of ids.
  m_eof_symbol_id = m_symbol_map[make_eof()];
  for (auto bundle : lang) {
    for (auto& body : bundle.second) {
      for (auto& part : body) {
        symbol mutable_part(part);
        if (mutable_part.unknown()) mutable_part.set(symbol_property::terminal);
        m_symbol_map[mutable_part];
      }
    }
  }
}
// populate m_nonterminal2rule , m_rule_map
//
void grammar_base::resolve_rules(const language& lang) {
  // add the principle_rule
  assert(m_symbol_map.size());
  rule_type principle{m_start_symbol_id, /* __start__ == 0 */
                      {symbol_unique_id(1), m_eof_symbol_id}};
  m_principle_rule = m_rule_map[principle];
  m_nonterminal2rule[m_start_symbol_id].push_back(m_principle_rule);
  assert(m_principle_rule == 0);
  for (auto bundle : lang) {
    // head = bundle.first;
    // body = bundle.second;
    auto head_id = m_symbol_map[bundle.first];
    for (const auto& alter : bundle.second) {
      rule_type rule;
      rule.first = head_id;
      for (const auto& part : alter) {
        auto part_id = m_symbol_map[part];
        rule.second.push_back(part_id);
      }
      auto ruleid = m_rule_map[rule];
      m_nonterminal2rule[head_id].push_back(ruleid);
    }
  }
}
void grammar_base::sanity_check() const {
  if (m_start_symbol_id == symbol_unique_map::npos) throw no_start_symbol();
  if (m_eof_symbol_id == symbol_unique_map::npos) throw no_eof_symbol();
  if (m_nonterminal2rule[m_start_symbol_id].size() != 1)
    throw invalid_principle_rule();
}

std::pair<grammar_base::symbol_iterator, grammar_base::symbol_iterator>
grammar_base::symbols() const {
  return std::make_pair(m_symbol_map.vbegin(), m_symbol_map.vend());
}
std::pair<grammar_base::nonterminal_iterator,
          grammar_base::nonterminal_iterator>
grammar_base::nonterminals() const {
  return std::make_pair(boost::make_filter_iterator<is_nonterminal>(
                            m_symbol_map.vbegin(), m_symbol_map.vend()),
                        boost::make_filter_iterator<is_nonterminal>(
                            m_symbol_map.vend(), m_symbol_map.vend()));
}
std::pair<grammar_base::terminal_iterator, grammar_base::terminal_iterator>
grammar_base::terminals() const {
  return std::make_pair(boost::make_filter_iterator<is_terminal>(
                            m_symbol_map.vbegin(), m_symbol_map.vend()),
                        boost::make_filter_iterator<is_terminal>(
                            m_symbol_map.vend(), m_symbol_map.vend()));
}
std::pair<grammar_base::rule_iterator, grammar_base::rule_iterator>
grammar_base::rules() const {
  return std::make_pair(m_rule_map.vbegin(), m_rule_map.vend());
}

grammar_base::symbol_unique_id grammar_base::find_symbol(
    std::string const& str) const {
  auto id = m_symbol_map.find(symbol(str));
  if (id == symbol_unique_map::npos) throw bad_symbol();
  return id;
}

}  // namespace experiment
