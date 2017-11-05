#ifndef ITEM_H
#define ITEM_H 1
#define ITEM_SET_INIT_LEN 20
#include "rules.h"
#include <stdlib.h>

void print_core(grammar *);
void print_item_set_all(grammar *);
char *format_rule(grammar *, int);
char *format_item(grammar *, int);
void print_item_set(grammar *, item_set *);

int alloc_item(grammar *, item *);
bool item_is_same(item *, item *);
int lookup_item(grammar *, item *, int *);
int lookup_item_set(grammar *, item_set *, int *);
int alloc_item_set(grammar *, item_set *);
void item_set_append(item_set *, int);
void item_set_init(item_set *);
int item_set2id(grammar *, item_set *);
int item2id(grammar *, item *);

int build_item_set(grammar *);
int do_closure(grammar *, item_set *);

void init_easy(void);
void init_simple(void);
void init_quick(grammar *);

bool is_nullable(grammar *gr, int nonterm);
int get_num_terminal(grammar *gr);

#endif
