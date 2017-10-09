#ifndef EXPERIMENT_OSTREAM_HPP
#define EXPERIMENT_OSTREAM_HPP 1
#include "grammar.hpp"
#include "language.hpp"
// impl complex print logic
namespace experiment {
class printer;
extern printer print;

class printer {
 private:
  typedef std::vector<std::string> string_vector_type;
  typedef std::ostream ostream_type;
  typedef ostream_type& ostream_reference;
  typedef grammar::symbol_unique_id symbol_unique_id;
  typedef grammar::item_unique_id item_unique_id;
  typedef grammar::rule_unique_id rule_unique_id;
  ostream_reference m_os;

 public:
  typedef const grammar& const_grammar;
  typedef const language& const_language;
  // subject
  typedef grammar::rule_type const& const_rule;
  typedef grammar::item_type const& const_item;
  typedef grammar::itemset_type const& const_itemset;
  // iterator
  typedef grammar::rule_iterator rule_iterator;
  typedef grammar::item_iterator item_iterator;
  typedef grammar::itemset_iterator itemset_iterator;
  typedef grammar::nonterminal2rule_map const& const_nonterminal2rule_map;
  typedef grammar::enum_item_iterator enum_item_iterator;
  typedef grammar::enum_itemset_iterator enum_itemset_iterator;

 public:  // tags
  struct nonterminal2rule_map_tag {};
  struct core_item_diagram_tag {};
  struct allow_tailing_newline_tag {};
  struct disallow_tailing_newline_tag {};

 private:
  void flush_to_stream(string_vector_type const&, bool newline,
                       const char* sep = " ");
  template <class InputIter, class UnaryFunc>
  void make_string_vector(InputIter first, InputIter last,
                          string_vector_type& toprint, UnaryFunc tostring) {
    std::transform(first, last, std::back_inserter(toprint), tostring);
  }

 public:
  printer(ostream_reference os) : m_os(os) {}

  void operator()(const_grammar);
  void operator()(const_language);
  // iterator
  void operator()(rule_iterator);
  void operator()(item_iterator);
  void operator()(itemset_iterator);
  // subject
  void operator()(const_rule);
  void operator()(const_item);
  void operator()(const_itemset);
  // grammar + iterator
  void operator()(const_grammar, rule_iterator);
  void operator()(const_grammar, item_iterator);
  void operator()(const_grammar, itemset_iterator);
  // grammar + subject
  void operator()(const_grammar, const_rule, bool newline = true);
  void operator()(const_grammar, const_item, bool newline = true);
  void operator()(const_grammar, const_itemset, bool newline = true);
  void operator()(const_grammar, nonterminal2rule_map_tag);
  void operator()(const_grammar, core_item_diagram_tag);
  // grammar + enum_subject
  void operator()(const_grammar, enum_item_iterator);
  void operator()(const_grammar, enum_itemset_iterator);

  // sugar printer
  template <class... Args>
  void operator()(Args const&...);
  template <class T, class... Args>
  void operator()(T const& t, Args const&... args) {
    m_os << t << " ";
    operator()(args...);
  }
  void operator()(
      allow_tailing_newline_tag t = allow_tailing_newline_tag() /* default */) {
    m_os << "\n";
  }
  void operator()(disallow_tailing_newline_tag) {}

  // print with unique_id
  template <class Item, class UniqueId>
  void operator()(const_grammar g, const std::pair<Item, UniqueId>& item) {
    // this function requires echo operator() (g, item.first)
    // does not print a tailing newline
    // and itself omits the tailing newline
    operator()(g, item.first, false);
    operator()("\t", item.second, allow_tailing_newline_tag());
  }
};
}  // namespace experiment

#endif  // EXPERIMENT_OSTREAM_HPP
