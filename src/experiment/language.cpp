#include "language.hpp"
#include "indent.hpp"
#include <algorithm>
#include <boost/format.hpp>

namespace experiment {
const language::symbol_unique_id language::start = 0, language::eof = 1,
                                 language::epsilon = 2;

language::language() : m_optional_count(0), m_list_count(0) , m_principle_rule(rule_unique_map::npos)
  {
  register_symbol("__start__", symbol_property::nonterminal); /* 0 */
  register_symbol("__eof__", symbol_property::terminal); /* 1 */
  register_symbol("__epsilon__", symbol_property::terminal); /* 2 */
}
void
language::notify() {
  resolve_symbols();
}

language::symbol_unique_id language::register_symbol(const char* str,
                                                     symbol_property property) {
  return m_symbol_map[symbol(str, property)];
}

language::symbol_unique_id language::register_symbol(std::string const& str,
                                                     symbol_property property) {
  return m_symbol_map[symbol(str, property)];
}

language::rule_unique_id language::register_rule(rule_type const& rule) {
  auto ruleid = m_rule_map[rule];
  m_nonterminal2rule[rule.head()].push_back(ruleid);
  return ruleid;
}

language::rule_vector const&
language::rule_for(symbol_unique_id id) const {
  return m_nonterminal2rule.at(id);
}

language::symbol_unique_id language::make_list() {
  auto fmt = boost::format("__list__%1%") % m_list_count;
  m_list_count++;
  return register_symbol(fmt.str(), symbol_property::nonterminal);
}
language::symbol_unique_id language::make_optional() {
  auto fmt = boost::format("__optional__%1%") % m_optional_count;
  ++m_optional_count;
  return register_symbol(fmt.str(), symbol_property::optional);
}

rule_tree language::operator[](const char* str) {
  return rule_tree(register_symbol(str, symbol_property::nonterminal), *this);
}

bool language::all_optional(rule_unique_id id) const {
  auto body = rule(id).body();
  return std::all_of(body.cbegin(),body.cend(),[this](symbol_unique_id id) {
      return get_symbol(id).optional();
  });
}

bool language::any_optional(rule_vector const& rules) const {
  return std::any_of(rules.cbegin(), rules.cend(),[this](rule_unique_id id) {
      return all_optional(id);
  });
}
language::size_type language::resolve_nullable() {
  int prev_nullable_count = 0;
  int next_nullable_count = 0;
  do {
    prev_nullable_count = next_nullable_count;
    next_nullable_count = 0;
    for (auto pack : m_nonterminal2rule) {
      auto symbolid = pack.first;
      if (get_symbol(symbolid).optional()) {
        next_nullable_count++;
      } else if (any_optional(pack.second)) {
        m_symbol_map[symbolid].set(symbol_property::optional);
        next_nullable_count++;
      }
    }
  } while (prev_nullable_count != next_nullable_count);
  return prev_nullable_count;
}

// eliminate unknown property
void language::resolve_symbols() {
  // figure out
  // 1. unique top symbol like 'translation_unit'
  // 2. which symbol is optional/nullable
  // 3. which symbol refers to no one so become terminal
  resolve_nullable(); 
  symbol_vector top_symbols;
  vertex_iterator vbegin, vend;
  boost::tie(vbegin, vend) = boost::vertices(m_symbol_graph);
  for (; vbegin != vend; ++vbegin) {
    if (boost::out_degree(*vbegin, m_symbol_graph) == 0) {
      // referring no one == terminal
      m_symbol_map[*vbegin].set(symbol_property::terminal);
    } else if (boost::in_degree(*vbegin, m_symbol_graph) == 0) {
        // referred to by no one == top nonterminal
        top_symbols.push_back(*vbegin);
    } 
  }
  resolve_rules(top_symbols);
  sanity_check();
}
void language::resolve_rules(symbol_vector const& top_symbols) {
  if (top_symbols.size()==1) {
    // only one top
    rule_type rule(
        language::start /* head */,
        { top_symbols[0], language::eof /* body */ }
    );
    m_principle_rule = register_rule(rule);
  } else {
    throw "invalid number of top rules";
  }
}
void language::sanity_check() const {
  if (std::any_of(m_symbol_map.vbegin(),m_symbol_map.vend(),
        [](symbol const& sym) {
        return sym.unknown();
        })) {
    throw "some symbols remain unknown after resolution";
  }
}

std::ostream& operator<<(std::ostream& os, const language& lang) {
  unsigned level = 0;
  os << boost::format("language \"%1%\"\n\n") % lang.m_name;
  for (auto iter=lang.rules();
      iter.first!=iter.second;
      ++iter.first) {
    rule_type const& rule=*(iter.first);
    indent4 i_(os, level);
    os << boost::format("\"%1%\" := ") % lang.get_symbol(rule.head());
    for (auto& symbol : rule.body()) {
      os << boost::format("\"%1%\"") % lang.get_symbol(symbol) << ' ';
    }
  }
return os;
}
}  // namespace experiment
