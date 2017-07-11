#include "item.h"
char *format_item(grammar *gr, int item_id)
{
  static char buf[1024];
  item *it=gr->items + item_id;
  int ruleid=it->ruleid;
  rule *r=gr->rules + ruleid;
  int dot=it->dot;

  sprintf(buf, "%s := ", gr->symbol[r->lhs]);
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

void make_item(item *itm, int dot, int ruleid)
{
  itm->dot=dot;
  itm->ruleid=ruleid;
}

int item2id(grammar *gr, item *itm)
{
  int item_id, item_set_id;
  switch (lookup_item(gr, itm, &item_id))
  {
    case 0:
      return item_id;
    case 1:
      return alloc_item(gr, itm);
  }
}

// lookup whether it is inside gr->items,
// if it is, its id is return at item_id 
// 0 is returned.
// if it isnot, 1 is returned.
// 
int lookup_item(grammar *gr, item *it, int *item_id)
{

  item *pitem;
  for (int i=0;i<gr->item_id;++i)
  {
    pitem=gr->items+i;
    if (item_is_same(pitem, it))
    {
      *item_id=i;
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

void item_set_init(item_set *set)
{
  memset(set,0,sizeof(item_set));
}

void item_set_append(item_set *set, int item_id)
{
  set->items[set->len++]=item_id;
}

int item_set2id(grammar *gr, item_set *set)
{
  int item_set_id;
  // like item2id, but search for set
  switch (lookup_item_set(gr, set, &item_set_id))
  {
    case 0:
      return item_set_id;
    case 1:
      return alloc_item_set(gr, set);
  }
}

int lookup_item_set(grammar *gr, item_set *set, int *item_set_id)
{
  item_set *pset;
  for (int i=0;i<gr->item_set_id;++i)
  {
    pset=gr->item_sets+i;
    if (memcmp(pset, set, sizeof(item_set))==0)
    {
      *item_set_id=i;
      return 0;
    }
  }
  return 1;
}


void print_item_set(grammar *gr, int item_set_id) 
{
  item_set *set=gr->item_sets + item_set_id;
  for (int i=0;i<set->len;++i)
  {
    puts(format_item(gr, set->items[i]));
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





int do_closure(grammar *gr, item_set *set)
{
  bool accounted[1024];
  memset (accounted, 0, sizeof accounted);
  item temp;
  int iid;
  int symbol;
  int nrule;
  int *rules;
  
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
          make_item(&temp, 0, rules[j]);
          iid=item2id(gr, &temp);
          item_set_append(set, iid);
        }
        accounted[symbol]=true;
        break;
    }
  }
}



int alloc_item_set(grammar *gr, item_set *set)
{
  item_set *pset=gr->item_sets + gr->item_set_id;
  memcpy(pset, set, sizeof(item_set));
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
// otherwise, the dot was moved(changing temp) and 1 is returned
// 
int item_try_move(grammar *gr,  int symbol, int item_id, item *temp)
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
 
  make_item(temp, old_item->dot+1, old_item->ruleid);
  return 1; 
}

// build up all the item sets and their transition
// using the following BFS algo:
// starting with an incomplete item set I0 with only the top rule
// S := top, we first do_closure on it, then put it into a queue.
// every time dequeue an item_set to be visited called cur_set. 
// from the cur_set we visit all the items where the dot is prefixed
// a certain symbol. we make a temp item_set that holding these items
// plus the closure. but before enqueue this new item_set, we first
// check whether the set with exactly the same items was created before.
// if so, we just go on to process the next symbol after adding a transition.
// otherwise we give an id to the new item_set and add it to the 
// to the tail of the queue.  
int build_item_set(grammar *gr)
{
  int queue[1024];
  bool visited[1024];
  int head=0;
  int tail=0;
  int nnont;
  int cur_set;
  int next_set;
  int item_id;
  item temp;
  item_set temp_set, *set;

  memset(gr->set2symbols, -1, sizeof(gr->set2symbols));
  memset(visited,0,sizeof visited);
  memset(queue,0,sizeof queue);
  nnont=gr->symbol_id;

  // make the first item_set
  make_item(&temp, 0,0);
  item_set_init(&temp_set);
  item_set_append(&temp_set, alloc_item(gr,&temp));
  do_closure(gr, &temp_set);
  cur_set=item_set2id(gr, &temp_set);
  queue[tail++]=cur_set;

  while (head != tail)
  {
    cur_set=queue[head++];
    set = gr->item_sets + cur_set;
    item_set_init(&temp_set);

    for (int symbol=0; symbol<nnont; ++symbol)
    {
      for (int i=0;i<set->len;++i)
      {
        switch (item_try_move(gr,symbol, set->items[i], &temp))
        {
          case 0:
            continue;
          case 1:
           item_id=item2id(gr, &temp);
           item_set_append(&temp_set, item_id);
           break;
        } 
      }
      do_closure(gr, &temp_set);
      next_set=item_set2id(gr, &temp_set);
      if (next_set <= gr->item_set_id)
        continue;

      add_edge(gr, cur_set, next_set, symbol);
      queue[tail++]=next_set;
    }
  }
  
}

