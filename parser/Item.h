#ifndef ITEM_H
#define ITEM_H 1
#include <vector>

typedef enum parser_builder_limit
{
  PB_MAX_ITEM,
  PB_MAX_ITEM_SET,
  PB_MAX_ITEM_PER_SET,
} parser_builder_limit;

struct Item
{
  int ruleid;
  int dot;

  Item(int ruleid, int dot);
  bool operator == (const Item& rhs);

};

struct ItemSet 
{
  std::vector <Item> items;

  void append(int itemID);
  ItemSet& operator = (const ItemSet& rhs);

};


#endif

