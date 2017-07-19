#ifndef PARSER_BUILDER_H
#define PARSER_BUILDER_H

#include "Item.h"
#include "ParserTable.h"
#include "Grammar.h"

#include <vector>

class ParserBuilder
{
  public:
    ParserBuilder(Grammar& grammar, ParserTable& table);
    int buildItemSet(void);
    void printCore(void);
    void printItemSets(void);
    

  private:
    Grammar& grammar;
    ParserTable& table;
    std::vector <Item> items;
    std::vector <ItemSet> itemSets;
  private:
    int allocItem(const Item& item);
    int lookupItem(const Item& item, int *itemID);
    int allocItemSet(const ItemSet& itemSet);
    int lookupItemSet(const ItemSet& itemSet, int *itemSetID);
    int doClosure(const ItemSet& itemSet);
    int item2ID(const Item& item);
    int itemSet2ID(const ItemSet& itemSet);


} ;

#endif

