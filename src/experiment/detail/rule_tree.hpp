#ifndef EXPERIMENT_DETAIL_RULE_TREE_HPP
#define EXPERIMENT_DETAIL_RULE_TREE_HPP 1
#include <vector>
namespace experiment {
namespace detail {
// not inherence for m_symbols access
struct symbol_node {
  // no way to access lang as a free function
  // stores the symbol and delay lookup
  typedef std::vector<const char*> base_type;
  typedef typename base_type::size_type size_type;
  typedef typename base_type::reference reference;
  base_type m_symbols;
  size_type size() const {
    return m_symbols.size();
  }
  reference operator[] (size_type pos) {
    m_symbols[pos];
  }
  /* operator base_type&() { return m_symbols; } */
  /* operator base_type const&() const { return m_symbols; } */
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

}  // namespace detail
}  // namespace experiment
#endif  // EXPERIMENT_DETAIL_RULE_TREE_HPP
