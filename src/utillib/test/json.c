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
#include <utillib/vector.h>

struct Point { double x, y; } ;

struct Line {
  struct Point begin;
  struct Point end;
} ;

struct Student {
  long id;
  char const *name;
  double gpa;
} ;

UTILLIB_JSON_OBJECT_FIELD_BEGIN(Student_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Student, "ID", id, utillib_json_long_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Student, "Name", name,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Student, "GPA", gpa, utillib_json_real_create)
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
UTILLIB_TEST_AUX(tostring_helper, struct utillib_json_value_t *val) {
  struct utillib_string s;
  utillib_json_tostring(val, &s);
  UTILLIB_TEST_MESSAGE("tostring: `%s'", utillib_string_c_str(&s));
  utillib_json_value_destroy(val);
  utillib_string_destroy(&s);
}

UTILLIB_TEST(json_real_create) {
  double v = 3.14;
  struct utillib_json_value_t *val = utillib_json_real_create(&v, 0);
  UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_REAL);
  UTILLIB_TEST_ASSERT_EQ(val->as_double, v);
  UTILLIB_TEST_MESSAGE("double value is `%lf'", val->as_double);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_bool_create) {
  for (int i = 0; i < 2; ++i) {
    bool b = i;
    struct utillib_json_value_t *val = utillib_json_bool_create(&b, 0);
    UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_BOOL);
    UTILLIB_TEST_ASSERT_EQ(val->as_bool, b);
    UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
  }
}

UTILLIB_TEST(json_long_create) {
  long longs[] = {0, 0x001, 0x003, -100, 100, 0127};
  size_t LEN = UTILLIB_TEST_LEN(longs);

  for (int i = 0; i < LEN; ++i) {
    struct utillib_json_value_t *val = utillib_json_long_create(&longs[i], 0);
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
    struct utillib_json_value_t *val = utillib_json_string_create(&strings[i], 0);
    UTILLIB_TEST_ASSERT_STREQ(strings[i], val->as_ptr);
    UTILLIB_TEST_ASSERT_EQ(val->kind, UT_JSON_STRING);
    UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
  }
}

UTILLIB_TEST(json_real_array_create) {
  double reals[] = {3.1, 3.14, 3.149, 3.1492, 3.14926};
  struct utillib_json_value_t *val =
      utillib_json_real_array_create(reals, sizeof reals);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_bool_array_create) {
  bool bools[] = {true, true, false, false};
  struct utillib_json_value_t *val =
      utillib_json_bool_array_create(bools, sizeof bools);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_long_array_create) {
  long longs[] = {1, 2, 3, 4};
  struct utillib_json_value_t *val =
      utillib_json_long_array_create(longs, sizeof longs);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_string_array_create) {
  char const *strings[] = {"C++", "C",    "Java", "Python",
                           "PHP", "Ruby", "Perl", "Go"};
  struct utillib_json_value_t *val =
      utillib_json_string_array_create(strings, sizeof strings);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_object_create) {
  struct utillib_json_value_t *val =
      utillib_json_object_create(&John, sizeof John, Student_Fields);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

static struct utillib_json_value_t *Point_JSON_create(void const *base,
                                               size_t offset) {
  return utillib_json_object_create(base, offset, Point_JSON);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(Line_JSON)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Line, "From", begin, Point_JSON_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Line, "To", end, Point_JSON_create)
UTILLIB_JSON_OBJECT_FIELD_END(Line_JSON)

UTILLIB_TEST(json_object_create_nested_object) {
  struct Point A = {.x = 2.11, .y = 3.14};
  struct utillib_json_value_t *A_val = Point_JSON_create(&A, sizeof A);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, A_val);

  struct Line L = {{1.0, 2.0}, {3.4, 3.2}};
  struct utillib_json_value_t *L_val =
      utillib_json_object_create(&L, sizeof L, Line_JSON);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, L_val);
}

UTILLIB_TEST(json_null_create) {
  struct utillib_json_value_t *null = utillib_json_null_create();
  UTILLIB_TEST_ASSERT_EQ(null->kind, UT_JSON_NULL);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, null);
}

UTILLIB_TEST(json_null_array_create) {
  void *null_array[] = {NULL, NULL};
  struct utillib_json_value_t *val =
      utillib_json_null_array_create(UTILLIB_TEST_LEN(null_array));
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

struct utillib_json_value_t *Student_Create(void const *base, size_t offset) {
  return utillib_json_object_create(base, offset, Student_Fields);
}

/**
 * Arrays of customized element type.
 */
UTILLIB_JSON_ARRAY_DESC(Student_Desc, sizeof(struct Student), Student_Create);

UTILLIB_TEST(json_array_create) {
  struct utillib_json_value_t *val =
      utillib_json_array_create(students, sizeof students, &Student_Desc);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_value_create) {
  struct utillib_json_value_t *val;
  double dv = 31.1111;
  val = utillib_json_value_create(UT_JSON_REAL, &dv);
  UTILLIB_TEST_ASSERT_EQ(val->as_double, dv);
  utillib_json_value_destroy(val);
  long lv = 100000000;
  val = utillib_json_value_create(UT_JSON_LONG, &lv);
  UTILLIB_TEST_ASSERT_EQ(val->as_long, lv);
  utillib_json_value_destroy(val);
  bool bv = true;
  val = utillib_json_value_create(UT_JSON_BOOL, &bv);
  UTILLIB_TEST_ASSERT_EQ(val->as_bool, bv);
  utillib_json_value_destroy(val);
  char const *sv = "string";
  val = utillib_json_value_create(UT_JSON_STRING, &sv);
  UTILLIB_TEST_ASSERT_STREQ(sv, val->as_str);
  utillib_json_value_destroy(val);
  // Using `students'.

  val = utillib_json_value_create(UT_JSON_ARRAY, students, sizeof students,
                                  &Student_Desc);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);

  val = utillib_json_value_create(UT_JSON_OBJECT, &John, sizeof John,
                                  Student_Fields);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_array_in_object) {
  struct Real_Array {
    double array[5];
    long size;
  } ;
  UTILLIB_JSON_OBJECT_FIELD_BEGIN(Real_Array_Fields)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Real_Array, "array", array,
                                 utillib_json_real_array_create)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct Real_Array, "size", size,
                                 utillib_json_long_create)
  UTILLIB_JSON_OBJECT_FIELD_END(Real_Array_Fields)

  struct Real_Array reals = {{0, 1, 2, 3, 4}, 5};
  struct utillib_json_value_t *val =
      utillib_json_object_create(&reals, sizeof reals, Real_Array_Fields);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_JSON_ARRAY_DESC(long_array2_desc, sizeof(long[2]),
                        utillib_json_long_array_create);
struct utillib_json_value_t *json_long_array2_create(void const *base, size_t size) {
  return utillib_json_array_create(base, size, &long_array2_desc);
}

UTILLIB_TEST(json_ndarray) {
  long array2[][2] = {{0, 1}, {1, 0}};
  struct utillib_json_value_t *val = json_long_array2_create(array2, sizeof array2);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

struct Boolean_Array {
  bool *barr;
  size_t size;
} ;

static struct utillib_json_value_t *status_string_create(void const *base,
                                                  size_t offset) {
  int status = *(int *)base;
  char const *str = utillib_test_status_kind_tostring(status);
  return utillib_json_string_create(&str, 0);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(TestEntry_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_entry_t, "test_name", func_name,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_entry_t, "status", status,
                               status_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_entry_t, "succeeded", succeeded,
                               utillib_json_bool_create)
UTILLIB_JSON_OBJECT_FIELD_END(TestEntry_Fields)

static struct utillib_json_value_t *json_test_entry_create(void const *base,
                                                    size_t offset) {
  return utillib_json_object_create(base, offset, TestEntry_Fields);
}

UTILLIB_JSON_ARRAY_DESC(TestEntry_ArrayDesc, sizeof(struct utillib_test_entry_t),
                        json_test_entry_create);

/**
 * \function json_test_entry_array_pointer_create
 * Creates the JSON array of `struct utillib_test_entry_t'
 * from the fields `ntests' and `cases' of a
 * `struct utillib_test_env_t' struct.
 * \param base Pointer to the `cases' field of the
 * `struct utillib_test_env_t' struct.
 * \param offset Useless.
 */

static struct utillib_json_value_t *
json_test_entry_array_pointer_create(void const *base, size_t offset) {
  size_t offsetof_base = offsetof(struct utillib_test_env_t, cases);
  size_t offsetof_size = offsetof(struct utillib_test_env_t, ntests);
  /* hack out the address of the field `ntests' from 2 offsets */
  void *psize = (char *)base - offsetof_base + offsetof_size;
  return utillib_json_array_pointer_create(*(void **)base, *(size_t *)psize,
                                           &TestEntry_ArrayDesc);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(TestEnv_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "filename", filename,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "case_name", case_name,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "number_tests", ntests,
                               utillib_json_long_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "number_run", nrun,
                               utillib_json_long_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "number_skipped", nskipped,
                               utillib_json_long_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "number_passed", nsuccess,
                               utillib_json_long_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "number_failed", nfailure,
                               utillib_json_long_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_env_t, "tests", cases,
                               json_test_entry_array_pointer_create)
UTILLIB_JSON_OBJECT_FIELD_END(TestEnv_Fields)

static struct utillib_json_value_t *json_test_env_create(void const *base,
                                                  size_t offset) {
  return utillib_json_object_create(base, offset, TestEnv_Fields);
}

static struct utillib_json_value_t *json_test_suite_test_create(void const *base,
                                                         size_t offset) {
  return utillib_json_array_create_from_vector(base, json_test_env_create);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(TestSuite_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_suite_t, "filename", filename,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_test_suite_t, "tests", tests,
                               json_test_suite_test_create)
UTILLIB_JSON_OBJECT_FIELD_END(TestSuite_Fields)

UTILLIB_TEST(test_entry_create) {
  struct utillib_test_entry_t entry = {.func_name = "func_name",
                                .assert_failure = 1,
                                .expect_failure = 3,
                                .abort_failure = 1,
                                .succeeded = false};
  struct utillib_json_value_t *val =
      utillib_json_object_create(&entry, sizeof entry, TestEntry_Fields);
  UTILLIB_TEST_AUX_INVOKE(tostring_helper, val);
}

UTILLIB_TEST(json_array_create_empty) {}

UTILLIB_TEST(json_object_pointer) {}

UTILLIB_TEST(json_tostring) {}

UTILLIB_TEST(json_value_destroy) {}

UTILLIB_TEST_DEFINE(Utillib_JSON) {
  UTILLIB_TEST_BEGIN(Utillib_JSON)
  UTILLIB_TEST_RUN(test_entry_create)
  UTILLIB_TEST_RUN(json_real_create)
  UTILLIB_TEST_RUN(json_bool_create)
  UTILLIB_TEST_RUN(json_long_create)
  UTILLIB_TEST_RUN(json_string_create)
  UTILLIB_TEST_RUN(json_real_array_create)
  UTILLIB_TEST_RUN(json_bool_array_create)
  UTILLIB_TEST_RUN(json_long_array_create)
  UTILLIB_TEST_RUN(json_string_array_create)
  UTILLIB_TEST_RUN(json_value_create)
  UTILLIB_TEST_RUN(json_array_create)
  UTILLIB_TEST_RUN(json_object_create)
  UTILLIB_TEST_RUN(json_object_create_nested_object)
  UTILLIB_TEST_RUN(json_value_destroy)
  UTILLIB_TEST_RUN(json_null_create)
  UTILLIB_TEST_RUN(json_array_in_object)
  UTILLIB_TEST_RUN(json_null_array_create)
  UTILLIB_TEST_RUN(json_tostring)
  UTILLIB_TEST_RUN(json_ndarray)
  UTILLIB_TEST_END(Utillib_JSON)
  UTILLIB_TEST_RETURN(Utillib_JSON)
}
