#ifndef UTILLIB_ERROR_H
#define UTILLIB_ERROR_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "slist.h"
#include "utillib/enum.h"
#define UTILLIB_MAKE_ERROR_TAG_DECLARE(NAME)                                   \
  utillib_error_tag const *make_##NAME(void)
#define UTILLIB_MAKE_ERROR_TAG_DEFINE(NAME, LEVEL, DTOR)                       \
  UTILLIB_MAKE_ERROR_TAG_DECLARE(NAME) {                                       \
    static const utillib_error_tag NAME = {.level = (LEVEL), .dtor = (DTOR)};  \
    return &NAME;                                                              \
  }

UTILLIB_ENUM_BEGIN(utillib_error_lv)
UTILLIB_ENUM_ELEM(ERROR_LV_ICE)
UTILLIB_ENUM_ELEM(ERROR_LV_FATAL)
UTILLIB_ENUM_ELEM(ERROR_LV_ERROR)
UTILLIB_ENUM_ELEM(ERROR_LV_WARNING)
UTILLIB_ENUM_ELEM(ERROR_LV_NOTE)
UTILLIB_ENUM_END(utillib_error_lv)

struct utillib_error_base;
typedef void(utillib_error_destructor)(struct utillib_error_base *);
typedef struct utillib_error_tag {
  int level;
  /* virtual */
  utillib_error_destructor *dtor;
} utillib_error_tag;

typedef struct utillib_error_base {
  utillib_error_tag const *tag;
} utillib_error_base;

typedef struct utillib_error_stack { utillib_slist list; } utillib_error_stack;

void utillib_die(const char *);
void utillib_error_base_init(utillib_error_base *, utillib_error_tag *);
void utillib_error_stack_init(utillib_error_stack *);
void utillib_error_stack_push(utillib_error_stack *, utillib_error_base *);
utillib_error_base *utillib_error_stack_pop(utillib_error_stack *);
size_t utillib_error_stack_size(utillib_error_stack *s);
bool utillib_error_stack_empty(utillib_error_stack *s);
utillib_error_base *utillib_error_stack_top(utillib_error_stack *s);
void utillib_error_base_destroy_trivial(utillib_error_base *);
#endif // UTILLIB_ERROR_H
