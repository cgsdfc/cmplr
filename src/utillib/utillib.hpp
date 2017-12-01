#ifndef UTILLIB_UTILLIB_HPP
#define UTILLIB_UTILLIB_HPP
extern "C" {
#include <utillib/bitset.h>
#include <utillib/enum.h>
#include <utillib/hashmap.h>
#include <utillib/json.h>
#include <utillib/json_foreach.h>
#include <utillib/json_parser.h>
#include <utillib/slist.h>
#include <utillib/string.h>
}

namespace utillib {
class string;

class json_value {
private:
  struct utillib_json_value *self;

public:
  explicit json_value(struct utillib_json_value *val);
  json_value();
  ~json_value(){};
  void destroy();
  void push_back(json_value element);
  void push_back(char const *key, json_value value);
  bool is_null() const;
  json_value member(const char *key);
  json_value element(size_t index);
  void pretty_print() const;
  string tostring() const;
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
public:
  struct utillib_string self;
  string();
  void destroy();
  explicit string(char const *str);
  ~string() {}
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
  ~hashmap() {}
  void destroy();
  void destroy_owning(void (*key_destroy)(void *key),
                      void (*value_destroy)(void *value));
  void rehash();
  int insert(void const *key, void const *value);
  void *update(void const *key, void const *value);
  void *discard(void const *key);
  bool exist_key(void const *key) const;
  void *at(void const *key);
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
  ~vector() {}
  void destroy();
  void destroy_owning(void (*destroy)(void *value));
  void push_back(void const *val);
  void pop_back();
  void *back();
  void *front();
  void *at(size_t index);
  size_t size() const;
  size_t capacity() const;
  template <class ValueCreate>
  json_value tojson(ValueCreate value_create) const {
    return json_value(utillib_vector_json_array_create(
        &self,
        reinterpret_cast<utillib_json_value_create_func_t>(value_create)));
  }
};

class slist {
private:
  struct utillib_slist self;

public:
  slist();
  ~slist() {}
  void destroy();
  void destroy_owning(void (*destroy)(void *value));
  void push_front(void const *value);
  void *front();
  void erase(size_t index);
  bool empty() const;
  size_t size() const;
  json_value tojson(utillib_json_value_create_func_t value_create) const;
};

class bitset {
private:
  struct utillib_bitset self;

public:
  bitset(size_t N);
  ~bitset() {}
  void destroy();
  void insert(size_t pos);
  void remove(size_t pos);
  bool contains(size_t pos) const;
  void merge(bitset const &rhs);
  bool equal(bitset const &rhs) const;
  bool is_intersect(bitset const &rhs) const;

  bool insert_updated(size_t pos);
  bool remove_updated(size_t pos);
  bool merge_updated(bitset const &rhs);
  json_value tojson() const;
};

class ll1_factory {
public:
  struct utillib_ll1_factory self;
  ll1_factory(int const *gentable, struct utillib_symbol const *symbols,
              struct utillib_rule_literal const *rules);
  ~ll1_factory();
};

class json_parser {
private:
  struct utillib_json_parser self;
  struct factory {
    struct utillib_ll1_factory self;
    factory() { utillib_json_parser_factory_init(&self); }
    ~factory() { utillib_ll1_factory_destroy(&self); }
  };

public:
  json_parser();
  ~json_parser() {}
  void destroy();
  json_value parse(char const *str);
};

} // utillib

#endif // UTILLIB_UTILLIB_HPP
