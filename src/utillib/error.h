#ifndef UTILLIB_ERROR_H
#define UTILLIB_ERROR_H
#include "enum.h"
#include "position.h"
#include "slist.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

UTILLIB_ENUM_BEGIN(utillib_error_lv)
UTILLIB_ENUM_ELEM(ERROR_LV_ICE)
UTILLIB_ENUM_ELEM(ERROR_LV_FATAL)
UTILLIB_ENUM_ELEM(ERROR_LV_ERROR)
UTILLIB_ENUM_ELEM(ERROR_LV_WARNING)
UTILLIB_ENUM_ELEM(ERROR_LV_NOTE)
UTILLIB_ENUM_END(utillib_error_lv)

typedef void(utillib_error_cleanup_func_t)(void);

typedef struct utillib_error {
  int ue_lv;
  utillib_position ue_pos;
  char *ue_msg;
} utillib_error;

void utillib_die(const char *);
utillib_error_cleanup_func_t *
utillib_error_cleanup_func(utillib_error_cleanup_func_t *);
utillib_error *utillib_make_error(int, utillib_position *, char *);
void utillib_destroy_error(utillib_error *);
#endif // UTILLIB_ERROR_H
