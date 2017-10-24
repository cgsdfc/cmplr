#include <utillib/argp.h>
typedef struct test_arg_t{
  char const * foo_arg;
  char const * bar_arg;
} test_arg_t;

static utillib_argp_error_t 
UTILLIB_ARGP_STATIC_PARSER(int key , char *arg, utillib_argp_state *state) {
  test_arg_t * self=UTILLIB_ARGP_STATE_INPUT(state);
 switch(key) {
   case 'f':
     self->foo_arg=arg;
     puts("options f was seen");
     break;
   case 'b':
     self->bar_arg=arg;
     puts("options b was seen");
     break;
   default:
     return ARGP_ERR_UNKNOWN;
 }
 return 0;
}

UTILLIB_ARGP_OPTION_BEGIN()
  UTILLIB_ARGP_OPTION_ELEM("foo", 'f', "FILE", "options foo")
  UTILLIB_ARGP_OPTION_ELEM("bar", 'b', "DIR", "options bar")
UTILLIB_ARGP_OPTION_END()

UTILLIB_ARGP_OPTION_REGISTER( 
    "[FILE...]",
    "test the utillib/argp.h"
)

UTILLIB_ARGP_PROGRAM_VERSO

int main(int argc, char **argv) {
  test_arg_t arg;
  UTILLIB_ARGP_PARSE(argc, argv, &arg);
  printf("--bar=%s, --foo=%s\n", arg.bar_arg, arg.foo_arg);
}



