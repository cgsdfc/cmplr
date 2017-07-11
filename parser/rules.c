#include "rules.h"

grammar grammar_clang;
language lang_clang= 
{
  .name="C programming language",
  .num_nonterm=80,
  .num_terminal=10,
  .num_keyword=40,
  .num_operator=50
};


int init_grammar(grammar *gr, language *lang)
{
  memset (gr, 0, sizeof (grammar));
  gr->symbol_id=lang->num_symbol;
  gr->name=lang->name;
  gr->nonterm_id=0;
  gr->terminal_id=gr->nonterm_id + lang->num_nonterm;
  gr->keyword_id=gr->terminal_id + lang->num_terminal;
  gr->operator_id=gr->keyword_id + lang->num_keyword;
  gr->punctuation_id=gr->operator_id + lang->num_operator;

  return 0;
}

int def_keyword(grammar *gr, char *rep)
{
  int next=gr->keyword_id;
  gr->symbol[next]=rep;
  return gr->keyword_id++;
}


int def_nonterm(grammar *gr, char *rep)
{
  int next=gr->nonterm_id;
  gr->symbol[next]=rep;
  return gr->nonterm_id++;
}

int def_terminal(grammar *gr, char *rep)
{
  int next=gr->terminal_id;
  gr->symbol[next]=rep;
  return gr->terminal_id++;
}

int def_punc(grammar *gr, char *rep)
{
  int next=gr->punctuation_id;
  gr->symbol[next]=rep;
  return gr->punctuation_id++;
}

int def_oper(grammar *gr, char *rep)
{
  int next=gr->operator_id;
  gr->symbol[next]=rep;
  return gr->operator_id++;
}

bool is_terminal(grammar *gr, int symbol)
{
  return gr->terminal_id <= symbol && symbol < gr->keyword_id;
}

bool is_nonterm(grammar *gr, int symbol)
{
  return 0 <= symbol && symbol < gr->nonterm_id;
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

  for (; *src != -1;++src)
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

void show_rules(grammar *gr)
{
  int nnont=gr->nonterm_id;
  char *symbol;
  rule *r;
  int nrules;
  int unresolved=0;
  int unrev[10];

  printf(">>>>>>>>> grammar rules for %s <<<<<<<<<<<\n\n",gr->name);

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



