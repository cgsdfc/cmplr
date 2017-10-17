#define _GNU_SOURCE
#include <stdio.h>
#include "error.h"
#include "except.h"
#include "input_buf.h"
#include "unordered_map.h"
static void test_unordered_map(void)
{
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

static void test_input_buf(void) {
  int c;
  char s[1024]="static void test_input_buf(void) {\n";
  utillib_input_buf buf;
  FILE *file=fmemopen(s, 1024, "r");
  utillib_input_buf_init(&buf, file, INPUT_BUF_STRING);
  while ((c=utillib_input_buf_getc(&buf))!=EOF) {
    printf("%lu:%lu:%c\n", 
        utillib_input_buf_row(&buf),
        utillib_input_buf_col(&buf),
        c);
    if (c=='(') {
      utillib_error *err=utillib_make_error(ERROR_LV_ERROR, 
          utillib_input_buf_current_pos(&buf),
          "bad LP");
      utillib_input_buf_pretty_perror(&buf, err);
      utillib_destroy_error(err);
      break;
    }
  }
  utillib_input_buf_destroy(&buf);
}

int main() {
  test_input_buf();
}
