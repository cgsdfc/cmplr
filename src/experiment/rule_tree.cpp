#include "rule_tree.hpp"
#include "language.hpp"

namespace experiment {
rule_adder::symbol_unique_id 
rule_adder::register_symbol(const char* str) {
  return m_lang.register_symbol(str);
}
rule_adder::symbol_unique_id
rule_adder::register_rule(rule_type const& rule) {
  // sololy call m_lang.register_rule;
  return m_lang.register_rule(rule);
}

rule_adder::symbol_unique_id
rule_adder::register_rule(symbol_unique_id head, symbol_id_vector const& body) {
  rule_type rule;
  rule.head() = head;
  rule.body() = body;
  return register_rule(std::move(rule));
}
void rule_node::parse(optional_node&& optional) {
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
void rule_node::parse(list_node&& list) {
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
  for (auto const& node :m_nodes) {
    auto body = node->body();
    register_rule(m_head, body);
    for (auto symid: body) {
      m_lang.add_edge(m_head, symid);
    }
  }
}
}  // namespace experiment
