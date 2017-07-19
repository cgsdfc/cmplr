#include "Grammar.h"

void Grammar::printRule(int ruleID)
{
  const Rule& rule = rules[ruleID];
  printf("%s = ", symbols[rule.lhs];

  for (int r:rule.rhs)
  {
    printf ("%s ", symbols[r]);
  }
}

void Grammar::Grammar (const vector <int> & symbol_blk)
{
  for (int i=0;i<symbol_blk.size();++i)
  {
    this->symbol_blk[i]=symbol_blk[i];
  }

}

void Grammar::addRule(int lhs, int ruleid)
{
int rulelen;

rulelen=gr->nnont_rule[lhs];
gr->nonterm[lhs][rulelen]=ruleid;
gr->nnont_rule[lhs]++;
}

int Grammar::alloc_rule(int lhs, int *rhs, int len)
{
  int ruleid;
  rule *r;

  ruleid=gr->nrule;
  r = &gr->rules[ruleid];
  r->lhs=lhs;
  memcpy (r->rhs, rhs, len * sizeof(int));
  r->len=len;
  gr->nrule++;
  return ruleid;

}



void Grammar::add_optional(int lhs, int *src, int *rhs, int len)
{
  int ruleid;

  for (; *src != RULE_END;++src)
  {
    switch (*src)
    {
      case RULE_OPT:
        Grammar::add_optional(gr,lhs, src+1, rhs, len);
        Grammar::add_optional(gr,lhs, src+2,rhs,len);
        return;
      default:
        rhs[len++]=*src;
        break;
    }
  }
  ruleid=grammar_alloc_rule(gr, lhs, rhs, len);
  grammar_add_rule(gr, lhs, ruleid);

}

void grammar_collect_symbol(va_list ap, int *symbols)
{
  int len=0;
  int rhs;

  for (rhs=va_arg(ap, int);rhs!=-1;rhs=va_arg(ap,int))
  {
    symbols[len++]=rhs;
  }
  symbols[len]=-1;

}


void Grammar::def_rule(int lhs, ...)
{
  va_list ap;
  int symbols[30];
  int expanded_sym[30];

  va_start(ap, lhs);
  grammar_collect_symbol(ap,symbols);
  grammar_add_optional(gr,lhs, symbols, expanded_sym,0);
  va_end(ap);

}

void grammar_def_oneof(int lhs, ...)
{
  va_list ap;
  int symbols[30];
  int expanded_sym[30];
  int ruleid;

  va_start(ap, lhs);
  grammar_collect_symbol(ap,symbols);
  for (int *sym=symbols; *sym!=-1;++sym)
  {
    ruleid=grammar_alloc_rule(gr,lhs,sym,1);
    grammar_add_rule(gr, lhs, ruleid);
  }

  va_end(ap);
}

void Grammar::def_onemore(int lhs, int rhs)
{
  int symbol[3];
  symbol[0]=rhs;
  grammar_add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,1));
  symbol[0]=lhs;
  symbol[1]=rhs;
  Grammar::add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,2));
}

void Grammar::def_sepmore(int lhs, int sep, int rhs)
{
  int symbol[3];
  symbol[0]=rhs;
  Grammar::add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,1));
  symbol[0]=lhs;
  symbol[1]=sep;
  symbol[2]=rhs;
  Grammar::add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,3));
}


void Grammar::def_symbol(const char *sym, int id)
{
  if (gr->symbol[id]) 
  {
    fprintf(stderr, "def_symbol: %s, %d was defined\n", sym, id);
    return ;
  }
  gr->symbol[id]=sym;
}

void Grammar::show_symbols(grammar *gr)
{
  int nsym=gr->symbol_blk[N_SYMBOLS_IDX];
  for (int i=0;i<nsym;++i)
  {
    printf ("%s, %d\n", gr->symbol[i], i);
  }
}


void Grammar::show_rules(grammar *gr)
{
  int nnont=gr->symbol_blk[N_NONTERM_IDX];
  const char *symbol;
  rule *r;
  int nrules;
  int unresolved=0;
  int unrev[10];


  for (int i=0;i<nnont;++i)
  {
    symbol=gr->symbol[i];
    nrules=gr->nnont_rule[i];
    if (!nrules)
    {
      unrev[unresolved++]=i;
      continue;
    }

    printf("%s := ", symbol);
    for (int j=0;j<nrules; ++j)
    {
      if (j>0)
        printf("\t| ");
      r=gr->rules+gr->nonterm[i][j];
      for (int k=0;k<r->len;++k)
      {
        printf("%s ", gr->symbol[r->rhs[k]]);
      }
      puts("\n");
    }
  }
}



