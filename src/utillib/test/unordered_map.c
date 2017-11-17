#include <utillib/pair.h>
#include <utillib/test.h>
#include <utillib/unordered_map.h>

static void test_unordered_map(void) {
  struct utillib_unordered_map map;
  struct utillib_pair stu[] = {
      {"Tony", 1},    {"John", 2},    {"Amy", 3},     {"Tom", 4},
      {"Sam", 5},     {"Johnson", 6}, {"Simpson", 7}, {"Adison", 8},
      {"Sipson", 7},  {"Adison", 8},  {"Simpson", 7}, {"Adison", 8},
      {"Smpson", 7},  {"Adison", 8},  {"Simpon", 7},  {"Adison", 8},
      {"Simpson", 7}, {"Adison", 8},  {"Siso", 7},    {"Adison", 8},
      {"Wang", 9},    {NULL, NULL}};

  utillib_unordered_map_init(&map, &utillib_unordered_strop);
  for (int i = 0; i < 100000; i++) {
    for (struct utillib_pair *p = stu; UTILLIB_PAIR_FIRST(p) != NULL; ++p) {
      utillib_unordered_map_insert(&map, p);
    }
  }
  for (struct utillib_pair *x = stu; UTILLIB_PAIR_FIRST(x) != NULL; ++x) {
    struct utillib_pair *p =
        utillib_unordered_map_find(&map, UTILLIB_PAIR_FIRST(x));

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
