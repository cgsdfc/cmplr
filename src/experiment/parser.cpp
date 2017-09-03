#include "parser.hpp"

namespace experiment {
parser::parser(const_grammar grammar)
    : m_grammar(grammar),
      m_state_table(grammar.num_symbols(), grammar.num_itemsets()) {
  build_state_diagram();
}

void parser::build_state_diagram() {
  auto eof_id = m_grammar.eof();
  // 1, set all the edge with a terminal on it to shift
  for (auto eit = m_grammar.edges(); eit.first != eit.second; ++eit.first) {
    auto E = *(eit.first);
    auto id = m_grammar.symbol_on_edge(E);
    auto source = m_grammar.source(E);
    auto target = m_grammar.target(E);
    entry cell;
    if (m_grammar.get_symbol(id).terminal()) {
      cell = entry(action::shift, target);
      // shift to state E.target
    } else {
      cell = entry(action::goto_, target);
      // goto to state E.target
    }
    m_state_table[source][id] = cell;
    // 2, itemset with an item having the form A -> w ^
    // will fill the whole row with reduce to rule A -> w
    // when the cell to be filled is already filled with sth
    // other than unknown, a conflict is detected and the language
    // is not LR(0)
    for (auto vit = m_grammar.vertices(); vit.first != vit.second;
         ++vit.first) {
      auto V = *(vit.first);
      auto itemset = m_grammar.itemset_on_vertex(V);
      for (auto item_id : itemset) {
        auto item = m_grammar.item(item_id);
        if (m_grammar.symbol_at_dot(item) == eof_id) {
          m_state_table[V][eof_id] = entry(action::accept, 0);
        }
        if (m_grammar.item_dot_reach_end(item)) {
          for (auto cell : m_state_table[V]) {
            if (cell.unknown()) {
              cell = entry(action::reduce, item.second);
              // are cell reference here?
              // reduce to Rule item.second
            } else if (cell.reduce()) {
              throw reduce_reduce_conflict();
            } else {
              throw shift_reduce_conflict();
            }
          }
        }
      }
    }
  }
  // 3. set the still unknown cell to error
  for (auto rows : m_state_table) {
    for (auto cell : rows) {
      if (cell.unknown()) cell = entry(action::error, 0);
    }
  }
}
parser::entry const& parser::lookup_table(core_state_id state,
                                          symbol_unique_id id) const {
  return m_state_table[state][id];
}

void parser::reduce(rule_unique_id id) {
  // may do ast action
  auto rule = m_grammar.rule(id);
  auto head = rule.first;
  auto len = rule.second.size();
  for (int i = 0; i < len; ++i) {
    m_symbol_stack.pop();
  }
  m_symbol_stack.push(head);
  m_state_stack.pop();
  auto entry = lookup_table(m_state_stack.top(), head);
  assert(entry.m_action == action::goto_);
  m_state_stack.push(entry.m_info.next);
}

bool parser::initial_state_on_stack() const {
  return m_state_stack.size() == 1 && core_state_id(0) == m_state_stack.top();
}

void parser::parse(symbol_stream first) {
  m_state_stack.push(core_state_id(0));
  symbol_unique_id lookahead_symbol;
  rule_unique_id ruleid;
  lookahead_symbol = *first;
  while (true) {
    auto entry = lookup_table(m_state_stack.top(), lookahead_symbol);
    switch (entry.m_action) {
      case action::shift:
        m_symbol_stack.push(lookahead_symbol);
        m_state_stack.push(entry.m_info.next);
        ++first;
        break;
      case action::reduce:
        reduce(entry.m_info.rule);
        break;
      case action::accept:
        if (initial_state_on_stack()) {
          return;
        }
      case action::error:
        throw syntax_error();
      default:
        throw mystery_error();
    }
  }
}

}  // namespace experiment
