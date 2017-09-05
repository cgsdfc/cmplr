#include "grammar.hpp"
#include <algorithm>
#include <bitset>
#include <queue>

namespace experiment {
void grammar::build_itemset() {
  std::queue<itemset_unique_id> queue;
  std::unordered_set<itemset_unique_id> visited;
  itemset_type initial;
  item_type first_item = make_item(0, m_lang.principle_rule());
  initial.push_back(m_item_map[first_item]);
  do_closure(initial);
  queue.push(m_itemset_map[initial]);

  while (!queue.empty()) {
    auto current_id = queue.front();
    auto current = m_itemset_map[current_id];
    queue.pop();
    symbol2itemset_map symbol2itemset;
    for (auto item_id : current) {
      const item_type& item = m_item_map[item_id];
      if (item_dot_reach_end(item)) continue;
      auto symbol = symbol_at_dot(item);
      auto new_item = make_item(item.dot() + 1, item.rule());
      symbol2itemset[symbol].push_back(m_item_map[new_item]);
    }
    for (auto& bundle : symbol2itemset) {
      itemset_type& next_itemset = bundle.second;
      do_closure(next_itemset);
      auto next_id = m_itemset_map[next_itemset];
      auto symbol = bundle.first;
      add_edge(current_id, next_id, symbol);
      if (visited.find(next_id) == visited.end()) {
        /* itemset is white */
        queue.push(next_id);
      }
    }
    visited.insert(current_id);
  }
}

void grammar::do_closure(itemset_type& itemset) {
  // close the itemset by adding item
  std::vector<bool> accounted(m_lang.num_symbols(), false);
  // must resize to the max possible items
  // to ensure iterator wont get invalidate
  // during the add-and-traversal loop
  itemset.reserve(m_lang.num_rules()); /* if use size(), reserve is not necessary */
  /* for (auto item_id : itemset) { */
  /* cannot use range_loop here */
  for (int i=0;i<itemset.size();++i) {
    auto item_id = itemset[i];
    const item_type& item = m_item_map[item_id];
    if (item_dot_reach_end(item)) continue;
    auto symbol = symbol_at_dot(item);
    if (m_lang.get_symbol(symbol).terminal()) continue;
    if (accounted[symbol]) continue;
    for (auto ruleid : m_lang.rule_for(symbol)) {
      auto new_item = make_item(0, ruleid);
      itemset.push_back(m_item_map[new_item]);
    }
    accounted[symbol]=true;
  }
  std::sort(itemset.begin(), itemset.end());
  itemset.shrink_to_fit();
}
}  // namespace experiment
