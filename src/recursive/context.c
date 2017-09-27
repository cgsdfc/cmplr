#include "recursive/context.h"
#include "recursive/node_base.h"

int
pcontext_init (pcontext * context, Lexer * lexer)
{
  if (!context || !lexer)
    {
      return 1;
    }
  memset (context, 0, sizeof *context);
  context->lexer = lexer;
  return 0;
}

Token *
pcontext_read_token (pcontext * context, unsigned pos)
{
  assert (pos < PCONTEXT_MAX_LOOKAHEAD);
  Token **lookaheads = context->lookaheads;
  for (int i = 0; i <= pos; i++)
    {
      if (!lookaheads[i])
	{
	  lookaheads[i] = LexerGetToken (context->lexer);
	}
    }
  return lookaheads[pos];
}

void
pcontext_shift_token (pcontext * context, unsigned num)
{
  assert (num <= PCONTEXT_MAX_LOOKAHEAD);
  Token **lookaheads = context->lookaheads;
  unsigned limit = PCONTEXT_MAX_LOOKAHEAD - num;
  // from pos to limit, shift one position
  for (int i = 0; i < limit; ++i)
    {
      lookaheads[i] = lookaheads[i + num];
    }
}

void
pcontext_push_node (pcontext * context, node_base * node)
{
  assert (context->subtree_top < PCONTEXT_MAX_SUBTREE);
  context->subtrees[context->subtree_top++] = node;
}

node_base *
pcontext_pop_node (pcontext * context)
{
  assert (context->subtree_top > 0);
  // TODO: use vector instead
  return context->subtrees[--context->subtree_top];
}

node_base *
pcontext_top_node (pcontext * context)
{
  node_base *root = context->subtrees[0];
  assert (root != NULL);
  return root;
}
