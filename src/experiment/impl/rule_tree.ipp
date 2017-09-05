#ifndef EXPERIMENT_IMPL_RULE_TREE_IPP
#define EXPERIMENT_IMPL_RULE_TREE_IPP 1
namespace experiment {
template <class... Args>
optional_node optional(Args&&... args) {
  optional_node node;
  node.parse(args...);
  return node;
}

template <class... Args>
optional_node list(Args&&... args) {
  list_node node;
  node.parse(args...);
  return node;
}
inline separator sep(const char* str) { return separator(str); }
template <class T, class... Args>
void rule_node::parse(T&& t, Args&&... args) {
  /* auto id = m_lang.register_symbol(t); */
  auto id = register_symbol(t);
  m_body.push_back(id);
  parse(args...);
}

template <class... Args>
rule_tree& rule_tree::operator()(Args&&... args) {
  auto prule_node = new rule_node(m_lang);
  prule_node->parse(args...);
  m_rules.emplace_back(prule_node);
  return *this;
}
}  // namespace experiment
#endif  // EXPERIMENT_IMPL_RULE_TREE_IPP
