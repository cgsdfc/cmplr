#ifndef UTILLIB_STRING_H
#define UTILLIB_STRING_H
#define UTILLIB_C_STR(S) ((S)->c_str)
#include <stdbool.h>
typedef unsigned utillib_string_size_type;
typedef char utillib_string_char_type;
typedef struct utillib_string {
  utillib_string_char_type *c_str;
  utillib_string_size_type size, capacity;
} utillib_string;

typedef enum string_cmpop {
  STRING_EQ,
  STRING_GE,
  STRING_GT,
  STRING_NE,
  STRING_LE,
  STRING_LT,
} string_cmpop;

utillib_string_size_type utillib_string_size(struct utillib_string *);
bool utillib_string_empty(struct utillib_string *);
int utillib_string_init(struct utillib_string *, utillib_string_size_type);
int utillib_string_append(struct utillib_string *, utillib_string_char_type);
void utillib_string_destroy(struct utillib_string *);
bool utillib_string_richcmp(utillib_string *, utillib_string *, string_cmpop);
#endif // UTILLIB_STRING_H
