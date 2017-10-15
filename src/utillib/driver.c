#include "error.h"
#include "except.h"
#include "input_buf.h"
#include "unordered_map.h"

int main() {
  utillib_unordered_map map;
  utillib_pair_t stu[] = {
      {"Tony", (utillib_value_t)1},    {"John", (utillib_value_t)2},
      {"Amy", (utillib_value_t)3},     {"Tom", (utillib_value_t)4},
      {"Sam", (utillib_value_t)5},     {"Johnson", (utillib_value_t)6},
      {"Simpson", (utillib_value_t)7}, {"Adison", (utillib_value_t)8},
      {"Wang", (utillib_value_t)9},    {NULL, NULL}};

  utillib_unordered_map_init_from_array(
      &map, utillib_unordered_map_const_charp_ft(), stu);
  for (utillib_pair_t *x = stu; UTILLIB_PAIR_FIRST(x) != NULL; ++x) {
    utillib_pair_t *p = utillib_unordered_map_find(&map, UTILLIB_PAIR_FIRST(x));

    if (p) {
      printf("found %s:%d in map\n", (char *)UTILLIB_PAIR_FIRST(p),
             (int)UTILLIB_PAIR_SECOND(p));
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
