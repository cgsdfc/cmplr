#ifndef UTILLIB_LOGING_H
#define UTILLIB_LOGING_H
#include "argp.h"
#include "enum.h"
#include "strref.h"
#include "typedef.h"
#include "unordered_map.h"
#include <assert.h>
#define UTILLIB_LOGGING_OPTIONS_STDERR_THRESHOLD(KEY)                          \
  UTILLIB_ARGP_OPTIOIN_ELEM(                                                   \
      "stderr-threshlod", KEY, "LEVEL",                                        \
      "the lowest severity level of loggings that go to stderr")
#define UTILLIB_LOGGING_OPTIONS_LOGDIR(KEY)                                    \
  UTILLIB_ARGP_OPTION_ELEM(                                                    \
      "log-dir", KEY, "the directory that loggings will go instead of `/tmp'")

UTILLIB_ENUM_BEGIN(logging_level_t)
UTILLIB_ENUM_ELEM(DEBUG)
UTILLIB_ENUM_ELEM(INFO)
UTILLIB_ENUM_ELEM(WARNING)
UTILLIB_ENUM_ELEM(ERROR)
UTILLIB_ENUM_ELEM(FATAL)
UTILLIB_ENUM_END(logging_level_t)

typedef struct utillib_logging_msg_t {
  /* severity level */
  logging_level_t lmsg_lv;
  /* arbitrary descriptive message */
  char *lmsg_msg;
  /* refcnt __FILE__ str */
  char const *lmsg_file;
  /* refcnt __func__ str */
  char const *lmsg_func;
  /* from __LINE__ */
  size_t lmsg_line;
} utillib_logging_msg_t;

typedef struct utillib_logging_core_t {
  /* the directory to put the logging instead of '/tmp' */
  char const *lco_logdir;
  /* the lowest severity level of logmsg that can go to stderr */
  int lco_stderr_threshold;
  /* the `FILE' that different severity levels of logmsg go to */
  FILE *lco_logfile[logging_level_t_N];
  /* the name of the logging program */
  char const *lco_prog;
  /* the reference counted strings */
  utillib_strref lco_strref;
  /* the collection of all the logmsg */
  utillib_vector lco_msgs;
  /* the cleanup function to call when `FATAL' was logged */
  utillib_destroy_func_t *lco_cleanup;
  /* the object to be cleanupped by `lco_cleanup' */
  void *lco_toclean;
  /* the freelist of the utillib_logging_msg_t */
  utillib_logging_msg_t *lco_free;
} utillib_logging_core_t;

#define UTILLIB_UNREACHABLE(MSG)                                               \
  do {                                                                         \
    static const unsigned control_flow_shouldnot_reach_here = 0;               \
    assert(MSG &&control_flow_shouldnot_reach_here);                           \
  } while (0)

#define UTILLIB_LOG(LEVEL, FMT, ...)                                           \
  utillib_logging_logmsg((LEVEL), __FILE__, __func__, __LINE__, (FMT),         \
                         ##__VA_ARGS__);

void utillib_logging_init(const char *);
void utillib_logging_destroy(void);
void utillib_logging_set_cleanup(void *, utillib_destroy_func_t *);
void utillib_logging_logmsg(int, const char *, const char *, size_t,
                            const char *, ...);
void utillib_logging_set_stderr_threshold(int);
void utillib_logging_set_logdir(const char *);

#endif // UTILLIB_LOGING_H
