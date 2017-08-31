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


void print_item_set(grammar *gr, item_set *set)
{
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
    print_item_set(gr, gr->item_sets+i);
    printf("\n");
  }
}

void print_core(grammar *gr)
{
  int symbol;
  char *rep;

  for (int i=0;i<gr->item_set_id;++i)
  {
    printf("\tI[%d]", i); 
  }
  puts("");
  for (int j=0;j<gr->item_set_id;++j)
  {
    printf("I[%d]", j);
    for (int i=0;i<gr->item_set_id;++i)
    {
      symbol = gr->set2symbols[j][i];
      if (symbol == -1) 
        printf("\tnull");
      else 
        printf("\t%s", gr->symbol[symbol]);
    }
    puts("");
  }
}

int int_cmp(const void *int0, const void *int1)
{
  int i0=*(int*) int0;
  int i1=*(int*) int1;
  return i0-i1;
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
  qsort(set->items, set->len, sizeof(int), int_cmp);
  return set->len;
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

static bool nullable_nonterminal(grammar * gr, int nterm);
static void directly_nullable(grammar * gr);

int build_item_set(grammar *gr)
{
  int queue[1024];
  bool visited[1024];
  int head=0;
  int tail=0;
  int symbol_cnt;
  int cur_set;
  int next_set;
  int item_id;
  item temp;
  item_set temp_set, *set;

  memset(gr->set2symbols, -1, sizeof(gr->set2symbols));
  memset(visited,0,sizeof visited);
  memset(queue,0,sizeof queue);
  symbol_cnt=gr->symbol_id;

  // make the first item_set
  make_item(&temp, 0,0);
  item_set_init(&temp_set);
  item_set_append(&temp_set, alloc_item(gr,&temp));
  do_closure(gr, &temp_set);
  cur_set=item_set2id(gr, &temp_set);
  queue[tail++]=cur_set;
  visited[cur_set]=true;

  while (head != tail)
  {
    cur_set=queue[head++];
    visited[cur_set]=true;
    set = gr->item_sets + cur_set;

    for (int symbol=0; symbol<symbol_cnt; ++symbol)
    {
      item_set_init(&temp_set);
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
      if (!temp_set.len)
      {
        continue;
      }

      do_closure(gr, &temp_set);
      next_set=item_set2id(gr, &temp_set);
      add_edge(gr, cur_set, next_set, symbol);
      if (visited[next_set])
      {
        continue;
      }
      queue[tail++]=next_set;
      visited[next_set]=true;
    }
  }
  directly_nullable(gr);
  // tail == 375 
  return 0;
}


static bool
nullable_rule(grammar * gr, int ruleid)
{
  rule * r = gr->rules + ruleid;
  int len = r->len;
  if (!len)
    return true;
  for (int i=0;i<len;++i)
  {
    if (is_terminal(gr, r->rhs[i]))
      return false;
    if (gr->is_nullable[r->rhs[i]]!=1)
      return false;
  }
  return true;
}

static void
directly_nullable(grammar * gr)
{
  int nnont = gr->nonterm_id;
  for (int i=0;i<nnont;++i) 
  {
    if (!gr->nnont_rule[i])
    {
      gr->is_nullable[i]=1;
      return;
    }
    for (int j=0;j<gr->nnont_rule[i];++j)
    {
      rule *r=gr->rules + gr->nonterm[i][j];
      if (!r->len)
      {
        gr->is_nullable[i]=1;
        return;
      }
    }
  }
}

/* A nullable nonterminal is a nonterminal directly yielding empty,
 *  e.g., A ::= , or having a body containing of nullable nonterminals,
 *   e.g., A ::= B C D, where all B C and D yield empty. */
static bool 
nullable_nonterminal(grammar * gr, int nterm)
{
  int *result=&gr->is_nullable[nterm];
  int nrule;
  int ruleid;
  if (*result == 1 || *result == 0)
    return result;

  nrule=gr->nnont_rule[nterm];
  if (!nrule)
    return *result=1; // A ::=

  for (int i=0;i<nrule;++i) 
  {
    ruleid=gr->nonterm[nterm][i];
    if (nullable_rule(gr, ruleid))
      return *result=1;
  }
  return *result=0;
}


bool is_nullable(grammar *gr, int symbol)
{
  if (is_terminal(gr, symbol))
      return false;
  return nullable_nonterminal(gr, symbol);
}

int get_num_terminal(grammar * gr)
{
  return gr->symbol_id - gr->nonterm_id-1;
}

