
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
#include <utillib/cpp/json.h>
#include <utility> // swap

// It is time for C++ hack...

using namespace utillib::json;

Value::~Value() noexcept {  utillib_json_value_destroy(val_); }

// Since we are not using `new' to allocate memory
// but our malloc that maybe exit the program if
// it failed, `std::bad_alloc' will not be thrown.
// However, using `noexcept' seems too aggressive.

Value::Value(struct utillib_json_value *val) : val_(val) {}

Value::Value() { val_ = utillib_json_null_create(); }

Value::Value(bool val) { val_ = utillib_json_bool_create(&val); }

Value::Value(std::size_t val) { val_ = utillib_json_size_t_create(&val); }

Value::Value(char const *val) { val_ = utillib_json_string_create(&val); }

Value::Value(double val) { val_ = utillib_json_real_create(&val); }

Value::Value(float val) { val_ = utillib_json_float_create(&val); }

Value::Value(int val) { val_ = utillib_json_int_create(&val); }

void Value::Reset()noexcept  { val_ = utillib_json_null_create(); }

void Value::swap(Value &other) noexcept { std::swap(val_, other.val_); }

std::string Value::ToString() const {
  struct utillib_string ustring;
  utillib_json_tostring(val_, &ustring);
  std::string str(utillib_string_c_str(&ustring));
  utillib_string_destroy(&ustring);
  return std::move(str);
}

void Value::PrettyPrint(std::FILE *file) const noexcept {
  utillib_json_pretty_print(val_, file);
}


Value::Value(Value &&other) noexcept : val_(other.val_) {
  other.val_ = utillib_json_null_create();
}

Value &Value::operator=(Value &&other) noexcept {
  Value().swap(*this);
  swap(other);
  return *this;
}

Object::Object() : val_(utillib_json_object_create_empty()) {}
Object::~Object() noexcept { utillib_json_value_destroy(val_); }

Object::operator Value() noexcept { auto val=val_; val_=utillib_json_null_create(); return Value(val); }

void Object::AddMember(char const *key, Value &&value) {
  utillib_json_object_push_back(val_, key, value.val_);
  value.Reset();
}

Array::Array() : val_(utillib_json_array_create_empty()) {}
Array::~Array() noexcept { utillib_json_value_destroy(val_); }
Array::operator Value() noexcept { auto val=val_; val_=utillib_json_null_create(); return Value(val); }

void Array::AddElement(Value &&element) {
  utillib_json_array_push_back(val_, element.val_);
  element.Reset();
}

