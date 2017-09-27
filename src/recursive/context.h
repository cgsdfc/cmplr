#ifndef RECURSIVE_CONTEXT_H
#define RECURSIVE_CONTEXT_H
#include "lexer/lexer.h"
// the interface between Lexer and RParser
struct node_base;
#define PCONTEXT_MAX_LOOKAHEAD 4
#define PCONTEXT_MAX_SUBTREE 10

typedef struct pcontext
{
  Lexer *lexer;
  Token *lookaheads[PCONTEXT_MAX_LOOKAHEAD + 5];
  struct node_base *subtrees[PCONTEXT_MAX_SUBTREE];
  int subtree_top;
} pcontext;

int pcontext_init (struct pcontext *, Lexer *);
Token *pcontext_read_token (struct pcontext *, unsigned);
void pcontext_shift_token (struct pcontext *, unsigned);
void pcontext_push_node (struct pcontext *, struct node_base *);
struct node_base *pcontext_pop_node (struct pcontext *);
struct node_base *pcontext_top_node (struct pcontext *);
#endif // RECURSIVE_CONTEXT_H
