#include <utillib/utillib.hpp>

using namespace utillib;

//
// utillib.json_value
//
json_value::json_value(struct utillib_json_value *val) : self(val) {}
json_value::~json_value() {}
json_value::json_value() : self(utillib_json_null_create()) {}
void json_value::destroy() { utillib_json_value_destroy(self); }
bool json_value::is_null() const { return self == &utillib_json_null; }
void json_value::array_push_back(json_value element) {
  utillib_json_array_push_back(self, element.self);
}
void json_value::object_push_back(char const *key, json_value value) {
  utillib_json_object_push_back(self, key, value.self);
}
json_value json_value::operator[](char const *key) {
  auto val = utillib_json_object_at(self, key);
  return val == NULL ? json_value() : json_value(val);
}

json_value json_value::operator[](size_t index) {
  auto val = utillib_json_array_at(self, index);
  return val == NULL ? json_value() : json_value(val);
}

json_value json_object_create() {
  auto val = utillib_json_object_create_empty();
  return json_value(val);
}

json_value json_array_create() {
  auto val = utillib_json_array_create_empty();
  return json_value(val);
}

//
// utillib.string
//
string::string() { utillib_string_init(&self); }
string::string(char const *str) { utillib_string_init_c_str(&self, str); }

string::~string() { }
void string::destroy() {
  utillib_string_destroy(&self); 
}

char const *string::c_str() { return utillib_string_c_str(&self); }

void string::append(char const *str) { utillib_string_append(&self, str); }

void string::append_char(char ch) { utillib_string_append_char(&self, ch); }
size_t string::size() const { return utillib_string_size(&self); }

bool string::empty() const { return utillib_string_empty(&self); }

void string::clear() { utillib_string_clear(&self); }

size_t string::capacity() const { return utillib_string_capacity(&self); }

void string::reserve(size_t size) { utillib_string_reserve(&self, size); }

//
// hashmap
//
hashmap::hashmap(struct utillib_hashmap_callback const &callback) {
  utillib_hashmap_init(&self, &callback);
}

hashmap::~hashmap() {}

void hashmap::destroy() { utillib_hashmap_destroy(&self); }

void hashmap::destroy_owning(void (*key_destroy)(void *key),
                             void (*value_destroy)(void *value)) {
  utillib_hashmap_destroy_owning(&self, key_destroy, value_destroy);
}

size_t hashmap::size() const { return utillib_hashmap_size(&self); }

void hashmap::rehash() { utillib_hashmap_rehash(&self); }

void *hashmap::update(void const *key, void const *value) {
  return utillib_hashmap_update(&self, key, value);
}

void *hashmap::discard(void const *key) {
  return utillib_hashmap_discard(&self, key);
}

bool hashmap::empty() const { return utillib_hashmap_empty(&self); }

size_t hashmap::buckets_size() const {
  return utillib_hashmap_buckets_size(&self);
}

int hashmap::insert(void const *key, void const *value) {
  return utillib_hashmap_insert(&self, key, value);
}

bool hashmap::exist_key(void const *key) const {
  return utillib_hashmap_exist_key(&self, key);
}

void *hashmap::operator[](void const *key) {
  return utillib_hashmap_at(&self, key);
}

json_value
hashmap::tojson(utillib_json_value_create_func_t value_create) const {
  return json_value(utillib_hashmap_json_object_create(&self, value_create));
}

json_value
hashmap::tojson(utillib_json_value_create_func_t key_create,
                utillib_json_value_create_func_t value_create) const {
  return json_value(
      utillib_hashmap_json_array_create(&self, key_create, value_create));
}

vector::vector() { utillib_vector_init(&self); }

vector::~vector() {}

void vector::push_back(void const *value) {
  utillib_vector_push_back(&self, value);
}

void vector::destroy() { utillib_vector_destroy(&self); }

void vector::destroy_owning(void (*destroy)(void *value)) {
  utillib_vector_destroy_owning(&self, destroy);
}

json_value vector::tojson(utillib_json_value_create_func_t value_create) const {
  return json_value(utillib_vector_json_array_create(&self, value_create));
}

size_t vector::size() const { return utillib_vector_size(&self); }
