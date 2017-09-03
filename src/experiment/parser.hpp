#ifndef EXPERIMENT_PARSER_HPP
#define EXPERIMENT_PARSER_HPP 1
#include <stack>
#include <vector>
#include "grammar.hpp"
#include "symbol_stream.hpp"
#include "matrix.hpp"

namespace experiment {
class parser {
 public:
  typedef grammar const& const_grammar;
  typedef grammar::rule_type const& const_rule;
  typedef grammar::rule_unique_id rule_unique_id;
  typedef grammar::symbol_unique_id symbol_unique_id;
  typedef grammar::itemset_unique_id itemset_unique_id;
  typedef itemset_unique_id core_state_id;
  typedef std::stack<symbol_unique_id> symbol_stack_type;
  typedef std::stack<core_state_id> state_stack_type;

 public:
  enum class action {
    shift,
    reduce,
    accept,
    goto_,
    error,
    unknown,
  };
  struct entry {
    action m_action;
    union info {
      core_state_id next;
      rule_unique_id rule;
      info(core_state_id id) : next(id) {}
      info() : next(0) {}
    } m_info;
    entry() : m_action(action::unknown) {}
    entry(action act, core_state_id id) : m_action(act), m_info(id) {}
    bool shift() const { return m_action == action::shift; }
    bool reduce() const { return m_action == action::reduce; }
    bool accept() const { return m_action == action::accept; }
    bool error() const { return m_action == action::error; }
    bool unknown() const { return m_action == action::unknown; }
  };
  typedef matrix<entry> state_table_type;
  typedef state_table_type::row_type state_table_row;

 private:
  typedef grammar::item_type item_type;
  const_grammar m_grammar;
  symbol_stack_type m_symbol_stack;
  state_stack_type m_state_stack;
  state_table_type m_state_table;

 private:
  void build_state_diagram();
  void reduce(rule_unique_id);
  entry const& lookup_table(core_state_id, symbol_unique_id) const;
  bool initial_state_on_stack() const;

 public:
  parser(const_grammar grammar);
  void parse(symbol_stream);
};

}  // namespace experiment
#endif  // EXPERIMENT_PARSER_HPP
