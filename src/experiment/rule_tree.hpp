#ifndef EXPERIMENT_RULE_TREE_HPP
#define EXPERIMENT_RULE_TREE_HPP 1
#include <list>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include "symbol.hpp"
#include "rule_type.hpp"
#include "language.hpp"

namespace experiment {
class language;
struct symbol_node {
  // no way to access lang as a free function
  // stores the symbol and delay lookup
  typedef std::vector<const char*> base_type;
  typedef typename base_type::size_type size_type;
  typedef typename base_type::reference reference;
  typedef typename base_type::const_reference const_reference;
  typedef typename base_type::const_iterator const_iterator;
  base_type m_symbols;
  size_type size() const { return m_symbols.size(); }
  reference operator[](size_type pos) { m_symbols[pos]; }
  const_reference operator[] (size_type pos) const { m_symbols[pos]; }
  const_iterator begin() const { return m_symbols.begin(); }
  const_iterator end() const {return m_symbols.end();}
};

struct optional_node : public symbol_node {
  template <class... Args>
  void parse(Args&&... args);
  template <class T, class... Args>
  void parse(T&& t, Args&&... args) {
    m_symbols.emplace_back(t);
    parse(args...);
  }
  void parse() {
    if (!size()) {
      // empty optional() or list() ?
    }
  }
};
struct separator {
  const char* m_str;
  operator bool() const { return m_str != nullptr; }
  separator(const char* str) : m_str(str) {}
  separator() : m_str(nullptr) {}
  operator const char*() const { return m_str; }
};

struct list_node : public symbol_node {
  separator m_sep;
  template <class T, class... Args>
  void parse(T&& t, Args&&... args) {
    m_symbols.emplace_back(t);
    parse(args...);
  }
  void parse(separator&& sep) { m_sep = sep; }
  void parse() {
    if (!size()) {
      // empty optional() or list() ?
    }
  }
};

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
class rule_adder {
  public:
    typedef language& lang_reference;
    typedef symbol_traits::unique_id symbol_unique_id;
    typedef rule_traits::unique_id rule_unique_id;
    typedef std::list<symbol_unique_id> symbol_id_list;
    typedef std::vector<symbol_unique_id> symbol_id_vector;
    typedef std::vector<const char *>  symbol_str_vector;
  protected:
    lang_reference m_lang;
  protected:
    rule_adder(lang_reference lang):m_lang(lang) {}
    rule_unique_id register_rule(rule_type const& rule);
    symbol_unique_id register_symbol(const char*);
    rule_unique_id register_rule(symbol_unique_id, symbol_id_list);
    template<class InputIter>
      rule_unique_id register_rule(symbol_unique_id head, InputIter first, InputIter last) {
        rule_type rule;
        rule.head() = head;
        std::copy(first,last, std::back_inserter(rule.body()));
        return register_rule(std::move(rule));
      }
    rule_unique_id register_rule(symbol_unique_id head, symbol_id_vector const& body);
};
class rule_node :public rule_adder {
  private:
    typedef symbol_id_list body_type;
    body_type m_body;
    typedef rule_adder base_t;
  public:
    rule_node(lang_reference lang) : base_t(lang) {}
    template <class... Args>
      void parse(Args&&... args); /* no definition */
    template <class... Args>
      void parse(const char *t, Args&&... args) {
        /* auto id = m_lang.register_symbol(t); */
        auto id = register_symbol(t);
        m_body.push_back(id);
        parse(args...);
      }
    template<class... Args>
      void parse(list_node&& node, Args&&... args) {
        parse(node);
        parse(args...);
      }
    template<class... Args>
      void parse(optional_node&& node, Args&&... args) {
        parse(node);
        parse(args...);
      }
    template<class InputIter, class OutputIter>
      void string2id(InputIter first, InputIter last, OutputIter out) {
        std::transform(first, last, out,
            std::bind(&rule_adder::register_symbol, *this, std::placeholders::_1)
        );
      }
    body_type const& body() const { return m_body; }
    void parse(optional_node&& optional);
    void parse(list_node&& list);
    void parse();
};
class rule_tree:public rule_adder{
  private:
    friend class language;
    typedef rule_adder base_t;
    typedef std::vector<std::shared_ptr<rule_node>>
      rule_node_vector;
    typedef symbol_unique_id head_type;
  public:
    const head_type m_head;
    rule_node_vector m_nodes;
    rule_tree(head_type const& head, lang_reference lang)
      : base_t(lang), m_head(head) {}

  public:
    template <class... Args>
      rule_tree& operator()(const Args&... args) {
        m_nodes.emplace_back(new rule_node(m_lang));
        m_nodes.back()->parse(args...);
        return *this;
      }
    // when rule_tree is destructed,
    // traverse all nodes and add them to lang
    ~rule_tree();
};
}  // namespace experiment
#endif  // EXPERIMENT_RULE_TREE_HPP
