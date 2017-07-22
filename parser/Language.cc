#include "Language.h"

void Alternative::addRule(const Rule& rule)
{
  alters_.push_back(rule);
}

Alternative& operator | (Alternative& alter, Rule& rule)
{
  alter.addRule(rule);
  return alter;
}


void Rule::addSymbol(Symbol& symbol)
{
  rightHandSides_.push_back(symbol);
}



