#ifndef ITEM_H
#define ITEM_H 1
#define ITEM_SET_INIT_LEN 20
#include "rules.h"

void print_core(grammar*);
char *format_rule(grammar *, int);
char *format_item(grammar*,int);
int alloc_item(grammar*,item*);
void init_simple(void);
int do_closure(grammar*,int); 
bool item_is_same(item*,item*);
int lookup_item(grammar*, item*,int*,int*);
void print_item_set(grammar*,int);
void item_set_append(grammar *,int,int);
int build_item_set(grammar *); 
void print_item_set_all (grammar*);
void init_easy(void);
void init_quick(void);

#endif

