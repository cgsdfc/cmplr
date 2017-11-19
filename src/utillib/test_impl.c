#include "test_impl.h"

/**
 * Strings associated with `utillib_test_severity_kind'.
 */
UTILLIB_ETAB_BEGIN(utillib_test_severity_kind)
UTILLIB_ETAB_ELEM_INIT(US_EXPECT, "Expected")
UTILLIB_ETAB_ELEM_INIT(US_ASSERT, "Assertion Failed")
UTILLIB_ETAB_ELEM_INIT(US_ABORT, "Abort")
UTILLIB_ETAB_END(utillib_test_severity_kind)

UTILLIB_ETAB_BEGIN(utillib_test_status_kind)
UTILLIB_ETAB_ELEM_INIT(UT_STATUS_SKIP, "skipped")
UTILLIB_ETAB_ELEM_INIT(UT_STATUS_RUN, "run")
UTILLIB_ETAB_END(utillib_test_status_kind)
/*
 * JSON interface
 */

/**
 * \function json_test_entry_create
 * Creates JSON value from `struct utillib_test_entry_t'.
 */
static struct utillib_json_value_t const*
test_entry_json_object_create(void const *_self)
{
  struct utillib_test_entry_t const *self=_self;
  struct utillib_json_value_t * object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "name", 
      utillib_json_string_create(self->func_name));
  utillib_json_object_push_back(object, "status",
      utillib_json_string_create(utillib_test_status_kind_tostring(self->status)));
  utillib_json_object_push_back(object, "result",
      utillib_json_bool_create(&self->succeeded));
  return object;
}

/**
 * \function json_test_entry_array_pointer_create
 * Creates the JSON array of `struct utillib_test_entry_t'
 */

static struct utillib_json_value_t const*
test_entry_json_array_create(struct utillib_test_env_t const *self)
{
  struct utillib_json_value_t *array=utillib_json_array_create_empty();
  for (struct utillib_test_entry_t const * item=self->cases;
      item->func!=NULL; ++item) {
    utillib_json_array_push_back(array, 
        test_entry_json_object_create(item));
  }
  return array;
}

/**
 * \function json_test_env_create
 * Wraps `TestEnv_Fields'.
 */
static struct utillib_json_value_t const*
test_env_json_object_create(struct utillib_test_env_t const *self)
{
  struct utillib_json_value_t *object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "filename",
      utillib_json_string_create(self->filename));
  utillib_json_object_push_back(object, "name",
      utillib_json_string_create(self->case_name));
  utillib_json_object_push_back(object, "ntests",
      utillib_json_long_create(&self->ntests));
  utillib_json_object_push_back(object, "nskipped",
      utillib_json_long_create(&self->nskipped));
  utillib_json_object_push_back(object, "nrun",
      utillib_json_long_create(&self->nrun));
  utillib_json_object_push_back(object, "npassed",
      utillib_json_long_create(&self->nsuccess));
  utillib_json_object_push_back(object, "nfailed",
      utillib_json_long_create(&self->nfailure));
  utillib_json_object_push_back(object, "details",
      test_entry_json_array_create(self));
  return object;
}

struct utillib_json_value_t const *
utillib_test_suite_json_object_create(struct utillib_test_suite_t const *self)
{
  struct utillib_json_value_t * object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "filename",
      utillib_json_string_create(self->filename));
  utillib_json_object_push_back(object, "tests",
      utillib_vector_json_array_create(&self->tests, test_entry_json_object_create));
  return object;
}
