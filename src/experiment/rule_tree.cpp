#include "rule_tree.hpp"
namespace experiment {
void rule_node::str2id(body_id_type& ids, body_str_type const& strs) const {
  for (int i = 0; i < size; ++i) {
    ids.push_back(m_lang.m_symbol_map[strs[i]]);
  }
}

void rule_node::parse(detail::optional_node&& optional) {
  auto head = m_lang.make_optional();
  body_id_type body;
  str2id(body, optional);
  // register a head -> body;
  m_lang.register_rule(rule_type(head, body));
  // register a head -> __epsilon__
  m_lang.register_rule(rule_type(head, m_lang.epsilon()));
  m_body.push_back(head);
  // part of original rule
}
void rule_node::parse(detail::list_node&& list) {
  auto head = m_lang.make_list();
  body_id_type body;
  str2id(body, list);
  // register a head -> body
  m_lang.register_rule(rule_type(head, body));
  if (list.m_sep) {
    auto sep = register_symbol(list.m_sep);
    body.push_front(sep)
    // now head -> sep body
  }
  body.push_front(head);
  // now head -> head (sep) body
  m_lang.register_rule(rule_type(head, body));
}
void rule_node::parse() {
  if (!m_body.size()) {
    // call without arguments
    m_body.push_back(m_lang.epsilon());
  }
}
rule_tree::~rule_tree() {
  for (auto rule : m_rules) {
    rule_type rule(m_head, rule->m_body);
    auto id = m_lang.m_rule_map[rule];
    m_lang.m_nonterminal2rule[m_head].push_back(id);
  }
}
}  // namespace experiment
