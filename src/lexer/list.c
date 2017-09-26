#include "lexer/list.h"
#include "lexer/tokenizer.h"
#include "lexer/token.h"
#include "lexer/char_buffer.h"

typedef struct token_list_node
{
  struct token_list_node *next;
  struct token *token;
} token_list_node;

// O <- O <- O <- tail
void
init_token_list (token_list * list, char_buffer * cb)
{
  memset (list, 0, sizeof *list);
  list->cb = cb;
  init_tokenizer ();
}

static token_list_node *
make_node (token * t)
{
  token_list_node *node = calloc (sizeof *node, 1);
  if (node)
    {
      node->token = t;
    }
  return node;
}

static void
destroy_node (token_list_node * node)
{
  destroy_token (node->token);
  free (node->token);
}

static void
push_back_node (token_list * list, token_list_node * node)
{
  token_list_node **tail = (&list->tail);
  node->next = (*tail);
  *tail = node;
}

static int
push_back_token (token_list * list)
{
  token *t = malloc (sizeof *t);
  token_list_node *node;
  switch (get_next_token (list->cb, t))
    {
    case 0:
      node = make_node (t);
      if (!node)
	{
	  return 1;
	}
      push_back_node (list, node);
      return 0;
    case 1:
      free (t);
      list->bad = true;
      return 1;
    case EOF:
      free (t);
      list->eof = true;
      return EOF;
    }
}

int
get_token (token_list * list, token ** tk)
{
  if (list->eof)
    {
      return EOF;
    }
  if (list->bad)
    {
      return 1;
    }
  int r = push_back_token (list);
  if (r)
    {
      return r;
    }
  *tk = list->tail->token;
  return 0;
}


void
destroy_token_list (token_list * list)
{
  token_list_node *tail;
  while (list->tail)
    {
      tail = list->tail;
      list->tail = tail->next;
      destroy_node (tail);
      free (tail);
    }
}
