#define MAX_SYMBOLS_LEN 10

typedef struct item 
{
  int dot;
  int rule;
} item;

typedef struct rule
{
  int lhs;
  int rhs[MAX_LHS_LEN];
  int len;
} rule;

char *symbols[MAX_SYMBOLS_LEN];
rule rules[MAX_SYMBOLS_LEN];
int nonterms[MAX_NONTERMS_LEN][MAX_SYMBOLS_LEN];
int noncount[MAX_NONTERMS_LEN];
int nonterm_count;
int terminal_count;

int def_terminal(char *rep)
{
  int id=terminal_count;
  symbols[id]=rep;
  terminal_count++;


