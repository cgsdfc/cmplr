#include "error.h"
#include "color.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

UTILLIB_ETAB_BEGIN(utillib_error_lv)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_ICE, RED_ICE)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_FATAL, RED_FATAL)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_ERROR, RED_ERROR)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_WARNING, PURPLE_WARNING)
UTILLIB_ETAB_ELEM_INIT(ERROR_LV_NOTE, CYAN_NOTE)
UTILLIB_ETAB_END(utillib_error_lv)

void utillib_error_base_init(utillib_error_base *base, utillib_error_tag *tag) {
  base->tag = tag;
}

void utillib_error_base_destroy_trivial(utillib_error_base *base) {
  free(base);
}

utillib_error_cleanup_func_t const *utillib_error_cleanup_func(void) {
  static utillib_error_cleanup_func_t const* cleanup_func;
  return cleanup_func;
}

void utillib_die(const char *msg) {
  utillib_error_cleanup_func_t * cleanup
  =utillib_error_cleanup_func();
  if (cleanup) { cleanup(); }
  fprintf(stderr, "%s %s\n", utillib_error_lv_tostring(ERROR_LV_ICE), msg);
  exit(1);
}

void utillib_error_stack_init(utillib_error_stack *s) {
  utillib_slist_init(&(s->list));
}

void utillib_error_stack_push(utillib_error_stack *s, utillib_error_base *e) {
  int r = utillib_slist_push_front(&(s->list), e);
  assert(r == 0);
}

utillib_error_base *utillib_error_stack_top(utillib_error_stack *s) {
  return utillib_slist_front(&(s->list));
}

utillib_error_base *utillib_error_stack_pop(utillib_error_stack *s) {
  return utillib_slist_pop_front(&(s->list));
}

bool utillib_error_stack_empty(utillib_error_stack *s) {
  return utillib_slist_empty(&(s->list));
}

size_t utillib_error_stack_size(utillib_error_stack *s) {
  return utillib_slist_size(&(s->list));
}

void utillib_error_stack_destroy(utillib_error_stack *s) {
  UTILLIB_SLIST_FOREACH(utillib_error_base *, e, &(s->list)) {
    utillib_error_destructor *dtor = e->tag->dtor;
    assert(dtor);
    dtor(e);
  }
  utillib_slist_destroy(&(s->list));
}
