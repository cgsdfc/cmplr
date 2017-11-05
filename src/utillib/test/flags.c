/* 
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
   02110-1301 USA

*/
#include <utillib/flags.h>
#include <utillib/test.h>

typedef struct test_arg_t {
  char const *foo_arg;
  char const *bar_arg;
} test_arg_t;

static utillib_argp_error_t
UTILLIB_ARGP_STATIC_PARSER(int key, char *arg, utillib_argp_state *state) {
  test_arg_t *self = UTILLIB_ARGP_STATE_INPUT(state);
  switch (key) {
  case 'f':
    self->foo_arg = arg;
    puts("options f was seen");
    break;
  case 'b':
    self->bar_arg = arg;
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

UTILLIB_ARGP_OPTION_REGISTER("[FILE...]", "test the utillib/argp.h")

UTILLIB_TEST(argp_main) 
{
  static char * argv[] = { "argp_main", "--bar=a.c", "--foo=./dir", NULL };
  static const int argc = UTILLIB_TEST_LEN(argv)-1;
  test_arg_t arg = {0};
  UTILLIB_ARGP_PARSE(argc, argv, &arg);
  UTILLIB_TEST_MESSAGE("--bar=%s, --foo=%s\n", arg.bar_arg, arg.foo_arg);
  UTILLIB_TEST_EXPECT_STREQ(arg.bar_arg, "a.c");
  UTILLIB_TEST_EXPECT_STREQ(arg.foo_arg, "./dir");

}

UTILLIB_TEST_DEFINE(Utillib_Flags) {
  UTILLIB_TEST_BEGIN(Utillib_Flags)
  UTILLIB_TEST_RUN(argp_main)
  UTILLIB_TEST_END(Utillib_Flags)
  UTILLIB_TEST_RETURN(Utillib_Flags);
}

