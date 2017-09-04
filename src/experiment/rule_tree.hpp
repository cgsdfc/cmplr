#ifndef EXPERIMENT_RULE_TREE_HPP
#define EXPERIMENT_RULE_TREE_HPP 1
#include <memory>
#include <vector>
#include <list>
#include "language.hpp"
#include "detail/rule_tree.hpp"

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
  typedef detail::language_base::symbol_unique_id symbol_unique_id;
  typedef std::list<symbol_unique_id> body_id_type;
  // for fast insertion at front
  typedef typename detail::symbol_node::base_type body_str_type;
  body_id_type m_body;

 public:
  rule_node(language& lang) : m_lang(lang) {}
  template <class... Args>
  void parse(Args&&... args); /* no definition */
  template <class T, class... Args>
    void parse(T&& t, Args&&... args);
  void str2id(body_id_type& ids, body_str_type const& strs) const;
  void parse(detail::optional_node&& optional);
  void parse(detail::list_node&& list);
  void parse();
};
class rule_tree {
 private:
  friend class language;
  const symbol_unique_id m_head;
  language& m_lang;
  symbol_unique_id register_symbol(const char *);
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
#endif  // EXPERIMENT_RULE_TREE_HPP
