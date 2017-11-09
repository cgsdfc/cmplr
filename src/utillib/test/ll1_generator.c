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
#define UT_TEST_LL1_SAMPLE_1

#ifdef UT_TEST_LL1_SAMPLE_1
#include "ll1_sample_1.h"
#include "non_ll1_sample_1.h"
#elif UT_TEST_LL1_SAMPLE_2
#include "ll1_sample_2.h"
#include "non_ll1_sample_2.h"
#endif

UTILLIB_TEST_SET_UP() {}

UTILLIB_TEST_TEAR_DOWN() { utillib_ll1_generator_destroy(UT_FIXTURE); }

UTILLIB_TEST(ll1_generator_works) {
  utillib_ll1_generator_init_from_code(UT_FIXTURE, ll1_sample_1_symbols,
                                       ll1_sample_1_rules);
  utillib_ll1_generator_generate(UT_FIXTURE, "sample_1.c");
}

#ifdef UT_TEST_LL1_SAMPLE_1
UTILLIB_TEST(ll1_generator_non_ll1_sample) {
  utillib_ll1_generator_init_from_code(UT_FIXTURE, non_ll1_sample_1_symbols,
                                       non_ll1_sample_1_rules);
  utillib_ll1_generator_generate(UT_FIXTURE, "this_should_not_be_generated");
}
#elif defined(UT_TEST_LL1_SAMPLE_2)
UTILLIB_TEST(ll1_generator_non_ll1_sample_2) {
  utillib_ll1_generator_init_from_code(UT_FIXTURE, non_ll1_sample_2_symbols,
                                       non_ll1_sample_2_rules);
  utillib_ll1_generator_generate(UT_FIXTURE, "this_should_not_be_generated");
}
#endif

UTILLIB_TEST_DEFINE(Utillib_LL1Generator) {
  UTILLIB_TEST_BEGIN(Utillib_LL1Generator)
  UTILLIB_TEST_RUN(ll1_generator_works)
  UTILLIB_TEST_RUN(ll1_generator_non_ll1_sample)
  UTILLIB_TEST_END(Utillib_LL1Generator)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_generator)
  UTILLIB_TEST_RETURN(Utillib_LL1Generator)
}
