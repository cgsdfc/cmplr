#ifndef RECURSIVE_CONTEXT_H
#define RECURSIVE_CONTEXT_H
#include "lexer/lexer.h"
#include "utillib/vector.h"
// the interface between Lexer and RParser
struct node_base;
#define PCONTEXT_MAX_LOOKAHEAD 4
#define PCONTEXT_MAX_SUBTREE 10

typedef enum symbol_type
{
  SYMBOL_UNKOWN,
  EXPR_PRIMARY,
  EXPR_POSTFIX,
  EXPR_UNARY,
  EXPR_CAST,
  EXPR_MUL,
  EXPR_ADD,
  EXPR_SHIFT,
  EXPR_REL,
  EXPR_EQUAL,
  EXPR_BIT_AND,
  EXPR_BIT_XOR,
  EXPR_BIT_BIT_OR,
  EXPR_LOG_AND,
  EXPR_LOG_OR,
  EXPR_COND, 
  EXPR_ASSIGN,
  EXPR_EXPR,
} symbol_type;

enum {
  EXPR_CONST=EXPR_COND,
};

typedef struct pcontext
{
  Lexer *lexer;
  Token *lookaheads[PCONTEXT_MAX_LOOKAHEAD + 5];
  utillib_vector subtrees;
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
#endif // RECURSIVE_CONTEXT_H
