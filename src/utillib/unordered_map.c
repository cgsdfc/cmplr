#include <assert.h>
#include "unordered_map.h"
#include "hash.h"
#include "equal.h"
#include "error.h"
#include "slist.h"
UTILLIB_ENUM_BEGIN(find_mode_t)
  UTILLIB_ENUM_ELEM(FIND_ONLY)
  UTILLIB_ENUM_ELEM(FORCE_INSERT)
UTILLIB_ENUM_END(find_mode_t)

// using a linking way
static size_t do_hash(
    utillib_unordered_map * self, utillib_key_t key)
{
  return self->un_ft->un_hash(key) % self->un_capacity;
}
static bool do_equal(
    utillib_unordered_map * self, utillib_key_t lhs, utillib_key_t rhs
) {
  return self->un_ft->un_equal(lhs, rhs);
}

static utillib_slist * make_slist(void)
{
  // create an empty slist on the heap
  utillib_slist *list=malloc (sizeof *list);
  if (list) {
    utillib_slist_init(list);
    return list;
  }
  utillib_die("NOMEM in make_slist");
}

static void destroy_slist(utillib_slist *list)
{
  UTILLIB_SLIST_FOREACH(utillib_pair_t*, data, list) {
    free(data);
  }
  utillib_slist_destroy(list);
  free(list);
}

static utillib_pair_t * make_pair(utillib_key_t key, utillib_value_t value)
{
  utillib_pair_t * pair=malloc(sizeof *pair);
  if (pair){
    *pair[0]=key;
    *pair[1]=value;
    return pair;
  }
  utillib_die("NOMEM in make_pair");
}

void utillib_unordered_map_init(
    utillib_unordered_map * self ,
    utillib_unordered_ft * ft
) {
  static const size_t init_capacity=10;
  self->un_ft=ft;
  self->un_capacity=init_capacity;
  utillib_vector_init(&(self->un_bucket));
  for (int i=0;i<init_capacity;++i) {
    utillib_slist *l=make_slist();
    utillib_vector_push_back(&(self->un_bucket), l);
  }
}

void utillib_unordered_map_destroy(
    utillib_unordered_map *self
) {
  size_t capacity=self->un_capacity;
  for (int i=0;i<capacity;++i) {
    utillib_slist * list=utillib_vector_at(&(self->un_bucket), i);
    destroy_slist(list);
  }
  utillib_vector_destroy(&(self->un_bucket));
}

static utillib_pair_t * linear_search_list(
    utillib_unordered_map * self, 
    utillib_slist *list, utillib_key_t key, utillib_slist_node ** pos
) {
  utillib_slist_node *node=UTILLIB_SLIST_HEAD(list);
  for (; node!=NULL; node=UTILLIB_SLIST_NODE_NEXT(node)) {
    utillib_pair_t * pair=UTILLIB_SLIST_NODE_DATA(node);
    if (do_equal(self, key, UTILLIB_PAIR_FIRST(pair))) {
      if (pos) { *pos=node; }
      return pair;
    }
  }
  return NULL;
}

static int find_impl(utillib_unordered_map * self,
    utillib_key_t key, utillib_value_t value, int mode,
    utillib_slist ** plist,
    utillib_slist_node ** pos, 
    utillib_pair_t ** retv)
{
  size_t hashv=do_hash(self, key);
  utillib_slist *list=utillib_vector_at(&(self->un_bucket), hashv);
  utillib_pair_t *pair=linear_search_list(self, list, key, pos);
  if (retv && pair) { *retv=pair; }
  if (plist && pair) { *plist=list; }
  switch (mode) {
    case FIND_ONLY:
      return pair == NULL ? KEY_FOUND : KEY_MISSING;
    case FORCE_INSERT:
      if (pair) { 
        return KEY_EXISTS; 
      }
      pair=make_pair(key, value);
      utillib_slist_push_front(list, pair);
      return KEY_INSERTED;
    default:
      assert(false);
  }
}

int utillib_unordered_map_insert(
    utillib_unordered_map * self,
    utillib_value_t key, utillib_value_t value)
{                                               // list, node, pair
  return find_impl(self, key, value, FORCE_INSERT, NULL, NULL, NULL);
}

utillib_pair_t * utillib_unordered_map_find(
    utillib_unordered_map * self,
    utillib_key_t key)
{
  utillib_pair_t *p;                 // list, node, pair
  find_impl(self, key, NULL, FIND_ONLY, NULL, NULL, &p);
  return p;
}

int utillib_unordered_map_erase(
    utillib_unordered_map * self,
    utillib_key_t key)
{
  utillib_slist *l;
  utillib_slist_node *n;                 // list, node, pair
  switch(find_impl(self, key, NULL, FIND_ONLY, &l, &n, NULL)) {
    case KEY_MISSING:
      return KEY_MISSING;
    case KEY_FOUND:
      utillib_slist_erase(l, n);
      return KEY_FOUND;
  }
}

utillib_unordered_ft const* utillib_unordered_map_const_charp_ft(void)
{
  static const utillib_unordered_ft charp_tf = {
    .un_hash=(utillib_hash_func_t *) utillib_charp_hash,
    .un_equal=(utillib_equal_func_t *) utillib_charp_equal,
  };
  return &charp_tf;
}

