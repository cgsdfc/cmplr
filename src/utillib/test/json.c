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
#include <utillib/json.h>
#include <utillib/test.h>

UTILLIB_TEST_AUX(tostring_helper, utillib_json_value_t *val) {
  utillib_string s;
  utillib_string_init(&s);
  utillib_json_tostring(val, &s);
  UTILLIB_TEST_MESSAGE("tostring: `%s'", utillib_string_c_str(&s));
  utillib_json_value_destroy(val);
  utillib_string_destroy(&s);
}


UTILLIB_TEST(json_real_create) {
  double v=3.14;
  utillib_json_value_t * val=utillib_json_real_create(&v, 0);
  UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_REAL);
  UTILLIB_TEST_ASSERT_EQ(val->as_double, v);
  UTILLIB_TEST_MESSAGE("double value is `%lf'", val->as_double);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}
UTILLIB_TEST(json_bool_create) {
  bool b=true;
  utillib_json_value_t * val=utillib_json_bool_create(&b,0);
  UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_BOOL);
  UTILLIB_TEST_ASSERT_EQ(val->as_bool, b);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper,val);

}
UTILLIB_TEST(json_long_create) {}
UTILLIB_TEST(json_string_create) {}
UTILLIB_TEST(json_real_array_create){}
UTILLIB_TEST(json_bool_array_create){}
UTILLIB_TEST(json_long_array_create){}
UTILLIB_TEST(json_string_array_create) {}
UTILLIB_TEST(json_value_create) {}
UTILLIB_TEST(json_value_createV) {}
UTILLIB_TEST(json_array_create) {}
UTILLIB_TEST(json_object_create) {}
UTILLIB_TEST(json_value_destroy) {}
UTILLIB_TEST(json_null_create) {}
UTILLIB_TEST(json_null_array_create) {}
UTILLIB_TEST(json_tostring) {}

/* UTILLIB_TEST() {} */
/* UTILLIB_TEST() {} */
/* UTILLIB_TEST() {} */
/* UTILLIB_TEST() {} */
/* UTILLIB_TEST() {} */

UTILLIB_TEST_DEFINE(Utillib_JSON) {
  UTILLIB_TEST_BEGIN(Utillib_JSON)
  UTILLIB_TEST_RUN(json_real_create) 
  UTILLIB_TEST_RUN(json_bool_create) 
  UTILLIB_TEST_RUN(json_long_create) 
  UTILLIB_TEST_RUN(json_string_create) 
  UTILLIB_TEST_RUN(json_real_array_create)
  UTILLIB_TEST_RUN(json_bool_array_create)
  UTILLIB_TEST_RUN(json_long_array_create)
  UTILLIB_TEST_RUN(json_string_array_create) 
  UTILLIB_TEST_RUN(json_value_create) 
  UTILLIB_TEST_RUN(json_value_createV) 
  UTILLIB_TEST_RUN(json_array_create) 
  UTILLIB_TEST_RUN(json_object_create) 
  UTILLIB_TEST_RUN(json_value_destroy) 
  UTILLIB_TEST_RUN(json_null_create) 
  UTILLIB_TEST_RUN(json_null_array_create) 
  UTILLIB_TEST_RUN(json_tostring) 
  UTILLIB_TEST_END(Utillib_JSON)
  UTILLIB_TEST_RETURN(Utillib_JSON)
}
