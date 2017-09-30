#ifndef UTILLIB_SLIST_H
#define UTILLIB_SLIST_H
#include <stdbool.h>
#include <stddef.h>

typedef struct utillib_slist
{
  struct utillib_slist_node *tail;
} utillib_slist;

void utillib_slist_init (struct utillib_slist *);
void utillib_slist_destroy(struct utillib_slist *);
bool utillib_slist_emptu (struct utillib_slist*);
int utillib_slist_push_front (struct utillib_slist * , void *);
size_t utillib_slist_size(struct utillib_slist *);
void * utillib_slist_front(struct utillib_slist *);
void * utillib_pop_front (struct utillib_slist *);
void utillib_slist_destroy (struct utillib_slist *);
#endif // UTILLIB_SLIST_H
