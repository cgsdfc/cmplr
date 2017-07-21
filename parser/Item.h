#ifndef ITEM_H
#define ITEM_H 1
#include <vector>


struct Item
{
  int ruleid;
  int dot;

  Item(int ruleid, int dot);
  bool operator == (const Item& rhs);

};

struct ItemSet 
{
  std::vector <Item*> items;

  void append(int itemID);
  ItemSet& operator = (const ItemSet& rhs);

};


#endif

