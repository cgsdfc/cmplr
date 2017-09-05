#include "language.hpp"
#include "rule_tree.hpp"
#include <boost/format.hpp>

namespace experiment {
const language::symbol_unique_id
  language::start=0,
  language::eof=1,
  language::epsilon=2;
const language::rule_unique_id
  language::principle_rule=0;

language::language():m_optional_count(0),m_list_count(0) {
  register_symbol("__start__", symbol_property::nonterminal);
  register_symbol("__eof__", symbol_property::terminal);
  register_symbol("__epsilon__", symbol_property::terminal);
}

language::symbol_unique_id
language::register_symbol(const char *str, symbol_property property) {
  return m_symbol_map[symbol(str, property)];
}

language::symbol_unique_id
language::register_symbol(std::string const& str, symbol_property property) {
  return m_symbol_map[symbol(str, property)];
}

language::rule_unique_id
language::register_rule(rule_type const& rule) {
  auto ruleid = m_rule_map[rule];
  m_nonterminal2rule[rule.head()].push_back(ruleid);
  return ruleid;
}

language::symbol_unique_id
language::make_list() {
  auto fmt = boost::format("__list__%1%") % m_list_count;
  m_list_count ++;
  return register_symbol(fmt.str(), symbol_property::nonterminal);
}
language::symbol_unique_id
language::make_optional() {
  auto fmt = boost::format("__optional__%1%") % m_optional_count;
  ++ m_optional_count;
  return register_symbol(fmt.str(), symbol_property::nonterminal);
}

rule_tree language::operator[](const char* str) {
  return rule_tree(register_symbol(str, symbol_property::unknown), *this);
}
// eliminate unknown property
void language::resolve_symbols() {
  /* for (auto bundle : lang) { */
  /*   symbol head = bundle.first; */
  /*   if (bundle.second.size() == 0) head.set(symbol_property::optional); */
  /*   m_symbol_map[head]; */
  /* }  // make nonterminals have the */
  /* // lower part of ids. */
  /* m_eof_symbol_id = m_symbol_map[make_eof()]; */
  /* for (auto bundle : lang) { */
  /*   for (auto& body : bundle.second) { */
  /*     for (auto& part : body) { */
  /*       symbol mutable_part(part); */
  /*       if (mutable_part.unknown())
   * mutable_part.set(symbol_property::terminal); */
  /*       m_symbol_map[mutable_part]; */
  /*     } */
  /*   } */
  /* } */
}
// populate m_nonterminal2rule , m_rule_map
//
void language::resolve_rules() {
  // add the principle_rule
  /* assert(m_symbol_map.size()); */
  /* rule_type principle{m_start_symbol_id, /1* __start__ == 0 *1/ */
  /*                     {symbol_unique_id(1), m_eof_symbol_id}}; */
  /* m_principle_rule = m_rule_map[principle]; */
  /* m_nonterminal2rule[m_start_symbol_id].push_back(m_principle_rule); */
  /* assert(m_principle_rule == 0); */
  /* for (auto bundle : lang) { */
  /*   // head = bundle.first; */
  /*   // body = bundle.second; */
  /*   auto head_id = m_symbol_map[bundle.first]; */
  /*   for (const auto& alter : bundle.second) { */
  /*     rule_type rule; */
  /*     rule.first = head_id; */
  /*     for (const auto& part : alter) { */
  /*       auto part_id = m_symbol_map[part]; */
  /*       rule.second.push_back(part_id); */
  /*     } */
  /*     auto ruleid = m_rule_map[rule]; */
  /*     m_nonterminal2rule[head_id].push_back(ruleid); */
  /*   } */
  /* } */
}
void language::sanity_check() const {
  /* if (m_start_symbol_id == symbol_unique_map::npos) throw no_start_symbol();
   */
  /* if (m_eof_symbol_id == symbol_unique_map::npos) throw no_eof_symbol(); */
  /* if (m_nonterminal2rule[m_start_symbol_id].size() != 1) */
  /*   throw invalid_principle_rule(); */
}
}  // namespace experiment
