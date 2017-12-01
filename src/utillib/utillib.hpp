#ifndef UTILLIB_UTILLIB_HPP
#define UTILLIB_UTILLIB_HPP
extern "C" {
#include <utillib/hashmap.h>
#include <utillib/json.h>
#include <utillib/json_foreach.h>
#include <utillib/slist.h>
#include <utillib/string.h>
}

namespace utillib {
class json_value {
private:
  struct utillib_json_value *self;

public:
  explicit json_value(struct utillib_json_value *val);
  json_value();
  ~json_value();
  void destroy();
  void array_push_back(json_value element);
  void object_push_back(char const *key, json_value value);
  bool is_null() const;
  json_value operator[](char const *key);
  json_value operator[](size_t index);
};
template <class T> json_value json_value_create(T val);
json_value json_array_create();
json_value json_object_create();

template <> inline json_value json_value_create<int>(int val) {
  return json_value(utillib_json_int_create(&val));
}

template <> inline json_value json_value_create<long>(long val) {
  return json_value(utillib_json_long_create(&val));
}

template <> inline json_value json_value_create<size_t>(size_t val) {
  return json_value(utillib_json_size_t_create(&val));
}

template <> inline json_value json_value_create<const char *>(const char *val) {
  return json_value(utillib_json_string_create(&val));
}

template <> inline json_value json_value_create<float>(float val) {
  return json_value(utillib_json_float_create(&val));
}

template <> inline json_value json_value_create<double>(double val) {
  return json_value(utillib_json_real_create(&val));
}

class string {
private:
  struct utillib_string self;

public:
  string();
  void destroy();
  explicit string(char const *str);
  ~string();
  char const *c_str();
  void append(char const *str);
  void append_char(char ch);
  size_t size() const;
  size_t capacity() const;
  bool empty() const;
  void clear();
  void reserve(size_t size);
};

class hashmap {
private:
  struct utillib_hashmap self;

public:
  hashmap(struct utillib_hashmap_callback const &callback);
  ~hashmap();
  void destroy();
  void destroy_owning(void (*key_destroy)(void *key),
                      void (*value_destroy)(void *value));
  void rehash();
  int insert(void const *key, void const *value);
  void *update(void const *key, void const *value);
  void *discard(void const *key);
  bool exist_key(void const *key) const;
  void *operator[](void const *key);
  size_t size() const;
  bool empty() const;
  size_t buckets_size() const;
  json_value tojson(utillib_json_value_create_func_t value_create) const;
  json_value tojson(utillib_json_value_create_func_t key_create,
                    utillib_json_value_create_func_t value_create) const;
};

class vector {
private:
  struct utillib_vector self;

public:
  vector();
  ~vector();
  void destroy();
  void destroy_owning(void (*destroy)(void *value));
  void push_back(void const *val);
  void pop_back();
  void *back();
  void *front();
  size_t size() const;
  json_value tojson(utillib_json_value_create_func_t value_create) const;
};

} // utillib

#endif // UTILLIB_UTILLIB_HPP
