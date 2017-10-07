#ifndef UTILLIB_SLIST_H
#define UTILLIB_SLIST_H
#include <stdbool.h>
#include <stddef.h>
#define UTILLIB_SLIST_FOREACH(T, X, L)\
  T X;\
  for (utillib_slist_node *_p=(L)->tail; _p != NULL && ((X)=_p->data); _p=_p->next)

typedef struct utillib_slist_node
{
  struct utillib_slist_node *next;
  void *data;
} utillib_slist_node;

typedef struct utillib_slist
{
  struct utillib_slist_node *tail;
} utillib_slist;

void utillib_slist_init (struct utillib_slist *);
void utillib_slist_destroy(struct utillib_slist *);
bool utillib_slist_empty (struct utillib_slist*);
int utillib_slist_push_front (struct utillib_slist * , void *);
size_t utillib_slist_size(struct utillib_slist *);
void * utillib_slist_front(struct utillib_slist *);
void * utillib_slist_pop_front (struct utillib_slist *);
void utillib_slist_destroy (struct utillib_slist *);
#endif // UTILLIB_SLIST_H
