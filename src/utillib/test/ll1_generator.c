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

#include <utillib/ll1_generator.h>
#include <utillib/test.h>

#include "ll1_sample_1.h"
#include "non_ll1_sample_1.h"

UTILLIB_TEST_SET_UP() {}

UTILLIB_TEST_TEAR_DOWN() { utillib_ll1_generator_destroy(UT_FIXTURE); }

UTILLIB_TEST(ll1_generator_works) {
  utillib_ll1_generator_init_from_code(UT_FIXTURE, ll1_sample_1_symbols,
                                       ll1_sample_1_rules);
  utillib_ll1_generator_generate(UT_FIXTURE, "sample_1.c");
}

UTILLIB_TEST(ll1_generator_non_ll1_sample) {
  utillib_ll1_generator_init_from_code(UT_FIXTURE, non_ll1_sample_1_symbols,
                                       non_ll1_sample_1_rules);
  utillib_ll1_generator_generate(UT_FIXTURE, "this_should_not_be_generated");
}

UTILLIB_TEST(ll1_generator_dump_set)
{
  utillib_ll1_generator_init_from_code(UT_FIXTURE, ll1_sample_1_symbols,
      ll1_sample_1_rules);
  utillib_ll1_generator_dump_set(UT_FIXTURE, UT_LL1_FIRST, SYM_E);
  utillib_ll1_generator_dump_set(UT_FIXTURE, UT_LL1_FOLLOW, SYM_E);
}

UTILLIB_TEST(ll1_generator_dump_all)
{
  utillib_ll1_generator_init_from_code(UT_FIXTURE, ll1_sample_1_symbols,
      ll1_sample_1_rules);
  utillib_ll1_generator_dump_all(UT_FIXTURE);
}

UTILLIB_TEST_DEFINE(Utillib_LL1Generator) {
  UTILLIB_TEST_BEGIN(Utillib_LL1Generator)
  UTILLIB_TEST_RUN(ll1_generator_works)
  UTILLIB_TEST_RUN(ll1_generator_non_ll1_sample)
  UTILLIB_TEST_RUN(ll1_generator_dump_all)
  UTILLIB_TEST_RUN(ll1_generator_dump_set)
  UTILLIB_TEST_END(Utillib_LL1Generator)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_generator)
  UTILLIB_TEST_RETURN(Utillib_LL1Generator)
}
