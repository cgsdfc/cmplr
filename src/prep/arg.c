#include <utillib/argp.h>

static utillib_argp_error_t parser(int key, char const *arg,
                                   utillib_argp_state *state) {
  static char const *default_val = "1";
  prep_arg_t *self = UTILLIB_ARGP_STATE_INPUT(state);
  switch (key) {
  case 'I':
    utillib_vector_push_back(&self->arg_incl_paths, arg);
    break;
  case 'D':
    /// currently VALUE is not supported
    utillib_vector_push_back(&self->arg_defs, arg);
  default:
    return ARGP_ERR_UNKNOWN;
  }

  static utillib_argp_error_t prep_argp_def(int argc, char **argv,
                                            int *argp_index, void *input) {
    UTILLIB_ARGP_OPTION_BEGIN(prep_options)
    UTILLIB_ARGP_OPTIOIN_ELEM(
        0, 'I', "DIR", 0,
        "each -IDIR specifies a directory to search for include directive", 0)
    UTILLIB_ARGP_OPTIOIN_ELEM(0, 'D', "KEY", 0, "define a macro with optional "
                                                "value as if it was defined by "
                                                "a `#define' directive",
                              0)
    UTILLIB_ARGP_OPTION_END(prep_options)
    UTILLIB_ARGP_BEGIN(prep_argp, &prep_options, prep_parser,
                       "[options] file...", /* args_doc */
                       "C preprocessor",    /* doc */
                       0,                   /* child */
                       0,                   /* domain */
                       )
    UTILLIB_ARGP_END(prep_argp, argc, argv, 0, /* flags */
                     argp_index, input)
  }

  void prep_arg_init(prep_arg_t * self, int argc, char **argv) {
    int argp_index;
    utillib_vector_init(&self->arg_incl_paths);
    utillib_vector_init(&self->arg_defs);
    prep_argp_parse(argc, argv, &argp_index, arg);
  }

  void prep_arg_destroy(prep_arg_t * self) {
    utillib_vector_destroy(&self->arg_incl_paths);
    utillib_vector_destroy(&self->arg_defs);
  }
