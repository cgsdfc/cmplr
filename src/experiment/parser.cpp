#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using std::string;
using std::shared_ptr;
using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::cout;
namespace cmplr {
namespace parser {

class symbol {
 private:
  bool m_is_terminal;
  string m_string;

 public:
  symbol(const char* str) : m_string(str), m_is_terminal(false) {}
  symbol(const char* str, bool is_terminal)
      : m_string(str), m_is_terminal(is_terminal) {}

  bool is_terminal() const { return m_is_terminal; }
  bool is_nontermial() const { return !is_terminal(); }
  operator std::string const&() const { return m_string; }
  struct hash : public std::hash<std::string> {};
  struct equal_to : public std::equal_to<std::string> {};
};

class rule_group {
 private:
  friend class rule_adder;
  void add_rule(vector<symbol>&& list) { m_body.push_back(list); }
  symbol m_head;
  vector<vector<symbol>> m_body;

 public:
  rule_group(const char* head) : m_head(head, false), m_body() {}
};

class terminal_adder;
class rule_adder;

class language {
 private:
  typedef std::unordered_set<symbol, symbol::hash, symbol::equal_to> symbol_set;
  symbol_set m_symbol_set;
  friend terminal_adder;

  const string m_name;
  vector<rule_group> m_rule_groups;

  void add_terminal(symbol const& terminal);

 public:
  language(const char* name) : m_name(name) {}
  terminal_adder terminals();
  rule_adder rule(const char* head);
};

class terminal_adder {
 private:
  friend class language;
  language& m_lang;
  terminal_adder(language& lang) : m_lang(lang) {}

 public:
  terminal_adder& add(const char* str);
};

class rule_adder {
 private:
  friend language;
  rule_group& m_group;
  rule_adder(rule_group& group) : m_group(group) {}

 public:
  rule_adder& operator()(std::initializer_list<const char*> list);
};

void language::add_terminal(symbol const& terminal) {
  m_symbol_set.insert(terminal);
}

terminal_adder language::terminals() { return terminal_adder(*this); }
rule_adder language::rule(const char* head) {
  m_rule_groups.emplace_back(head);
  return rule_adder(m_rule_groups.back());
}

terminal_adder& terminal_adder::add(const char* str) {
  m_lang.add_terminal(symbol(str, false));
  return *this;
}

rule_adder& rule_adder::operator()(std::initializer_list<const char*> list) {
  vector<symbol> to_add;
  for (auto item : list) {
    to_add.emplace_back(item);
  }
  m_group.add_rule(std::move(to_add));
  return *this;
}
}
}

int main(int, char**) {
  cmplr::parser::language lang("test");
  lang.terminals().add("e").add("f").add("k");
  lang.rule("xxx")({"A", "B", "C"})({"e"});
  cout << "done";
}
