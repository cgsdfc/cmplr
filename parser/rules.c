#include "rules.h"

int init_grammar(grammar *gr, const int *symol_blk, int len)
{
  if (!gr || !symol_blk || len <= 0)
    return -1;

  memset (gr, 0, sizeof (grammar));
  memcpy (gr->symbol_blk, symbol_blk, sizeof(int) * len);

  return 0;
}

void add_rule(grammar *gr, int lhs, int ruleid)
{
  int rulelen;

  rulelen=gr->nnont_rule[lhs];
  gr->nonterm[lhs][rulelen]=ruleid;
  gr->nnont_rule[lhs]++;
}

int alloc_rule(grammar *gr, int lhs, int *rhs, int len)
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



void add_optional(grammar *gr, int lhs, int *src, int *rhs, int len)
{
  int ruleid;

  for (; *src != RULE_END;++src)
  {
    switch (*src)
    {
      case RULE_OPT:
        add_optional(gr,lhs, src+1, rhs, len);
        add_optional(gr,lhs, src+2,rhs,len);
        return;
      default:
        rhs[len++]=*src;
        break;
    }
  }
  ruleid=alloc_rule(gr, lhs, rhs, len);
  add_rule(gr, lhs, ruleid);

}

void collect_symbol(va_list ap, int *symbols)
{
  int len=0;
  int rhs;

  for (rhs=va_arg(ap, int);rhs!=-1;rhs=va_arg(ap,int))
  {
    symbols[len++]=rhs;
  }
  symbols[len]=-1;

}


void _def_rule(grammar *gr, int lhs, ...)
{
  va_list ap;
  int symbols[30];
  int expanded_sym[30];

  va_start(ap, lhs);
  collect_symbol(ap,symbols);
  add_optional(gr,lhs, symbols, expanded_sym,0);
  va_end(ap);

}

void _def_oneof(grammar *gr, int lhs, ...)
{
  va_list ap;
  int symbols[30];
  int expanded_sym[30];
  int ruleid;

  va_start(ap, lhs);
  collect_symbol(ap,symbols);
  for (int *sym=symbols; *sym!=-1;++sym)
  {
    ruleid=alloc_rule(gr,lhs,sym,1);
    add_rule(gr, lhs, ruleid);
  }

  va_end(ap);
}

void def_onemore(grammar *gr, int lhs, int rhs)
{
  int symbol[3];
  symbol[0]=rhs;
  add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,1));
  symbol[0]=lhs;
  symbol[1]=rhs;
  add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,2));
}

void def_sepmore(grammar *gr, int lhs, int sep, int rhs)
{
  int symbol[3];
  symbol[0]=rhs;
  add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,1));
  symbol[0]=lhs;
  symbol[1]=sep;
  symbol[2]=rhs;
  add_rule(gr,lhs,alloc_rule(gr,lhs,symbol,3));
}


int _def_symbol(grammar *gr, const char *sym, int id)
{
  if (id < 0 || id > GR_MAX_SYMBOL)
    return -1;

  if (gr->symbol[sym]) 
  {
    fprintf(stderr, "def_symbol: %s, %d was defined\n", sym, id);
    return -1;
  }
  gr->symbol[id]=sym;
  return 0;
}

void show_symbols(grammar *gr)
{
  int nsym=gr->symbol_blk[RULE_N_SYMBOLS];
  for (int i=0;i<nsym;++i)
  {
    printf ("%s, %d\n", gr->symbol[i], i);
  }
}


void show_rules(grammar *gr)
{
  int nnont=gr->nonterm_id;
  char *symbol;
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



