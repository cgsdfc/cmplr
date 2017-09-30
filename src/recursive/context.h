#ifndef RECURSIVE_CONTEXT_H
#define RECURSIVE_CONTEXT_H
#include "lexer/lexer.h"
#include "utillib/vector.h"
// the interface between Lexer and RParser
struct node_base;
#define PCONTEXT_MAX_LOOKAHEAD 4
#define PCONTEXT_MAX_SUBTREE 10

typedef enum pcontext_prefix
{
  PCONTEXT_UNARY,
  PCONTEXT_DESP_DECLR,
  PCONTEXT_N_PREFIX,
} pcontext_prefix;

typedef struct pcontext
{
  Lexer *lexer;
  Token *lookaheads[PCONTEXT_MAX_LOOKAHEAD + 5];
  utillib_vector subtrees;
  bool prefix[PCONTEXT_N_PREFIX];
  bool unary_on_top;
} pcontext;

int pcontext_init (struct pcontext *, Lexer *);
Token *pcontext_read_token (struct pcontext *, unsigned);
void pcontext_shift_token (struct pcontext *, unsigned);
void pcontext_push_node (struct pcontext *, struct node_base *);
struct node_base *pcontext_pop_node (struct pcontext *);
struct node_base *pcontext_top_node (struct pcontext *);
size_t pcontext_node_size (struct pcontext *);
bool pcontext_get_unary_ontop(struct pcontext*);
void pcontext_set_unary_ontop(struct pcontext *, bool);
void pcontext_mark_prefix(struct pcontext *, pcontext_prefix, bool);
bool pcontext_test_prefix(struct pcontext *, pcontext_prefix);
#endif // RECURSIVE_CONTEXT_H
