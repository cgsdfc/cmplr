#include "lexer/list.h"
#include "lexer/tokenizer.h"
#include "lexer/token.h"
#include "lexer/char_buffer.h"

typedef struct token_list_node
{
  struct token_list_node *prev;
  struct token_list_node *next;
  struct token *t;
} token_list_node;

// O <- O <- O <- tail
int
init_token_list (token_list * list, char_buffer * cb)
{
  list->bad = false;
  list->eof = false;
  list->cb = cb;
  list->tail = list->cur = NULL;
  return init_tokenizer ();
}

static token_list_node *
make_node (token * t)
{
  token_list_node *node = malloc (sizeof *node);
  if (node)
    {
      node->t = t;
    }
  return node;
}

static void
destroy_node(token_list_node * node)
{
  destroy_token(node->t);
  free(node->t);
}

static void
append_node (token_list * list, token_list_node * node)
{
  // push_back
  if (list->tail)
    {
      // non empty case
      node->next = NULL;
      node->prev = list->tail;
      list->tail->next = node;
      list->tail = node;
    }
  else
    {
      // empty case
      list->tail = node;
      node->prev = node->next = NULL;
    }
  list->cur = list->tail;
}

static int
add_node (token_list * list)
{
  token *t = malloc (sizeof *t);
  token_list_node *node;
  switch (get_next_token (list->cb, t))
    {
    case 0:
      node = make_node (t);
      append_node (list, node);
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
  if (list->cur == NULL)
    {
      int r = add_node (list);
      if (r)
	{
	  return r;
	}
    }
  *tk = list->cur->t;
  return 0;
}

void
consume_token (token_list * list)
{
  token_list_node *cur = list->cur;
  if (cur)
    {
      list->cur = cur->next;
    }
}

void
destroy_token_list (token_list * list)
{
  token_list_node *tail;
  while (list->tail)
    {
      tail = list->tail;
      list->tail = tail->prev;
      destroy_node(tail);
      free (tail);
    }
}
