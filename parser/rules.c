#include "rules.h"

grammar grammar_clang;

int init_grammar(grammar *gr, char *lang, int nnont)
{
  memset (gr, 0, sizeof (grammar));
  gr->nterm=nnont;
  gr->lang=lang;
  return 0;
}

int def_nonterm(grammar *gr, char *rep)
{
  int next=gr->nnont;
  gr->symbol[next]=rep;
  return gr->nnont++;
}

int def_terminal(grammar *gr, char *rep)
{
  int next=gr->nterm;
  gr->symbol[next]=rep;
  return gr->nterm++;
}

bool is_terminal(grammar *gr, int symbol)
{
  return symbol >= gr->nterm;
}

bool is_nonterm(grammar *gr, int symbol)
{
  return 0 <= symbol && symbol < gr->nterm;
}

// input is an int array of
// [ lhs, rhs,...] with all the opt prefix 
// expanded
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

  for (; *src != -1;++src)
  {
    switch (*src)
    {
      case -2:
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


void def_rule(grammar *gr, int lhs, ...)
{
  va_list ap;
  int symbols[10];
  int expanded_sym[10];

  va_start(ap, lhs);
  collect_symbol(ap,symbols);
  add_optional(gr,lhs, symbols, expanded_sym,0);
  va_end(ap);

}

void def_oneof(grammar *gr, int lhs, ...)
{
  va_list ap;
  int symbols[10];
  int expanded_sym[10];
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

void show_rules(grammar *gr)
{
  int nnont=gr->nnont;
  char *symbol;
  rule *r;
  int nrules;

  printf(">>>>>>>>> grammar rules for %s <<<<<<<<<<<\n\n",gr->lang);

  for (int i=0;i<nnont;++i)
  {
    symbol=gr->symbol[i];
    nrules=gr->nnont_rule[i];
    if (!nrules)
    {
      printf("[INFO] rules set for %s is empty\n", symbol);
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



