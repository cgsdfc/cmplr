#include "unordered_map.h"
#include "equal.h"
#include "error.h"
#include "hash.h"
#include "slist.h"
#include <assert.h>
UTILLIB_ENUM_BEGIN(find_mode_t)
UTILLIB_ENUM_ELEM(FIND_ONLY)
UTILLIB_ENUM_ELEM(FORCE_INSERT)
UTILLIB_ENUM_END(find_mode_t)

// using a linking way
static size_t do_hash(utillib_unordered_map *self, utillib_key_t key) {
  return self->un_ft->un_hash(key) % self->un_nbucket;
}
static bool do_equal(utillib_unordered_map *self, utillib_key_t lhs,
                     utillib_key_t rhs) {
  return self->un_ft->un_equal(lhs, rhs);
}

static utillib_slist *make_slist(void) {
  // create an empty slist on the heap
  utillib_slist *list = malloc(sizeof *list);
  if (list) {
    utillib_slist_init(list);
    return list;
  }
  utillib_die("NOMEM in make_slist");
}

static void destroy_slist(utillib_slist *list) {
  UTILLIB_SLIST_FOREACH(utillib_pair_t *, data, list) { free(data); }
  utillib_slist_destroy(list);
  free(list);
}

static utillib_pair_t *make_pair(utillib_key_t key, utillib_value_t value) {
  utillib_pair_t *pair = malloc(sizeof *pair);
  if (pair) {
    UTILLIB_PAIR_FIRST(pair) = key;
    UTILLIB_PAIR_SECOND(pair) = value;
    return pair;
  }
  utillib_die("NOMEM in make_pair");
}

static void push_back_bucket(utillib_unordered_map *self, size_t nbucket) {
  for (int i = 0; i < nbucket; ++i) {
    utillib_slist *l = make_slist();
    utillib_vector_push_back(&(self->un_bucket), l);
  }
}

void utillib_unordered_map_init(utillib_unordered_map *self,
                                utillib_unordered_map_ft ft) {
  static const size_t init_nbucket = 10;
  static const double init_max_lf = 15;
  self->un_nbucket = init_nbucket;
  self->un_max_lf = init_max_lf;
  self->un_ft = ft;
  self->un_size = 0;
  utillib_vector_init(&(self->un_bucket));
  push_back_bucket(self, init_nbucket);
}

void utillib_unordered_map_init_from_array(utillib_unordered_map *self,
                                           utillib_unordered_map_ft ft,
                                           utillib_pair_t const *p) {
  utillib_unordered_map_init(self, ft);
  for (; UTILLIB_PAIR_FIRST(p) != NULL; ++p) {
    utillib_unordered_map_insert(self, UTILLIB_PAIR_FIRST(p),
                                 UTILLIB_PAIR_SECOND(p));
  }
}

void utillib_unordered_map_destroy(utillib_unordered_map *self) {
  size_t nbucket = utillib_vector_size(&(self->un_bucket));
  for (int i = 0; i < nbucket; ++i) {
    utillib_slist *list = utillib_vector_at(&(self->un_bucket), i);
    destroy_slist(list);
  }
  utillib_vector_destroy(&(self->un_bucket));
}

static utillib_pair_t *linear_search_list(utillib_unordered_map *self,
                                          utillib_slist *list,
                                          utillib_key_t key,
                                          utillib_slist_node **pos) {
  utillib_slist_node *node = UTILLIB_SLIST_HEAD(list);
  for (; node != NULL; node = UTILLIB_SLIST_NODE_NEXT(node)) {
    utillib_pair_t *pair = UTILLIB_SLIST_NODE_DATA(node);
    if (do_equal(self, key, UTILLIB_PAIR_FIRST(pair))) {
      if (pos) {
        *pos = node;
      }
      return pair;
    }
  }
  return NULL;
}

static int find_impl(utillib_unordered_map *self, utillib_key_t key,
                     utillib_value_t value, int mode, utillib_slist **plist,
                     utillib_slist_node **pos, utillib_pair_t **retv) {
  size_t hashv = do_hash(self, key);
  utillib_slist *list = utillib_vector_at(&(self->un_bucket), hashv);
  utillib_pair_t *pair = linear_search_list(self, list, key, pos);
  if (retv && pair) {
    *retv = pair;
  }
  if (plist && pair) {
    *plist = list;
  }
  switch (mode) {
  case FIND_ONLY:
    return pair == NULL ? KEY_FOUND : KEY_MISSING;
  case FORCE_INSERT:
    if (pair) {
      return KEY_EXISTS;
    }
    pair = make_pair(key, value);
    utillib_slist_push_front(list, pair);
    ++(self->un_size);
    return KEY_INSERTED;
  default:
    assert(false);
  }
}

int utillib_unordered_map_emplace(utillib_unordered_map *self,
                                  utillib_value_t key,
                                  utillib_value_t value) { // list, node, pair
}

int utillib_unordered_map_insert(utillib_unordered_map *self,
                                 utillib_value_t key,
                                 utillib_value_t value) { // list, node, pair
  return find_impl(self, key, value, FORCE_INSERT, NULL, NULL, NULL);
}

utillib_pair_t *utillib_unordered_map_find(utillib_unordered_map *self,
                                           utillib_key_t key) {
  utillib_pair_t *p; // list, node, pair
  find_impl(self, key, NULL, FIND_ONLY, NULL, NULL, &p);
  return p;
}

int utillib_unordered_map_erase(utillib_unordered_map *self,
                                utillib_key_t key) {
  utillib_slist *l;
  utillib_slist_node *n; // list, node, pair
  switch (find_impl(self, key, NULL, FIND_ONLY, &l, &n, NULL)) {
  case KEY_MISSING:
    return KEY_MISSING;
  case KEY_FOUND:
    utillib_slist_erase(l, n);
    --(self->un_size);
    return KEY_FOUND;
  }
}

utillib_unordered_map_ft utillib_unordered_map_const_charp_ft(void) {
  static utillib_unordered_map_ft_impl charp_tf = {
      .un_hash = (utillib_hash_func_t *)utillib_charp_hash,
      .un_equal = (utillib_equal_func_t *)utillib_charp_equal,
  };
  return &charp_tf;
}

size_t utillib_unordered_map_size(utillib_unordered_map *self) {
  return self->un_size;
}

bool utillib_unordered_map_empty(utillib_unordered_map *self) {
  return utillib_unordered_map_size(self) == 0;
}

double utillib_unordered_map_load_factor(utillib_unordered_map *self) {
  return (double)utillib_unordered_map_size(self) /
         utillib_unordered_map_bucket_count(self);
}

size_t utillib_unordered_map_bucket_count(utillib_unordered_map *self) {
  return self->un_nbucket;
}

static int utillib_unordered_map_rehash_impl(utillib_unordered_map *self,
                                             size_t nbucket) {
  int r;
  utillib_vector holder;
  utillib_vector_init(&holder);
  r = utillib_vector_reserve(&holder, self->un_size);
  size_t cur_nb = utillib_vector_size(&(self->un_bucket));
  if (r != 0) {
    return r;
  }
  // move all the elem to holder from all buckets
  UTILLIB_VECTOR_FOREACH(utillib_slist *, l, &(self->un_bucket)) {
    for (utillib_slist_node *x = UTILLIB_SLIST_HEAD(l); x != NULL;
         x = UTILLIB_SLIST_NODE_NEXT(x)) {
      utillib_vector_push_back(&holder, x);
    }
    UTILLIB_SLIST_HEAD(l) = NULL;
  }
  for (int i = cur_nb; i < nbucket; ++i) {
    utillib_vector_push_back(&(self->un_bucket), make_slist());
  }
  UTILLIB_VECTOR_FOREACH(utillib_slist_node *, x, &holder) {
    utillib_pair_t *pair = UTILLIB_SLIST_NODE_DATA(x);
    size_t hashv = do_hash(self, UTILLIB_PAIR_FIRST(pair));
    utillib_slist *l = utillib_vector_at(&(self->un_bucket), hashv);
    utillib_slist_push_front_node(l, x);
  }
  self->un_nbucket = nbucket;
  utillib_vector_destroy(&holder);
  return 0;
}

size_t utillib_unordered_map_bucket_size(utillib_unordered_map *self,
                                         size_t n) {
  assert(n < self->un_nbucket);
  utillib_slist *l = utillib_vector_at(&(self->un_bucket), n);
  return utillib_slist_size(l);
}

size_t utillib_unordered_map_bucket(utillib_unordered_map *self,
                                    utillib_key_t key) {}

void utillib_unordered_map_set_max_load_factor(utillib_unordered_map *self,
                                               double max_lf) {
  self->un_max_lf = max_lf;
}
