// tst.c TRINARY_SEARCH_TREE
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tst_node;

typedef struct tst_node {
  struct tst_node *m_left;
  struct tst_node *m_right;
  struct tst_node *m_mid;
  void *data;
  int m_value;
} tst_node;

typedef struct tst_tree {
  struct tst_node *m_root;
  int m_null_val;
} tst_tree;

void tst_tree_init(tst_tree *self, int null_val) {
  self->m_root = NULL;
  self->m_null_val = null_val;
}

// init the root of tst with a null_val
// that signals the end of the sequence to
// search
void tst_node_init(tst_node *self, int value) {
  self->m_left = self->m_right = NULL;
  self->m_value = value;
  memset(self->m_mid, 0, sizeof self->m_mid);
}

tst_node *tst_node_create(int value) {
  tst_node *node;
  node = malloc(sizeof(struct tst_node));
  if (node) {
    tst_init(node, value);
  }
  return node;
}

// add a null_val terminated sequence to tst.
// readdition is not allowed.
// if succeeded, return data.
// if failed, return NULL.
// data should be on the heap and not abused.
// precond: data!=NULL, first!=NULL, *first!=self->null_val,
// self!=NULL.
void *tst_tree_add(tst_tree *self, int *first, void *data) {}
