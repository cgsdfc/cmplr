#include "language.hpp"
#include <algorithm>
#include <boost/format.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include "indent.hpp"

namespace experiment {
const language::symbol_unique_id language::start = 0, language::eof = 1,
                                 language::epsilon = 2;

language::language()
    : m_optional_count(0),
      m_list_count(0),
      m_principle_rule(rule_unique_map::npos) {
  register_symbol("__start__", symbol_property::nonterminal); /* 0 */
  register_symbol("__eof__", symbol_property::terminal);      /* 1 */
  register_symbol("__epsilon__", symbol_property::terminal);  /* 2 */
}

language::symbol_unique_id language::register_symbol(symbol const& sym) {
  return m_symbol_map[sym];
}
language::symbol_unique_id language::register_symbol(const char* str,
                                                     symbol_property property) {
  return register_symbol(std::string(str), property);
}
language::symbol_unique_id language::register_symbol(std::string const& str,
                                                     symbol_property property) {
  auto id = m_symbol_map[symbol(str, property)];
  m_symbol_map[id].set(property);
  return id;
}

language::rule_unique_id language::register_rule(rule_type const& rule) {
  auto ruleid = m_rule_map[rule];
  m_nonterminal2rule[rule.head()].push_back(ruleid);
  return ruleid;
}

language::rule_vector const& language::rule_for(symbol_unique_id id) const {
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

rule_tree language::operator[](symbol const& modified) {
  return rule_tree(register_symbol(modified), *this);
}

bool language::all_optional(rule_unique_id id) const {
  auto body = rule(id).body();
  return std::all_of(body.cbegin(), body.cend(), [this](symbol_unique_id id) {
    return get_symbol(id).optional();
  });
}

bool language::any_optional(rule_vector const& rules) const {
  return std::any_of(rules.cbegin(), rules.cend(),
                     [this](rule_unique_id id) { return all_optional(id); });
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
void language::notify() {
  resolve_nullable();
  symbol_vector top_symbols;
  rule_type principle_rule;
  principle_rule.head() = language::start;
  for (auto symbol : m_symbol_map.items()) {
    if (symbol.first.toplevel()) {
      top_symbols.push_back(symbol.second);
    } else if (symbol.first.unknown()) {
      symbol.first.set(symbol_property::terminal);
    }
  }
  for (auto top : top_symbols) {
    principle_rule.body().push_back(top);
    principle_rule.body().push_back(language::eof);
    register_rule(principle_rule);
    principle_rule.body().clear();
  }
}
void language::print(ostream_reference os, symbol const& sym,
                     bool show_property) const {
  if (!show_property) {
    os << sym;  // plain string
    return;
  }
  switch (sym.property()) {
    case symbol_property::optional:
      os << "opt";
      break;
    case symbol_property::terminal:
      os << "t";
      break;
    case symbol_property::nonterminal:
      os << "n";
      break;
    case symbol_property::unknown:
      os << "u";
      break;
  }
  os << sym;
}

void language::print(ostream_reference os, rule_type const& rule) const {
  // print in head := N N N format
  print(os, get_symbol(rule.head()));  // not show_property;
  os << " := ";
  auto id2symbol = [this](symbol_unique_id id) { return get_symbol(id); };
  auto begin = boost::make_transform_iterator(rule.body().begin(), id2symbol);
  auto end = boost::make_transform_iterator(rule.body().end(), id2symbol);
  std::copy(begin, end, std::ostream_iterator<symbol>(os, " "));
}

void language::print(ostream_reference os) const {
  // this print show the lang in a format closest to
  // original input, that is,
  // N
  //  A B C
  //  C E F
  unsigned level = 0;
  os << boost::format("\nlanguage \"%1%\"\n\n") % m_name;
  for (auto pair : m_nonterminal2rule) {
    indent2 i_(os, level);
    print(os, get_symbol(pair.first), true);
    os << "\n";
    for (auto rules : pair.second) {
      indent4 i__(os, level);
      auto body = rule(rules).body();
      for (auto symbol : body) {
        print(os, get_symbol(symbol), true);
        os << " ";
      }
    }
  }
}

std::ostream& operator<<(std::ostream& os, const language& lang) {
  lang.print(os);
  return os;
}
}  // namespace experiment
