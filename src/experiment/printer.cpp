#include "printer.hpp"
#include <algorithm>
#include <boost/format.hpp>

namespace experiment {

void printer::operator()(const_language lang) { m_os << lang; }

////////////////// grammar + subject /////////////////////////
void printer::operator()(const_grammar g, const_item item) {
  
}
void printer::operator()(const_grammar g, const_rule rule) {}
void printer::operator()(const_grammar g, const_itemset itemset) {}
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
  m_os << boost::format("%1% -> ") % rule.first;  // rule.head
  std::for_each(                                  // rule.body
      std::begin(rule.second), std::end(rule.second),
      [&](symbol_unique_id id) { m_os << id << ", "; });
  m_os << '\n';
}

}  // namespace experiment
