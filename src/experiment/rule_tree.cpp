#include "rule_tree.hpp"

namespace experiment {
rule_adder::symbol_unique_id rule_adder::register_symbol(const char* str) {
  return m_lang.register_symbol(str);
}
rule_adder::symbol_unique_id rule_adder::register_rule(rule_type const& rule) {
  // sololy call m_lang.register_rule;
  return m_lang.register_rule(rule);
}

rule_adder::symbol_unique_id rule_adder::register_rule(
    symbol_unique_id head, symbol_id_vector const& body) {
  rule_type rule;
  rule.head() = head;
  rule.body() = body;
  auto ruleid = register_rule(std::move(rule));
  for (auto id:body) {
    m_lang.add_edge(head, id, ruleid);
  }
}
void rule_node::xparse(const char* str) {
  m_body.push_back(register_symbol(str));
}

void rule_node::xparse(optional_node const& optional) {
  auto head = m_lang.make_optional();
  symbol_id_vector body;
  string2id(optional.begin(), optional.end(), std::back_inserter(body));
  // register a head -> body;
  register_rule(head, body.begin(), body.end());
  // register a head -> __epsilon__
  body.clear();
  body.push_back(language::epsilon);
  register_rule(head, body.begin(), body.end());
  m_body.push_back(head);
  // part of original rule
}
void rule_node::xparse(list_node const& list) {
  auto head = m_lang.make_list();
  symbol_id_list body;
  string2id(list.begin(), list.end(), std::back_inserter(body));
  // register a head -> body
  register_rule(head, body.begin(), body.end());
  if (list.m_sep) {
    auto sep = register_symbol(list.m_sep);
    body.push_front(sep);
    // now head -> sep body
  }
  body.push_front(head);
  // now head -> head (sep) body
  register_rule(head, body.begin(), body.end());
  m_body.push_back(head);
  // part of the original rule
}
void rule_node::parse() {
  if (!m_body.size()) {
    // call without arguments
    m_body.push_back(language::epsilon);
  }
}
rule_tree::~rule_tree() {
  for (auto const& node : m_nodes) {
    auto body = node->body();
    auto ruleid=register_rule(m_head, body.begin(), body.end());
    for (auto symid : body) {
      m_lang.add_edge(m_head, symid,ruleid);
    }
  }
}
}  // namespace experiment
