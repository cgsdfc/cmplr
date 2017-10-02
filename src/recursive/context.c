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
  utillib_vector_init (&(context->subtrees));
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
  int r;
  r = utillib_vector_push_back (&(context->subtrees), node);
  assert (r == 0);
}

node_base *
pcontext_pop_node (pcontext * context)
{
  return utillib_vector_pop_back (&(context->subtrees));
}

node_base *
pcontext_top_node (pcontext * context)
{
  node_base *root = utillib_vector_back (&(context->subtrees));
  assert (root != NULL);
  return root;
}

size_t
pcontext_node_size (pcontext * context)
{
  return utillib_vector_size (&(context->subtrees));
}

bool
pcontext_get_unary_ontop (struct pcontext * context)
{
  return context->unary_on_top;
}

void
pcontext_set_unary_ontop (struct pcontext *context, bool ontop)
{
  context->unary_on_top = ontop;
}

void
pcontext_mark_prefix (struct pcontext *context, pcontext_prefix prefix,
		      bool value)
{
  assert (prefix >= 0 && prefix < PCONTEXT_N_PREFIX);
  context->prefix[prefix] = value;
}

bool
pcontext_test_prefix (struct pcontext *context, pcontext_prefix prefix)
{
  assert (prefix >= 0 && prefix < PCONTEXT_N_PREFIX);
  return context->prefix[prefix];
}
