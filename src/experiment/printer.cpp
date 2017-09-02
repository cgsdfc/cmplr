#include "printer.hpp"
#include <algorithm>
#include <boost/format.hpp>

namespace experiment {

void printer::operator()(const_language lang) { m_os << lang; }

////////////////// grammar + subject /////////////////////////
void printer::operator()(const_grammar g, const_item item) {
  
}
void printer::operator()(const_grammar g, const_rule rule) {

}
void printer::operator()(const_grammar g, const_itemset itemset) {

}
///////////////// grammar + iterator ////////////////////////
void printer::operator()(const_grammar g, rule_iterator it) {
  operator()(g, *it);
}
void printer::operator()(const_grammar g, item_iterator it) {
  operator() (g, *it);
}
void printer::operator()(const_grammar g, itemset_iterator it) {
  operator() (g, *it);
}
/////////////////// iterator //////////////////////////////////
void printer::operator()(item_iterator it) {
  operator() (*it);
}
void printer::operator()(rule_iterator it) {
  m_os << boost::format("%1% -> ") % it->first;  // rule.head
  std::for_each(                                 // rule.body
      std::begin(it->second), std::end(it->second),
      [&](symbol_unique_id id) { m_os << id << ", "; });
  m_os << '\n';
}
void printer::operator()(itemset_iterator it) {

}
//////////////////// subject //////////////////////////////////
void printer::operator()(const_item item) {
  m_os << boost::format("item(ruleid=%1%, dot=%2%)"), item.second, item.first;
}
void printer::operator()(const_itemset itemset) {

}
void printer::operator()(const_rule rule) {

}

}  // namespace experiment
