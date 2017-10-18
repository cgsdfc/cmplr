#define _GNU_SOURCE
#include "error.h"
#include "print.h"
#include "unordered_map.h"
#include <stdio.h>

static void test_unordered_map(void) {
  utillib_unordered_map map;
  utillib_pair_t stu[] = {
      {"Tony", (utillib_value_t)1},    {"John", (utillib_value_t)2},
      {"Amy", (utillib_value_t)3},     {"Tom", (utillib_value_t)4},
      {"Sam", (utillib_value_t)5},     {"Johnson", (utillib_value_t)6},
      {"Simpson", (utillib_value_t)7}, {"Adison", (utillib_value_t)8},
      {"Sipson", (utillib_value_t)7},  {"Adison", (utillib_value_t)8},
      {"Simpson", (utillib_value_t)7}, {"Adison", (utillib_value_t)8},
      {"Smpson", (utillib_value_t)7},  {"Adison", (utillib_value_t)8},
      {"Simpon", (utillib_value_t)7},  {"Adison", (utillib_value_t)8},
      {"Simpson", (utillib_value_t)7}, {"Adison", (utillib_value_t)8},
      {"Siso", (utillib_value_t)7},    {"Adison", (utillib_value_t)8},
      {"Wang", (utillib_value_t)9},    {NULL, NULL}};

  utillib_unordered_map_init(&map, utillib_unordered_map_const_charp_ft());
  for (int i = 0; i < 100000; i++) {
    for (utillib_pair_t *p = stu; UTILLIB_PAIR_FIRST(p) != NULL; ++p) {
      utillib_unordered_map_insert(&map, p);
    }
  }
  for (utillib_pair_t *x = stu; UTILLIB_PAIR_FIRST(x) != NULL; ++x) {
    utillib_pair_t *p = utillib_unordered_map_find(&map, UTILLIB_PAIR_FIRST(x));

    if (p) {
      printf("found %s:%ld in map\n", (char *)UTILLIB_PAIR_FIRST(p),
             (long)UTILLIB_PAIR_SECOND(p));
    } else {
      printf("nothing found!\n");
    }
  }
  printf("data about map: size=%lu, nbucket=%lu, load-factor=%lf\n",
         utillib_unordered_map_size(&map),
         utillib_unordered_map_bucket_count(&map),
         utillib_unordered_map_load_factor(&map));
  utillib_unordered_map_destroy(&map);
}

static void test_vector(void) {
  const int N = 4;
  utillib_vector v;
  utillib_vector_init(&v);
  for (int i = 0; i < N; ++i) {
    utillib_vector_push_back(&v, i);
  }
  utillib_vector_print(&v, utillib_int_str);
  utillib_vector_destroy(&v);
}
int main() { test_vector(); }
