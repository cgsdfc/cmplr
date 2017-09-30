#include "utillib/slist.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct utillib_slist_node
{
  struct utillib_slist_node *next;
  void *data;
} utillib_slist_node;

void
utillib_slist_init (utillib_slist * list)
{
  memset (list, 0, sizeof *list);
}

static utillib_slist_node *
make_node (void * t)
{
  utillib_slist_node *node = calloc (sizeof *node, 1);
  if (node)
    {
      node->data=t;
    }
  return node;
}

static void
push_front_aux (utillib_slist * list, utillib_slist_node * node)
{
  utillib_slist_node **tail = (&list->tail);
  node->next = (*tail);
  *tail = node;
}

int utillib_slist_push_front (utillib_slist *list, void *data)
{
  utillib_slist_node *to_push = 
  make_node(data);
  if (to_push) {
    push_front_aux(list, to_push);
    return 0;
  }
  return -1;
}

static void
destroy_node(utillib_slist_node *node)
{
  free (node);
}

void *
utillib_pop_front (utillib_slist *list)
{
  assert (list->tail);
  utillib_slist_node * tail=list->tail;
  list->tail = tail->next;
  void *data=tail->data;
  destroy_node(tail);
  return data;
}

void
utillib_slist_destroy(utillib_slist * list)
{
  utillib_slist_node *tail;
  while (list->tail)
    {
      tail = list->tail;
      list->tail = tail->next;
      destroy_node(tail);
    }
}

bool
utillib_slist_empty (utillib_slist *list)
{
  return list->tail == NULL;
}

size_t
utillib_slist_size(utillib_slist *list)
{
  size_t i=0;
  for (utillib_slist_node *tail=list->tail;
      tail != NULL;
      tail=tail->next) { i++; }
  return i;
}
