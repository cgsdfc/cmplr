#ifndef LANGUAGE_H
#define LANGUAGE_H 1

#include <vector>
#include <unordered_set>

class Rule;
class Symbol
{
  private:
    int id_;
    std::string name_;
  public:
    Symbol(std::string name):name_(name),id_(0) {}
    int getID() const 
    { 
      return id_;
    }
    const std::string& getName() const 
    { 
      return name_;
    }
    void setID(int id){ id_=id; }

    bool operator== (const Symbol& other) const
    {
      return name_ == other .name_;
    }

    Symbol(Symbol&& other)
    {
      id_=other.id_;
name_=other.name_;
}

};

namespace std
{
  template<>
    struct hash<Symbol>
    {
      std::size_t operator () (const Symbol& symbol) const
      {
        return hash<std::string>() (symbol.getName());
      }
    };
}

class Alternative
{
  private:
    std::vector<Rule> alters_;

  public:
    void addRule(Rule& rule);
};


class Rule
{
  public:
    void addSymbol(Symbol& symbol);
Rule(Rule&& other)
{
rihtHandSides_=std::move(other.rightHandSides_);

  private:
    std::vector<Symbol> rightHandSides_;
};

class Language
{
  private:
    std::unordered_set<Symbol>
      symbols_;
    std::vector<Alternative> alters_;

  public:
    Alternative& define(Symbol &&leftHandSide);

};

Alternative& operator | (Alternative& alter, Rule rule);
Alternative& operator |= (Alternative& alter, Rule rule);
Rule operator << (Rule rule, Symbol symbol);

#endif


