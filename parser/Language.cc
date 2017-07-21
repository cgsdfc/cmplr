#include "Language.h"

void Alternative::addRule(Rule& rule)
{
  alters_.push_back(rule);
}

Alternative& operator | (Alternative& alter, Rule& rule)
{
  alter.addRule(rule);
  return alter;
}


Alternative& operator |= (Alternative& alter, Rule& rule)
{
  alter.addRule(rule);
  return alter;
}

void Rule::addSymbol(Symbol& symbol)
{
  rightHandSides_.push_back(symbol);
}


Rule& operator << (Rule& rule, Symbol& symbol)
{
  rule.addSymbol(symbol);
  return rule;
}

Alternative& Language::define(Symbol &leftHandSide)
{
  auto iter=symbols_.find(leftHandSide);
  if (iter==symbols_.end())
  {
    int size=alters_.size();
    leftHandSide.setID(size);
    alters_.resize(size+1);
    Alternative& alter = alters_.back();
    symbols_.insert(leftHandSide);
    return alter;
  }

  int id=iter-> getID();
  return alters_[id];
}

