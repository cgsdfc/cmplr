#include "rules.h"

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
void add_rule(grammar *gr, int *input, int len)
{
  // the rule is about to add
  int ruleid;
  int rulelen;
  rule *r;
  int lhs;

  // get the lhs of the rule
  lhs=input[0];
  input++;
  len--;
  // get the current rule id
  ruleid=gr->nrule;
  r = &gr->rules[ruleid];
  rulelen=gr->nnont_rule[lhs];

  // initialize the new rule
  r->lhs=lhs;
  memcpy (r->rhs, input, len * sizeof(int));
  r->len=len;
  // put the rule under lhs's rules row
  gr->nonterm[lhs][rulelen]=ruleid;
  gr->nrule++;
  gr->nnont_rule[lhs]++;
}

void expand_opt(grammar *gr, int *src, int *dst, int len)
{
  for (; *src != -1;++src)
  {
    switch (*src)
    {
      case -2:
        expand_opt(gr, src+1, dst, len);
        expand_opt(gr, src+2,dst,len);
        break;
      default:
        dst[len++]=*src;
        break;
    }
  }
  add_rule(gr, dst, len);

}

void def_rule(grammar *gr, int lhs,...)
{
  int rhs;
  va_list ap;
  int len=0;
  int array[10];
  int dst[10];

  va_start(ap,lhs);
  array[len++]=lhs;
  for (rhs=va_arg(ap, int);rhs!=-1;rhs=va_arg(ap,int))
  {
    array[len++]=rhs;
  }
  array[len]=-1;
  expand_opt(gr, array, dst, 0);
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
