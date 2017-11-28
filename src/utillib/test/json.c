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
#include <utillib/json_foreach.h>
#include <utillib/test.h>
#include <utillib/vector.h>

struct Point {
  double x, y;
};

struct Line {
  struct Point begin;
  struct Point end;
};

struct Student {
  long id;
  char const *name;
  double gpa;
};

UTILLIB_JSON_OBJECT_FIELD_BEGIN(Student_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Student, "ID", id,
                               utillib_json_long_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Student, "Name", name,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Student, "GPA", gpa,
                               utillib_json_real_create)
UTILLIB_JSON_OBJECT_FIELD_END(Student_Fields)

UTILLIB_JSON_OBJECT_FIELD_BEGIN(Point_JSON)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Point, "X", x, utillib_json_real_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Point, "Y", y, utillib_json_real_create)
UTILLIB_JSON_OBJECT_FIELD_END(Point_JSON)

struct Student John = {.id = 12, .name = "John", .gpa = 2.70};
struct Student students[] = {{.name = "John", .id = 12, .gpa = 3.2},
                             {.name = "Tom", .id = 13, .gpa = 3.3},
                             {.name = "Amy", .id = 14, .gpa = 1.2},
                             {.name = "Wuyifan", .id = 10, .gpa = 2.5}};

/**
 * \function tostring_helper
 * Since it destroys the passed-in val,
 * it must be called in the end of the testing logic.
 */
UTILLIB_TEST_AUX(tostring_helper, struct utillib_json_value *val) {
  struct utillib_string s;
  utillib_json_tostring(val, &s);
  UTILLIB_TEST_MESSAGE("tostring: `%s'", utillib_string_c_str(&s));
  utillib_json_value_destroy(val);
  utillib_string_destroy(&s);
}

UTILLIB_TEST(json_real_create) {
  double v = 3.14;
  struct utillib_json_value *val = utillib_json_real_create(&v);
  UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_REAL);
  UTILLIB_TEST_ASSERT_EQ(val->as_double, v);
  UTILLIB_TEST_MESSAGE("double value is `%lf'", val->as_double);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_bool_create) {
  bool b = true;
  struct utillib_json_value *val = utillib_json_bool_create(&b);
  UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_BOOL);
  UTILLIB_TEST_ASSERT_EQ(val, &utillib_json_true);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_long_create) {
  long longs[] = {0, 0x001, 0x003, -100, 100, 0127};
  size_t LEN = UTILLIB_TEST_LEN(longs);

  for (int i = 0; i < LEN; ++i) {
    struct utillib_json_value *val = utillib_json_long_create(&longs[i]);
    UTILLIB_TEST_ASSERT_EQ(longs[i], val->as_long);
    UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_LONG);
    UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
  }
}

UTILLIB_TEST(json_string_create) {
  char const *strings[] = {"C++", "C",    "Java", "Python",
                           "PHP", "Ruby", "Perl", "Go"};
  size_t LEN = UTILLIB_TEST_LEN(strings);

  for (int i = 0; i < LEN; ++i) {
    struct utillib_json_value *val = utillib_json_string_create(&strings[i]);
    UTILLIB_TEST_ASSERT_STREQ(strings[i], val->as_ptr);
    UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_STRING);
    UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
  }
}

UTILLIB_TEST(json_real_array_create) {
  double reals[] = {3.1, 3.14, 3.149, 3.1492, 3.14926};
  struct utillib_json_array_desc desc;
  utillib_json_array_desc_init(&desc, sizeof(double), UTILLIB_TEST_LEN(reals),
                               utillib_json_real_create);
  struct utillib_json_value *val = utillib_json_array_create(reals, &desc);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_bool_array_create) {
  bool bools[] = {true, true, false, false};
  struct utillib_json_array_desc desc;
  utillib_json_array_desc_init(&desc, sizeof(bool), UTILLIB_TEST_LEN(bools),
                               utillib_json_bool_create);
  struct utillib_json_value *val = utillib_json_array_create(bools, &desc);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_string_array_create) {
  char const *strings[] = {"C++", "C",    "Java", "Python",
                           "PHP", "Ruby", "Perl", "Go"};
  struct utillib_json_array_desc desc;
  utillib_json_array_desc_init(&desc, sizeof(char const *),
                               UTILLIB_TEST_LEN(strings),
                               utillib_json_string_create);
  struct utillib_json_value *val = utillib_json_array_create(strings, &desc);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_object_create) {
  struct utillib_json_value *val =
      utillib_json_object_create(&John, Student_Fields);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

static struct utillib_json_value *Point_JSON_create(void const *base) {
  return utillib_json_object_create(base, Point_JSON);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(Line_JSON)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Line, "From", begin, Point_JSON_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Line, "To", end, Point_JSON_create)
UTILLIB_JSON_OBJECT_FIELD_END(Line_JSON)

UTILLIB_TEST(json_object_create_nested_object) {
  struct Point A = {.x = 2.11, .y = 3.14};
  struct utillib_json_value *A_val = Point_JSON_create(&A);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, A_val);

  struct Line L = {{1.0, 2.0}, {3.4, 3.2}};
  struct utillib_json_value *L_val = utillib_json_object_create(&L, Line_JSON);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, L_val);
}

UTILLIB_TEST(json_null_array_create) {
  void *null_array[] = {NULL, NULL};
  struct utillib_json_value *val =
      utillib_json_null_array_create(UTILLIB_TEST_LEN(null_array));
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

struct utillib_json_value *Student_Create(void const *base) {
  return utillib_json_object_create(base, Student_Fields);
}

UTILLIB_TEST(json_array_create) {
  struct utillib_json_array_desc desc;
  utillib_json_array_desc_init(&desc, sizeof students[0],
                               UTILLIB_TEST_LEN(students), Student_Create);
  struct utillib_json_value *val = utillib_json_array_create(students, &desc);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_array_foreach) {
  struct utillib_json_value * array=utillib_json_array_create_empty();
  UTILLIB_JSON_ARRAY_FOREACH(val, array) {
    utillib_json_pretty_print(val, stderr);
  }
  utillib_json_array_push_back(array, utillib_json_null_create());
  utillib_json_array_push_back(array, utillib_json_null_create());
  utillib_json_array_push_back(array, utillib_json_null_create());
  utillib_json_array_push_back(array, utillib_json_null_create());
  UTILLIB_JSON_ARRAY_FOREACH(val_, array) {
    utillib_json_pretty_print(val_, stderr);
  }
  utillib_json_value_destroy(array);
}

UTILLIB_TEST(json_object_foreach) {
  struct utillib_json_value * object=utillib_json_object_create_empty();
  UTILLIB_JSON_OBJECT_FOREACH(pair_, object) {
    puts(pair_->up_first);
    utillib_json_pretty_print(pair_->up_second, stderr);
  }
  bool _false=false;
  utillib_json_object_push_back(object, "key1", 
      utillib_json_bool_create(&_false));
  _false=true;
  utillib_json_object_push_back(object, "key2",
      utillib_json_bool_create(&_false));
  utillib_json_object_push_back(object, "key3",
      utillib_json_null_create());
  UTILLIB_JSON_OBJECT_FOREACH(pair, object) {
    puts(pair->up_first);
    utillib_json_pretty_print(pair->up_second, stderr);
  }
}
UTILLIB_TEST_DEFINE(Utillib_JSON) {
  UTILLIB_TEST_BEGIN(Utillib_JSON)
  UTILLIB_TEST_RUN(json_array_foreach)
  UTILLIB_TEST_RUN(json_object_foreach)
  UTILLIB_TEST_RUN(json_real_create)
  UTILLIB_TEST_RUN(json_bool_create)
  UTILLIB_TEST_RUN(json_long_create)
  UTILLIB_TEST_RUN(json_string_create)
  UTILLIB_TEST_RUN(json_real_array_create)
  UTILLIB_TEST_RUN(json_bool_array_create)
  UTILLIB_TEST_RUN(json_string_array_create)
  UTILLIB_TEST_RUN(json_array_create)
  UTILLIB_TEST_RUN(json_object_create)
  UTILLIB_TEST_RUN(json_object_create_nested_object)
  UTILLIB_TEST_RUN(json_null_array_create)
  UTILLIB_TEST_END(Utillib_JSON)
  UTILLIB_TEST_RETURN(Utillib_JSON)
}
