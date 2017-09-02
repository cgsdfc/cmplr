#include "grammar_base.hpp"

namespace experiment {
using std::cout;
using std::endl;
grammar_base::grammar_base(const language& lang)
    : m_start_symbol_id(symbol_unique_map::npos),
      m_eof_symbol_id(symbol_unique_map::npos),
      m_nonterminal2rule(lang.num_nonterminals()) {
  m_rule_map.reserve(lang.num_rules());
  resolve_symbols(lang);
  resolve_rules(lang);
  sanity_check();
}

// populate m_symbol_map
// eliminate unknown property
void grammar_base::resolve_symbols(const language& lang) {
  for (auto bundle : lang) {
    symbol head = bundle.first;
    if (head.terminal()) throw terminal_as_head();
    if (bundle.second.size() == 0) head.set(symbol_property::optional);
    auto id = m_symbol_map[head];
    if (head.start()) {
      m_start_symbol_id = id;
    }
  }  // make nontermials have the
  // lower part of ids.
  for (auto bundle : lang) {
    for (auto& body : bundle.second) {
      for (auto& part : body) {
        symbol mutable_part(part);
        if (mutable_part.unknown()) mutable_part.set(symbol_property::terminal);
        auto id = m_symbol_map[mutable_part];
        if (mutable_part.eof()) {
          m_eof_symbol_id = id;
        }
      }
    }
  }
}
// populate m_nonterminal2rule , m_rule_map
//
void grammar_base::resolve_rules(const language& lang) {
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
}  // namespace experiment
