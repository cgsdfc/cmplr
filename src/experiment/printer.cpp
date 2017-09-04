#include "printer.hpp"
#include <algorithm>
#include <boost/format.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <iterator>
#define PRINTER_GET_SYMBOL(g) [&](symbol_unique_id id){ return g.get_symbol(id); }
namespace experiment {
printer print(std::cout);
void printer::operator()(const_language lang) { m_os << lang; }
void printer::flush_to_stream(string_vector_type const& toprint,bool newline, const char *sep) {
  std::copy(toprint.begin(), toprint.end(),
      std::ostream_iterator<std::string> (m_os, sep));
  if (newline)
    m_os << '\n';
}
////////////////// grammar + subject /////////////////////////
void printer::operator()(const_grammar g, const_item item, bool newline) {
  auto rule = g.rule(item);
  auto head = g.get_symbol(rule.head());
  auto body = rule.body();
  m_os << boost::format("%1% := ") % head;
  std::vector<std::string> toprint;
  make_string_vector(body.begin(), body.end(), toprint,
      PRINTER_GET_SYMBOL(g));
  toprint.insert(toprint.begin() + item.dot(), "^");
  flush_to_stream(toprint, newline);
}
void printer::operator()(const_grammar g, const_rule rule, bool newline) {
  string_vector_type toprint;
  auto body = rule.body();
  toprint.reserve(body.size()+2);
  toprint.emplace_back(g.get_symbol(rule.head()));
  toprint.emplace_back(":=");
  make_string_vector(body.begin(), body.end(), toprint,
      PRINTER_GET_SYMBOL(g));
  flush_to_stream(toprint, newline);
}
void printer::operator()(const_grammar g, const_itemset itemset, bool newline) {
  // itemset is just a list of ids
  m_os << "itemset\n";
  std::for_each(itemset.begin(), itemset.end(),
      [&](item_unique_id id) {
      operator() (g, g.item(id));
      });
  if (newline) m_os << "\n";
}
void printer::operator() (const_grammar g, nonterminal2rule_map_tag) {
  boost::counting_iterator<std::size_t>
    begin(0), end(g.m_nonterminal2rule.size());
  for (; begin!=end; ++begin) {
    m_os << g.get_symbol(*begin) << ":\n";
    for (auto ruleid: g.m_nonterminal2rule[*begin]) {
      m_os << ' ';
      operator() (g, g.rule(ruleid));
    }
    m_os << '\n';
  }
}
void printer::operator() (const_grammar g, core_item_diagram_tag) {
  // print the edges of m_graph;
  auto eit = g.edges();
  for (;eit.first!=eit.second;++eit.first) {
    m_os << *(eit.first) << " ";
    auto id = g.symbol_on_edge(*(eit.first));
    auto sym = g.get_symbol(id);
    operator() (id, sym);
  }
}
///////////////// grammar + iterator ////////////////////////
void printer::operator()(const_grammar g, rule_iterator it) {
  operator()(g, *it);
}
void printer::operator()(const_grammar g, item_iterator it) {
  operator()(g, *it);
}
void printer::operator()(const_grammar g, itemset_iterator it) {
  operator()(g, *it);
}
/////////////////// iterator //////////////////////////////////
void printer::operator()(item_iterator it) { operator()(*it); }
void printer::operator()(rule_iterator it) { operator()(*it); }
void printer::operator()(itemset_iterator it) { operator()(*it); }
//////////////////// subject //////////////////////////////////
void printer::operator()(const_item item) {
  m_os << boost::format("item(ruleid=%1%, dot=%2%)\n"), item.second, item.first;
}
void printer::operator()(const_itemset itemset) {
  m_os << boost::format("itemset(");
  std::for_each(std::begin(itemset), std::end(itemset),
                [&](item_unique_id id) { m_os << id << ", "; });
  m_os << ")\n";
}
void printer::operator()(const_rule rule) {
  m_os << boost::format("%1% := ") % rule.first;  // rule.head
  std::for_each(                                  // rule.body
      std::begin(rule.second), std::end(rule.second),
      [&](symbol_unique_id id) { m_os << id << ", "; });
  m_os << '\n';
}

}  // namespace experiment
