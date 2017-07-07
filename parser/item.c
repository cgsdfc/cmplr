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

int get_sym_after_dot(grammar *gr, int item_id)
{
  item *it=gr->items + item_id;
  int dot=it->dot;
  rule *r=gr->rules + it->ruleid;
  return r->len == it->dot ? -1 : r->rhs[it->dot];
}


int alloc_item(grammar *gr, int ruleid, int dot)
{
  item *it;
  int len;

  for (int i=0;i<gr->item_id;++i)
  {
    it=gr->items + i;
    if (it->ruleid == ruleid && 
        it->dot == dot)
      return i;
  }

  it=gr->items + gr->item_id;
  it->ruleid=ruleid;
  it->dot=dot;
  return gr->item_id++;
}

void item_set_append(item_set *set, int item_id)
{
  set->items[set->len++]=item_id;
}

void print_item_set(grammar *gr, item_set *set)
{
  for (int i=0;i<set->len;++i)
  {
    puts(format_item(gr, set->items[i]));
  }
}



int do_closure(grammar *gr, item_set *set)
{
  bool accounted[1024];
  memset (accounted, 0, sizeof accounted);
  rule *r;
  item *itm;
  int lhs;
  int item_id;
  int len;
  int rhs;
  int symbol;
  int ruleid;

  for (int i=0; i<set->len;++i)
  {
    itm=gr->items + set->items[i];
    r=gr->rules + itm->ruleid;
    if (itm->dot == r->len)
    {
      continue;
    }
    symbol=r->rhs[itm->dot];
    if (accounted[symbol])
    {
      continue;
    }
    len=gr->nnont_rule[symbol];
    for (int j=0;j<len;++j)
    {
      ruleid=gr->nonterm[symbol][j];
      item_id=alloc_item(gr, ruleid, itm->dot);
      item_set_append(set, item_id);
    }
    accounted[symbol]=true;
  }

  return 0;
}


void build_item_set(grammar *gr, item_set *set)
{
  int rhs2item[1024][1024];
  int rhslen[1024];
  memset (rhslen,0,sizeof rhslen);
  item *it;
  int len;
  rule *r;
  int symbol;

  do_closure(gr,set);
  for (int i=0;i<set->len;++i)
  {
    it=gr->items + set->items[i];
    r=gr->rules + it->ruleid;
    if (r->len == it->dot)
    {
      continue;
    }
    symbol=r->rhs[it->dot];
    len=rhslen[symbol];
    rhs2item[symbol][len++]=it->ruleid;
  }

}




