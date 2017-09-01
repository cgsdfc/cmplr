#include "grammar.hpp"
#include <bitset>
#include <algorithm>
#include <queue>

namespace experiment {

  void grammar::resolve_symbols(const language& lang) {

  }
  void grammar::resolve_rules(const language& lang) {

  }
  void grammar::accept(const language& lang) {

  }

void grammar::build_itemset() {
  std::queue<itemset_unique_id> queue;
  std::unordered_set<itemset_unique_id> visited;
  itemset_type initial;
  item_type first_item=make_item(0,rule_unique_id(0));
  initial.push_back(m_item_map[first_item]);
  do_closure(initial);
  queue.push(m_itemset_map[initial]);

  while (!queue.empty()) {
    auto current_id = queue.front();
    auto current = m_itemset_map[current_id];
    queue.pop();
    symbol2itemset_map symbol2itemset;
    for (auto item_id: current) {
      const item_type& item = m_item_map[item_id];
      if (item_dot_reach_end(item))
        continue;
      auto symbol = symbol_at_dot(item);
      auto new_item = make_item(item.first+1,item.second);
      symbol2itemset[symbol].push_back(m_item_map[new_item]);
    }
    for (auto& bundle:symbol2itemset) {
      itemset_type& next_itemset = bundle.second;
      do_closure(next_itemset);
      auto next_id = m_itemset_map[next_itemset];
      auto symbol = bundle.first;
      add_edge(current_id, next_id, symbol);
      if (visited.find(next_id)!=visited.end()) {
        queue.push(next_id);
      }
    }
    visited.insert(current_id);
  }  
}

void
grammar::do_closure(itemset_type& itemset) {
  // close the itemset by adding the item
  // whose dot pinned at the first symbol on the left
  // hand size and whose rule having 
  std::vector<bool> accounted(m_symbol_map.size(), false);
  for (auto item_id: itemset) {
    const item_type& item = m_item_map[item_id];
    if (item_dot_reach_end(item))
      continue;
    auto nontermial = symbol_at_dot(item);
    if (accounted[nontermial])
      continue;
    for (auto ruleid: m_nonterminal2rule[nontermial]) {
      auto new_item = make_item(0, ruleid);
      itemset.push_back(m_item_map[new_item]);
    }
  }
  std::sort(itemset.begin(), itemset.end());
}

} // namespace experiment

