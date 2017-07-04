#include "rules.h"
static stack_buffer *rule_alloc;
static stack_buffer *item_alloc;
static stack_buffer *item_set_alloc;

int init_rule_allocator(void)
{
  rule_alloc = alloc_stack_buffer(sizeof(rule));
  item_alloc = alloc_stack_buffer(sizeof(item));
  item_set_alloc = alloc_stack_buffer(sizeof(item_set));

  if (! (rule_alloc && item_alloc && item_set_alloc))
    return -1;

  return 0;
}

rule *alloc_rule(int lhs)
{
  rule *r;
  int max;
  int *rhs;

  r=stack_buffer_alloc(rule_alloc);
  if (!r)
    return NULL;
  max=RULE_INIT_SYM_LEN;
  rhs=malloc(sizeof(int)*max);
  if(!rhs)
    return NULL;

  r->len=0;
  r->max=max;
  r->lhs=lhs;
  r->rhs=rhs;
  return 0;
}

int rule_append(rule *r, int symbol)
{
  if (!r || symbol <= 0)
    return -1;

  int *newspace;
  int newlen;

  if (r->len == r->max)
  {
    newspace=do_resize(r->rhs,r->len,sizeof(int),&newlen);
    if (!newspace)
      return -1;
    r->rhs=newspace;
    r->max=newlen;
  }

  r->rhs[r->len++]=symbol;
  return 0;
}


rule *make_top_rule(void)
{
  rule *r;
  int lhs=0;
  int len=0;

  r=alloc_rule(lhs);
  r->rhs[len++]=1;
  r->rhs[len++]=EOF;
  r->len=len;
  return r;
}

  


