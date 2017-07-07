#include "item.h"
char *format_item(grammar *gr, int item_id)
{
  static char buf[1024];
  item *it=gr->items + item_id;
  int ruleid=it->ruleid;
  rule *r=gr->rules + ruleid;
  int dot=it->dot;

  sprintf(buf, "creator[%d] %s := ",it->creator, gr->symbol[r->lhs]);
  for (int i=0;i<r->len ;++i)
  {
    if (i==dot) 
    {
      strcat(buf," ^");
    }
    strcat(buf," ");
    strcat(buf,gr->symbol[r->rhs[i]]);
  }
  if (dot==r->len)
    strcat(buf," ^");
  return buf;
}

char *format_rule(grammar *gr, int ruleid)
{
  static char buf[1024];
  rule *r=gr->rules + ruleid;
  sprintf(buf, "%s := ", gr->symbol[r->lhs]);
  for (int i=0;i<r->len ;++i)
  {
    strcat(buf," ");
    strcat(buf,gr->symbol[r->rhs[i]]);
  }
  return buf;
}

void add_edge(grammar *gr, int from, int to, int symbol)
{
  gr->set2symbols[from][to]=symbol;
}

int item_get_symbol (grammar *gr, int item_id)
{
  item *it=gr->items + item_id;
  rule *r=gr->rules + it->ruleid;
  return r->len == it->dot ? -1 : r->rhs[it->dot];
}

void make_item(item *itm, int dot, int ruleid, int creator)
{
  itm->dot=dot;
  itm->ruleid=ruleid;
  itm->creator=creator;
}

int item2id(grammar *gr, item *itm)
{
  int item_id, item_set_id;
  switch (lookup_item(gr, itm, &item_id, &item_set_id))
  {
    case 0:
      return item_id;
    case 1:
      return alloc_item(gr, itm);
  }
}

// lookup whether it is inside gr->items,
// if it is, its id is return at item_id and its creator is returned at item_set_id
// 0 is returned.
// if it isnot, 1 is returned.
// 
int lookup_item(grammar *gr, item *it, int *item_id, int *creator)
{

  item *pitem;
  for (int i=0;i<gr->item_id;++i)
  {
    pitem=gr->items+i;
    if (item_is_same(pitem, it))
    {
      *item_id=i;
      *creator=pitem->creator;
      return 0;
    }
  }

  return 1;
}

item_set *get_item_set(grammar *gr, int item_set_id)
{
  if (0<=item_set_id && item_set_id < gr->item_set_id)
  {
    return gr->item_sets + item_set_id;
  }
  return NULL;
}



int alloc_item(grammar *gr, item *it)
{
  memcpy (gr->items + gr->item_id, it, sizeof(item));
  return gr->item_id++;
}

void item_set_append(grammar *gr, int item_set_id, int item_id)
{
  item_set *set=gr->item_sets + item_set_id;
  set->items[set->len++]=item_id;
}

void print_item_set(grammar *gr, int item_set_id) 
{
  item_set *set=gr->item_sets + item_set_id;
  for (int i=0;i<set->len;++i)
  {
    puts(format_item(gr, set->items[i]));
  }
}



int do_closure(grammar *gr, int item_set_id)
{
  bool accounted[1024];
  memset (accounted, 0, sizeof accounted);
  item temp;
  int iid;
  int symbol;
  int nrule;
  int *rules;
  item_set *set;
  
  set=gr->item_sets + item_set_id;
  for (int i=0; i<set->len;++i)
  {
    // get the symbol after dot
    symbol=item_get_symbol(gr, set->items[i]);
    switch (symbol) {
      case -1:
        // no symbol after dot
        continue;
      default:
        if (accounted[ symbol ])
          continue;
        nrule=gr->nnont_rule[symbol];
        rules=gr->nonterm[symbol];

        for (int j=0;j<nrule;++j)
        {
          make_item(&temp, 0, rules[j], item_set_id);
          iid=item2id(gr, &temp);
          item_set_append(gr, item_set_id, iid);
        }
        accounted[symbol]=true;
        break;
    }
  }
}



int alloc_item_set(grammar *gr)
{
  return gr->item_set_id++;
}

item *get_item(grammar *gr, int item_id)
{
  if (0<=item_id && item_id < gr->item_id)
  {
    return gr->items + item_id;
  }
  return NULL;
}

bool item_is_same(item *s, item*t)
{
  return (s->ruleid == t->ruleid && s->dot == t->dot);
}


// try to move the dot of the item denoted by item_id 
// one symbol leftwards.
// 
// if there is no symbol following the dot or the symbol following
// the dot does not match `symbol`, return 0;
// 
// the moved item already exists(created by another item_set),
// return 1 and old_item 's id is put at moved->do, the creator
// 's id is put at moved->belong.
// 
// if the item is new, nothing is done except returning 2.
// 
int item_try_move(grammar *gr,  int symbol, int item_id, item *temp, int *creator)
{
  item *old_item;
  int dot;
  int ruleid;
  rule *r;
  int iid;

  old_item=gr->items + item_id;
  r=gr->rules + old_item->ruleid;
  if (r->len == old_item->dot)
    // reach end of rule
    return 0;
  if (r->rhs[old_item->dot]!=symbol)
    // symbol after dot is not `symbol`
    return 0;
 
  make_item(temp, old_item->dot+1, old_item->ruleid,0);
  switch (lookup_item(gr, temp, &iid, creator))
  {
    case 0:
      // found
      return 1;
    case 1:
      // not found
      return 2;
  }
  
}

void print_item_set_all(grammar *gr)
{
  for (int i=0;i<gr->item_set_id;++i)
  {
    printf("I[%d]\n", i);
    print_item_set(gr, i);
    printf("\n");
  }
}

void print_core(grammar *gr)
{
  int symbol;
  char *rep;

  for (int i=0;i<gr->symbol_id;++i)
  {
    printf("\t%s", gr->symbol[i]);
  }
  puts("");
  for (int j=0;j<gr->item_set_id;++j)
  {
    printf("I[%d]", j);
    for (int i=0;i<gr->symbol_id;++i)
    {
      symbol = gr->set2symbols[j][i];
      printf("\t%d", symbol);
    }
    puts("");
  }
}


/* int item_try_move(item *it, */ 
int build_item_set(grammar *gr)
{
  int queue[1024];
  int head=0;
  int tail=0;
  int nnont;
  int cur_set;
  int next_set;
  int creator;
  item_set *set;
  int item_id;
  item temp;

  memset(gr->set2symbols, -1, sizeof(gr->set2symbols));
  nnont=gr->symbol_id;
  cur_set=alloc_item_set(gr);
  make_item(&temp, 0,0,0);
  item_set_append(gr, cur_set, alloc_item(gr,&temp));
  queue[tail++]=cur_set;

get_queue_head:
  while (head != tail)
  {
    cur_set=queue[head++];
    do_closure(gr, cur_set);
    set = get_item_set(gr, cur_set);
    for (int symbol=0; symbol<nnont; ++symbol)
    {
      next_set=-1;
      for (int i=0;i<set->len;++i)
      {
        switch (item_try_move(gr,symbol, set->items[i], &temp, &creator))
        {
          case 0:
            continue;
          case 1:
            add_edge(gr, cur_set, creator, symbol);
            goto get_queue_head;

          case 2:
           if (next_set==-1)
           {
             next_set=alloc_item_set(gr);
             add_edge(gr, cur_set, next_set, symbol);
           }
           temp.creator=next_set;
           item_id=alloc_item(gr, &temp);
           item_set_append(gr, next_set, item_id);
           break;
        } 
      }
      if (next_set != -1)
      {
        queue[tail++]=next_set;
      }
    }
  }
  
}

