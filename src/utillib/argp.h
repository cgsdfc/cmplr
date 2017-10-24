#ifndef UTILLIB_ARGP_H
#define UTILLIB_ARGP_H

/**
 * \file utillib/argp.h
 * Light but convenient wrapper on gnu-c's
 * argp library.
 */
#include <argp.h>
typedef struct argp utillib_argp;
typedef struct argp_option utillib_argp_option;
typedef argp_parser_t utillib_argp_parser_t;
typedef struct argp_child utillib_argp_child;
typedef char *(*utillib_help_filter)(int key, const char *text, void *input);
typedef error_t utillib_argp_error_t;
typedef struct argp_state utillib_argp_state;
#define UTILLIB_ARGP_STATIC_ARGP static_argp
#define UTILLIB_ARGP_STATIC_OPTIONS static_argp_options
#define UTILLIB_ARGP_STATIC_PARSER static_argp_parser
#define UTILLIB_ARGP_PROGRAM_VERSION(VERSION) (argp_program_version=(VERSION));
#define UTILLIB_ARGP_PROGRAM_BUG_ADDRESS(ADDRESS) (argp_program_bug_address=(ADDRESS));

#define UTILLIB_ARGP_OPTION_BEGIN()                                            \
  static const utillib_argp_option UTILLIB_ARGP_STATIC_OPTIONS[] = {

#define UTILLIB_ARGP_OPTION_ELEM(NAME, KEY, ARG, DOC)                          \
  {(NAME), (KEY), (ARG), (0), (DOC), (0)},

#define UTILLIB_ARGP_OPTION_END()                                              \
  UTILLIB_ARGP_OPTION_ELEM(0, 0, 0, 0)                                         \
  }                                                                            \
  ;

#define UTILLIB_ARGP_OPTION_REGISTER(ARG_DOC, DOC)                             \
  static const utillib_argp UTILLIB_ARGP_STATIC_ARGP = {                       \
      .parser = UTILLIB_ARGP_STATIC_PARSER,                                    \
      .options = (UTILLIB_ARGP_STATIC_OPTIONS),                                \
      .args_doc = (ARG_DOC),                                                    \
      .doc = (DOC)};

#define UTILLIB_ARGP_PARSE(ARGC, ARGV, INPUT)                                  \
  argp_parse(&UTILLIB_ARGP_STATIC_ARGP, ARGC, ARGV, 0,0, INPUT)

/* access to argp_state structure */
#define UTILLIB_ARGP_STATE_INPUT(S) ((S)->input)

#endif // UTILLIB_ARGP_H
