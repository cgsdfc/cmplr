#define _GNU_SOURCE
#include "logging.h"
#include "print.h"  // for utillib_static_vsprintf
#include <stdlib.h> // for abort
#include <string.h> // strdup

UTILLIB_ETAB_BEGIN(logging_level_t)
UTILLIB_ETAB_ELEM(DEBUG)
UTILLIB_ETAB_ELEM(INFO)
UTILLIB_ETAB_ELEM(WARNING)
UTILLIB_ETAB_ELEM(ERROR)
UTILLIB_ETAB_ELEM(FATAL)
UTILLIB_ETAB_END(logging_level_t)

static utillib_logging_core_t static_logging_core;

static void utillib_logging_core_set_logdir(utillib_logging_core_t *self,
                                            char const *logdir) {
  self->lco_logdir = logdir;
}

static void
utillib_logging_core_set_stderr_threshold(utillib_logging_core_t *self,
                                          int level) {
  self->lco_stderr_threshold = level;
}

static void utillib_logging_core_set_cleanup(utillib_logging_core_t *self,
                                             void *toclean,
                                             void (*cleanup)(void *)) {
  self->lco_toclean = toclean;
  self->lco_cleanup = cleanup;
}

static char const *core_str_incr(utillib_logging_core_t *self,
                                 char const *str) {
  return utillib_strref_incr(&self->lco_strref, str);
}

static void core_str_decr(utillib_logging_core_t *self, char const *str) {
  utillib_strref_decr(&self->lco_strref, str);
}

static void core_push_free(utillib_logging_core_t *self,
                           utillib_logging_msg_t *logmsg) {
  utillib_logging_msg_t **phead = &self->lco_free;
  logmsg->lmsg_msg = (void *)*phead;
  *phead = logmsg;
}

static utillib_logging_msg_t *core_pop_free(utillib_logging_core_t *self) {
  utillib_logging_msg_t *logmsg = self->lco_free;
  self->lco_free = (void *)logmsg->lmsg_msg;
  return logmsg;
}

static void core_init(utillib_logging_core_t *self, char const *prog) {
  static const char *lco_anonymous = "(anonymous)";
  utillib_vector_init(&self->lco_msgs);
  utillib_strref_init(&self->lco_strref);
  self->lco_prog = prog ? prog : lco_anonymous;
  self->lco_free = NULL;
}

static void utillib_logging_msg_destroy(utillib_logging_msg_t *self) {
  utillib_logging_core_t *core = &static_logging_core;
  core_str_decr(core, self->lmsg_file);
  core_str_decr(core, self->lmsg_func);
  free(self->lmsg_msg);
  free(self);
}

static void core_destroy(utillib_logging_core_t *self) {
  utillib_vector_destroy_owning(&self->lco_msgs, (void *)free);
  utillib_strref_destroy(&self->lco_strref);
}

static utillib_logging_msg_t *
core_make_logging_msg(utillib_logging_core_t *core, enum logging_level_t level,
                      char const *file, char const *func, size_t line,
                      char const *msg) {
  utillib_logging_msg_t *logmsg;
  if (core->lco_free) {
    logmsg = core_pop_free(core);
  } else {
    logmsg = malloc(sizeof *logmsg);
  }
  /* inline Initializes */
  logmsg->lmsg_msg = strdup(msg);
  logmsg->lmsg_lv = level;
  logmsg->lmsg_line = line;
  logmsg->lmsg_file = core_str_incr(core, file);
  logmsg->lmsg_func = core_str_incr(core, func);
  return logmsg;
}

static void utillib_logmsg_output(FILE *stream, utillib_logging_msg_t *msg) {
  char const *lv_str = logging_level_t_tostring(msg->lmsg_lv);
  fprintf(stream, "%s:%s:%s:%lu: %s\n", lv_str, msg->lmsg_file, msg->lmsg_func,
          msg->lmsg_line, msg->lmsg_msg);
}

static void core_push_back_logmsg(utillib_logging_core_t *self,
                                  utillib_logging_msg_t *msg) {
  utillib_logmsg_output(stderr, msg);
  utillib_logging_msg_destroy(msg);
}

static void utillib_logmsgV(utillib_logging_core_t *core, int level,
                            char const *file, char const *func, size_t line,
                            char const *fmt, va_list ap) {
  char const *msg = utillib_static_vsprintf(fmt, ap);
  utillib_logging_msg_t *logmsg =
      core_make_logging_msg(core, level, file, func, line, msg);
  core_push_back_logmsg(core, logmsg);
}

void utillib_logging_init(char const *prog) {
  core_init(&static_logging_core, prog);
}

void utillib_logging_destroy(void) { core_destroy(&static_logging_core); }

void utillib_logging_set_cleanup(void *toclean, void (*cleanup)(void *)) {
  utillib_logging_core_set_cleanup(&static_logging_core, toclean, cleanup);
}

void utillib_logging_logmsg(int level, char const *file, char const *func,
                            size_t line, char const *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  utillib_logmsgV(&static_logging_core, level, file, func, line, fmt, ap);
  va_end(ap);
}

void utillib_logging_set_stderr_threshold(int level) {
  utillib_logging_core_set_stderr_threshold(&static_logging_core, level);
}

void utillib_logging_set_logdir(char const *logdir) {
  utillib_logging_core_set_logdir(&static_logging_core, logdir);
}
