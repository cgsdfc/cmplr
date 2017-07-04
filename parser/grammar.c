#include "grammar.h"
#include "rules.h"
static grammar glbl_grammar;
static grammar_config glbl_grammar_config;


int def_grammar_begin(int *top_start, int nnonterms)
{
  int r;
  if (init_rule_allocator()<0)
    return -1;
  return init_grammar(&glbl_grammar,
      &glbl_grammar_config,
      top_start,
      nnonterms);
}

int def_nonterm(const char *rep)
{

}

int def_terminal(const char *rep)
{

}

int def_keyword(const char *keyword)
{


}


int def_grammar_rule(int *lhs,...)
{
  va_list ap;
  int r;
  va_start(ap,lhs);
  r=add_rule(*glbl_grammar,
      &glbl_grammar_config,
      lhs,
      ap);
  va_end(ap);
  return r;
}


int init_grammar(grammar *gr,
    grammar_config *gc,
    int *top_start,
    int nnonterms)
{
  if (! (gr && gc && top_start))
    return -1;
  if (nnonterms <= 0)
    return -1;

  int **nont2rules;
  int *len2rule;
  int *max2rule;
  int rule_max=GR_INIT_RULES;

  memset(gr,0,sizeof(grammar));
  memset(gc,0,sizeof(grammar_config));

  nont2rules=malloc(sizeof(int*) * nnonterms);
  len2rule=malloc(sizeof(int) * nnonterms);
  max2rule=malloc(sizeof(int) * nnonterms);
  all_rules=malloc(sizeof(rule*) * rule_max);
  if (! (nont2rules && len2rule && max2rule && all_rules))
    return -1;

  gr->all_rules=all_rules;
  gr->rule_max=rule_max;
  gr->nnonterms=nnonterms;
  gr->nont2rules=nont2rules;
  gr->len2rule=len2rule;
  gr->max2rule=max2rule;
  gc->term_id=nnonterms;
  gc->nonterm_id=2;
  // S := `S EOF, S=0, `S=1=*top_start
  gr->all_rules[gr->rule_len++]=make_top_rule();
  return 0;
}

int alloc_nonterm(grammar_config *gc)
{
  return gc->nonterm_id++;
}

// TODO generalize this, make it a memory pool
void *do_resize(void *array, int len, int elemsz, int *newlen)
{
  if (!(array && newlen))
    return NULL;

  void *newspace;
  *newlen = (len&1)?(len<<1):(len*3+5);
  newspace = malloc(elemsz * (*newlen));
  if (!newspace)
    return NULL;

  memcpy(newspace, array, elemsz*len);
  free(array);

  return newspace;
}

int add_rule_for_symbol(grammar *gr,
    int lhs,
    int rule)
{
  int len;
  int max;
  int rules;
  int newlen;
  int *newspace;

  len=gr->len2rule[lhs];
  max=gr->max2rule[lhs];
  rules=gr->nont2rules[lhs];

  if (len==max)
  {
    newspace=do_resize(rules,len,sizeof(int), &newlen);
    if (!newspace)
      return -1;
    gr->nont2rules[lhs]=newspace;
    gr->max2rule[lhs]=newlen;
  }

  newspace[len++]=rule;
  gr->len2rule[lhs]++;
  return 0;
}



int add_rule(grammar *gr,
    grammar_config *gc,
    int *lhs, va_list ap)
{
  if (! (gr && gc && lhs))
    return -1;

  int *symbol;
  rule *r;
  int new_rule_max;
  int rule_len;
  rule **new_rules;

  if (!*lhs)
  {
    *lhs=alloc_nonterm(gc);
  }

  r=alloc_rule(*lhs);
  if (!r)
    return -1;

  if (gr->rule_len == gr->rule_max)
  {
    new_rules=do_resize(gr->all_rules,
        gr->rule_len,
        sizeof(rule*),
        &new_rule_max);

    if (!new_rules)
      return -1;
    gr->all_rules=new_rules;
    gr->rule_max=new_rule_max;
  }

  do {
    symbol=va_arg(ap, int*);
    if (!*symbol)
      *symbol=alloc_nonterm(gc);
    rule_append(r, *symbol);
  } while (symbol != NULL);

  rule_len=gr->rule_len;
  gr->all_rules[rule_len]=r;
  if (add_rule_for_symbol(gr, rule_len)<0)
    return -1;

  gr->rule_len++;
  return 0;
}









