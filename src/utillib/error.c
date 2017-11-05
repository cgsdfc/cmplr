#include "error.h"
#include "color.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define _BSD_SOURCE
#include <string.h>

UTILLIB_ETAB_BEGIN(utillib_error_lv)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_ICE, RED_ICE)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_FATAL, RED_FATAL)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_ERROR, RED_ERROR)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_WARNING, PURPLE_WARNING)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_NOTE, CYAN_NOTE)
UTILLIB_ETAB_END(utillib_error_lv)

utillib_error *utillib_make_error(int lv, utillib_position *pos, char *msg) {
  utillib_error *error = malloc(sizeof *error);
  if (error) {
    error->ue_lv = lv;
    error->ue_msg = strdup(msg);
    memcpy(&(error->ue_pos), pos, sizeof *pos);
    return error;
  }
  utillib_die("utillib_make_error: malloc failed");
}

void utillib_destroy_error(utillib_error *self) {
  free(self->ue_msg);
  free(self);
}

utillib_error_cleanup_func_t *
utillib_error_cleanup_func(utillib_error_cleanup_func_t *func) {
  static utillib_error_cleanup_func_t *cleanup_func;
  if (func) {
    cleanup_func = func;
  }
  return cleanup_func;
}

void utillib_die(const char *msg) {
  utillib_error_cleanup_func_t *cleanup = utillib_error_cleanup_func(NULL);
  if (cleanup) {
    (cleanup)();
  }
  fprintf(stderr, "%s %s\n", utillib_error_lv_tostring(ERROR_LV_ERROR), msg);
  exit(1);
}
