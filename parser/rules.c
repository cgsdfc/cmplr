#include "rules.h"

int init_grammar(grammar *gr, int nnont)
{
  memset (gr, 0, sizeof (grammar));
  gr->nterm=nnont;
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

void def_rule(grammar *gr, int lhs,...)
{
  rule *r;
  int rhs;
  va_list ap;
  int nrule;
  int len;

  len=gr->nnont_rule[lhs];
  nrule=gr->nrule;
  r=gr->rules + nrule;
  r->lhs=lhs;
  gr->nonterm[lhs][len]=nrule;
  gr->nnont_rule[lhs]++;
  gr->nrule++;
  va_start(ap,lhs);
  for (rhs=va_arg(ap, int); rhs != -1; rhs=va_arg(ap, int))
  {
    r->rhs[r->len++]=rhs;
  }
}

void show_rules(grammar *gr)
{
  int nnont=gr->nnont;
  char *symbol;
  rule *r;

  for (int i=0;i<nnont;++i)
  {
    symbol=gr->symbol[i];
    printf("%s := ", symbol);
    for (int j=0;j<gr->nnont_rule[i];++j)
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
