#include <utillib/utillib.hpp>
#include <cstddef>
#include <new>

using namespace utillib;

json_value::json_value(struct utillib_json_value * val):val_(val) {}
json_value::~json_value() { }
json_value::json_value():val_(utillib_json_null_create()) {}
void json_value::destroy() { utillib_json_value_destroy(val_); }
bool json_value::is_null() const { return val_ == &utillib_json_null; }
void json_value::array_push_back(json_value element)
{
  utillib_json_array_push_back(val_, element.val_);
}
void json_value::object_push_back(char const *key, json_value value)
{
  utillib_json_object_push_back(val_, key, value.val_);
}
json_value json_value::operator[](char const *key)
{
  auto val=utillib_json_object_at(val_, key);
  return val == NULL ? json_value() : json_value(val);
}

json_value json_value::operator[](std::size_t index)
{
  auto val=utillib_json_array_at(val_, index);
  return val == NULL ? json_value() : json_value(val);
}

json_value json_object_create()
{
  auto val=utillib_json_object_create_empty();
  return json_value(val);
}

json_value json_array_create()
{
  auto val=utillib_json_array_create_empty();
  return json_value(val);
}

