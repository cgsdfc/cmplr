#ifndef EXPERIMENT_RULE_TREE_HPP
#define EXPERIMENT_RULE_TREE_HPP 1
#include <memory>
#include <vector>
#include "detail/rule_tree.hpp"
#include "language.hpp"

namespace experiment {
template <class... Args>
detail::optional_node optional(Args&&... args);
template <class... Args>
detail::list_node list(Args&&... args);
detail::separator sep(const char* str);
class language;
struct rule_node {
 private:
  language& m_lang;
  typedef std::list<symbol_unique_id> body_id_type;
  // for fast insertion at front
  typedef typename detail::symbol_node::base_type body_str_type;
  body_id_type m_body;

 public:
  rule_node(language& lang) : m_lang(lang) {}
  template <class... Args>
  void parse(Args&&... args); /* no definition */
  template <class T, class... Args>
  void parse(T&&... t, Args&&... args);
  void str2id(body_id_type& ids, body_str_type const& strs) const;
  void parse(optional_node&& optional);
  void parse(list_node&& list);
  void parse();
};

class rule_tree {
 private:
  friend class language;
  typedef language::symbol_unique_id symbol_unique_id;
  typedef language::rule_unique_id rule_unique_id;
  const symbol_unique_id m_head;
  language& m_lang;
  std::vector<std::unique_ptr<rule_node>> m_rules;
  rule_tree(symbol_unique_id head, language& lang)
      : m_head(head), m_lang(lang) {}

 public:
  template <class... Args>
  rule_tree& operator()(Args&&... args);
  // when rule_tree is destructed,
  // traverse all nodes and add them to lang
  ~rule_tree();
};
}  // namespace experiment
#include "impl/rule_tree.ipp"
#endif  // EXPERIMENT_RULE_TREE_HPP
