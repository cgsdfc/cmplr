#ifndef UTILLIB_STRING_H
#define UTILLIB_STRING_H
#define UTILLIB_C_STR(S) ((S)->c_str)
#include <stdbool.h>
#include <stddef.h>

typedef struct utillib_string {
  char *c_str;
  size_t size, capacity;
} utillib_string;

typedef enum string_cmpop {
  STRING_EQ,
  STRING_GE,
  STRING_GT,
  STRING_NE,
  STRING_LE,
  STRING_LT,
} string_cmpop;

void utillib_string_clear(utillib_string *);
void utillib_string_erase_last(utillib_string *);
char const *utillib_string_c_str(struct utillib_string *);
size_t utillib_string_size(struct utillib_string *);
bool utillib_string_empty(struct utillib_string *);
int utillib_string_init(struct utillib_string *);
int utillib_string_append(struct utillib_string *, char);
void utillib_string_destroy(struct utillib_string *);
bool utillib_string_richcmp(utillib_string *, utillib_string *, string_cmpop);
#endif // UTILLIB_STRING_H
