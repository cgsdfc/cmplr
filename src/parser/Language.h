#ifndef LANGUAGE_H
#define LANGUAGE_H 1

#include <memory>
#include <unordered_set>
#include <vector>

namespace parser {
class Rule;
class Symbol {
private:
  int id_;
  std::string name_;

public:
  Symbol() {}

  Symbol(const std::string &name) : name_(name), id_(0) {}

  Symbol(const char *name) : name_(name), id_(0) {}

  Symbol(int id, const std::string name) : name_(name), id_(id) {}

  int getID() const { return id_; }
  const std::string &getName() const { return name_; }
  void setID(int id) { id_ = id; }

  bool operator==(const Symbol &other) const { return name_ == other.name_; }
};
}
namespace std {
using parser::Symbol;
template <> struct hash<Symbol> {
  std::size_t operator()(const Symbol &symbol) const {
    return hash<std::string>()(symbol.getName());
  }
};
}

namespace parser {
class Alternative {
private:
  std::vector<Rule> alters_;
  Symbol lhs;

public:
  Alternative() {}

  Alternative(const Symbol &symbol) : lhs(symbol) {}

  void addRule(const Rule &rule) { alters_.push_back(rule); }
};

class Rule {
public:
  void addSymbol(const char *symbol) { addSymbol(Symbol(symbol)); }

  void addSymbol(const Symbol &symbol) { rightHandSides_.emplace_back(symbol); }

private:
  std::vector<Symbol> rightHandSides_;
};

struct Rhs {
  Rule rule_;
  std::vector<Rule> rules_;

  std::vector<Rule> &getRules() { return rules_; }

  template <typename T, typename... Args>
  std::vector<Rule> &operator()(const T &t, const Args... args);

  std::vector<Rule> &operator()();
};

class Language {
private:
  std::unordered_set<Symbol> symbols_;
  std::vector<Alternative> alters_;

public:
  Alternative &lhs(const char *symbol) {
    auto iter = symbols_.find(symbol);
    if (iter == symbols_.end()) {
      Symbol sym(symbols_.size(), symbol);
      symbols_.emplace(sym);
      alters_.emplace_back(Alternative(sym));
      return alters_.back();
    }

    return alters_[iter->getID()];
  }

  template <typename... Args> std::vector<Rule> &rhs(const Args &... args) {
    std::shared_ptr<Rhs> rhs_(new Rhs());
    rhs_->operator()(args...);
    return rhs_->getRules();
  }
};

std::vector<Rule> &Rhs::operator()() { return rules_; }

template <typename T, typename... Args>
std::vector<Rule> &Rhs::operator()(const T &t, const Args... args) {
  rule_.addSymbol(t);
  return operator()(args...);
}

Alternative &operator|(Alternative &alter, std::vector<Rule> &rules) {
  for (auto &rule : rules) {
    alter.addRule(rule);
  }
}
}
#endif
