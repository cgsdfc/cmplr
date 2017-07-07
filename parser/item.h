#ifndef ITEM_H
#define ITEM_H 1
#define ITEM_SET_INIT_LEN 20
#include "rules.h"

char *format_rule(grammar *, int );
char *format_item(grammar*,int );
int alloc_item(grammar*,int,int);
void init_simple(void);
int do_closure(grammar *gr, item_set *set);
void print_item_set(grammar*,item_set*);
void item_set_append(item_set*,int);
#endif

