#include "slist.h"
#include "error.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void destroy_node(utillib_slist_node *node) { free(node); }
static void destory_slist(utillib_slist_node *node) {
  utillib_slist_node *x;
  while (node) {
    x = node;
    node = x->next;
    destroy_node(x);
  }
}

static void push_front_aux(utillib_slist_node **tail,
                           utillib_slist_node *node) {
  node->next = (*tail);
  *tail = node;
}
static void pop_front_aux(utillib_slist_node **tail,
                          utillib_slist_node **node) {
  *node = *tail;
  *tail = (*tail)->next;
}

static utillib_slist_node *make_node(utillib_slist *self, void *data) {
  utillib_slist_node *node;
  if (self->sl_free) {
    pop_front_aux(&(self->sl_free), &node);
    node->data = data;
    node->next = NULL;
    return node;
  }
  node = calloc(sizeof *node, 1);
  if (node) {
    node->data = data;
    return node;
  }
  utillib_die("ENOMEM in make_node");
}

void utillib_slist_push_front_node(utillib_slist *self,
                                   utillib_slist_node *node) {
  assert(node);
  push_front_aux(&(self->sl_tail), node);
}

void utillib_slist_init(utillib_slist *self) {
  self->sl_tail = self->sl_free = NULL;
}

void utillib_slist_push_front(utillib_slist *self, void *data) {
  utillib_slist_node *to_push = make_node(self, data);
  push_front_aux(&(self->sl_tail), to_push);
}

void utillib_slist_erase(utillib_slist *self, utillib_slist_node *node) {
  assert(node);
  utillib_slist_node **prev = &(self->sl_tail);
  for (; *prev && (*prev)->next != node; *prev = (*prev)->next)
    ;
  *prev = node->next;
  push_front_aux(&(self->sl_free), *prev);
}

void *utillib_slist_front(utillib_slist *self) {
  assert(self->sl_tail);
  return self->sl_tail->data;
}

void utillib_slist_pop_front(utillib_slist *self) {
  assert(self->sl_tail);
  utillib_slist_node *x;
  pop_front_aux(&(self->sl_tail), &x);
  push_front_aux(&(self->sl_free), x);
}

void utillib_slist_destroy(utillib_slist *self) {
  destory_slist(self->sl_tail);
  destory_slist(self->sl_free);
}

bool utillib_slist_empty(utillib_slist *self) { return self->sl_tail == NULL; }

size_t utillib_slist_size(utillib_slist *self) {
  size_t i = 0;
  for (utillib_slist_node *tail = self->sl_tail; tail != NULL;
       tail = tail->next) {
    i++;
  }
  return i;
}

void utillib_slist_iterator_init_null(utillib_slist_iterator *self) {
  self->iter_node = NULL;
}

void utillib_slist_iterator_init(utillib_slist_iterator *self,
                                 utillib_slist *cont) {
  self->iter_node = cont->sl_tail;
}

bool utillib_slist_iterator_has_next(utillib_slist_iterator *self) {
  return NULL != self->iter_node;
}

void utillib_slist_iterator_next(utillib_slist_iterator *self) {
  self->iter_node = UTILLIB_SLIST_NODE_NEXT(self->iter_node);
}

void *utillib_slist_iterator_get(utillib_slist_iterator *self) {
  return UTILLIB_SLIST_NODE_DATA(self->iter_node);
}
