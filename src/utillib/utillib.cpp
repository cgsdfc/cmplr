#include <utillib/utillib.hpp>

namespace utillib {
//
// utillib.json_value
//
json_value::json_value(struct utillib_json_value *val) : self(val) {}

json_value::json_value() : self(utillib_json_null_create()) {}

void json_value::destroy() { utillib_json_value_destroy(self); }

bool json_value::is_null() const { return self == &utillib_json_null; }

bool json_value::is_array() const { return self->kind == UT_JSON_ARRAY; }

bool json_value::is_object() const { return self->kind == UT_JSON_OBJECT; }

void json_value::pretty_print() const {
  utillib_json_pretty_print(self, stdout);
}

json_value::operator json_object() const {
  json_value_check_kind(self, UT_JSON_OBJECT);
  return json_object(*this);
}

json_value::operator json_array() const {
  json_value_check_kind(self, UT_JSON_ARRAY);
  return json_array(*this);
}

json_value::operator size_t() const { 
  json_value_check_kind(self, UT_JSON_SIZE_T);
  return self->as_size_t;
}

json_value::operator double() const { 
  json_value_check_kind(self, UT_JSON_REAL);
  return self->as_double;
}

string json_value::tostring() const {
  string str;
  utillib_json_tostring(self, &str.self);
  return str;
}

//
// json_array
// 
void json_array::push_back(json_value element) {
  utillib_json_array_push_back(self, element.self);
}

json_value json_array::at(size_t index) {
  auto val = utillib_json_array_at(self, index);
  return val == NULL ? json_value() : json_value(val);
}

json_array::operator json_value() const {
  return *this;
}

json_array::json_array(json_value val) :json_value(val) {}

size_t json_array::size() const {
  return utillib_json_array_size(self);
}

//
// json_object
//
void json_object::push_back(char const *key, json_value value) {
  utillib_json_object_push_back(self, key, value.self);
}

json_value json_object::at(char const *key) {
  auto val = utillib_json_object_at(self, key);
  return val == NULL ? json_value() : json_value(val);
}

json_object::json_object(json_value val):json_value(val) {}

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

void string::destroy() { utillib_string_destroy(&self); }

char const *string::c_str() { return utillib_string_c_str(&self); }

void string::append(char const *str) { utillib_string_append(&self, str); }

void string::append_char(char ch) { utillib_string_append_char(&self, ch); }

size_t string::size() const { return utillib_string_size(&self); }

bool string::empty() const { return utillib_string_empty(&self); }

void string::clear() { utillib_string_clear(&self); }

size_t string::capacity() const { return utillib_string_capacity(&self); }

void string::reserve(size_t size) { utillib_string_reserve(&self, size); }

void string::erase_last() { utillib_string_erase_last(&self); }

void string::replace_last(char ch) { utillib_string_replace_last(&self, ch); }

//
// hashmap
//
hashmap::hashmap(struct utillib_hashmap_callback const &callback) {
  utillib_hashmap_init(&self, &callback);
}

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

void *hashmap::at(void const *key) { return utillib_hashmap_at(&self, key); }

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

void *vector::at(size_t index) { return utillib_vector_at(&self, index); }

void vector::push_back(void const *value) {
  utillib_vector_push_back(&self, value);
}

void vector::destroy() { utillib_vector_destroy(&self); }

void vector::destroy_owning(void (*destroy)(void *value)) {
  utillib_vector_destroy_owning(&self, destroy);
}

size_t vector::size() const { return utillib_vector_size(&self); }

size_t vector::capacity() const { return utillib_vector_capacity(&self); }

//
// slit
//
slist::slist() { utillib_slist_init(&self); }

void slist::destroy() { utillib_slist_destroy(&self); }

void slist::destroy_owning(void (*destroy)(void *value)) {
  utillib_slist_destroy_owning(&self, destroy);
}

bool slist::empty() const { utillib_slist_empty(&self); }

void *slist::front() { return utillib_slist_front(&self); }

void slist::push_front(void const *value) {
  utillib_slist_push_front(&self, value);
}

size_t slist::size() const { return utillib_slist_size(&self); }

//
// bitset
//
bitset::bitset(size_t N) { utillib_bitset_init(&self, N); }

void bitset::destroy() { utillib_bitset_destroy(&self); }

void bitset::insert(size_t pos) { utillib_bitset_insert(&self, pos); }

void bitset::remove(size_t pos) { utillib_bitset_remove(&self, pos); }

bool bitset::contains(size_t pos) const {
  return utillib_bitset_contains(&self, pos);
}

void bitset::merge(bitset const &rhs) {
  utillib_bitset_union(&self, &rhs.self);
}

bool bitset::equal(bitset const &rhs) const {
  return utillib_bitset_equal(&self, &rhs.self);
}

bool bitset::is_intersect(bitset const &rhs) const {
  return utillib_bitset_is_intersect(&self, &rhs.self);
}

bool bitset::insert_updated(size_t pos) {
  return utillib_bitset_insert_updated(&self, pos);
}
bool bitset::remove_updated(size_t pos) {
  return utillib_bitset_remove_updated(&self, pos);
}

bool bitset::merge_updated(bitset const &rhs) {
  return utillib_bitset_union_updated(&self, &rhs.self);
}

json_value bitset::tojson() const {
  return json_value(utillib_bitset_json_array_create(&self));
}

//
// ll1_factory
//
ll1_factory::ll1_factory(int const *gentable,
    struct utillib_symbol const *symbols,
    struct utillib_rule_literal const *rules) {
  utillib_ll1_factory_gen_init(&self, gentable, symbols, rules);
}

ll1_factory::~ll1_factory() { utillib_ll1_factory_destroy(&self); }

//
// json_parser
//
json_parser::json_parser() {
  static factory json_factory;
  utillib_json_parser_init(&self, &json_factory.self);
}

void json_parser::destroy() { utillib_json_parser_destroy(&self); }

json_value json_parser::parse(char const *str) {
  return json_value(utillib_json_parser_parse(&self, str));
}

} // utillib
