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
#ifndef UTILLIB_CPP_JSON_H
#define UTILLIB_CPP_JSON_H

extern "C" {
#include <utillib/json.h>
}
#include <string>
#include <cstddef>
#include <cstdio>

namespace utillib {
namespace json {
class Object;
class Array;
class Value {
public:
  Value();
  Value(bool val);
  Value(size_t val);
  Value(char const *val);
  Value(double val);
  Value(float val);
  Value(int val);
  ~Value() noexcept;
  Value(Value const &other) = delete;
  Value &operator=(Value const &other) = delete;
  Value(Value &&other) noexcept;
  Value &operator=(Value &&other) noexcept;
  void swap(Value &other) noexcept;
  std::string ToString() const ;
  void PrettyPrint(FILE *file) const noexcept;

  bool operator== (Value const& other) = delete;
  bool operator<= (Value const& other) = delete;
  bool operator>= (Value const& other) = delete;
  bool operator!= (Value const& other) = delete;
  bool operator< (Value const& other) = delete;
  bool operator> (Value const& other) = delete;

private:
  friend class Object;
  friend class Array;
  Value(struct utillib_json_value *val);
  void  Reset() noexcept ;
  struct utillib_json_value *val_;
};

template<class T>
class ObjectFields {
  public:
    ObjectFields(struct utillib_json_object_field const *fields)noexcept:fields_(fields) {}

  private:
    friend class Object;
    struct utillib_json_object_field const *fields_;
};

class Object {
public:
  Object(Object const &other) = delete;
  Object &operator=(Object const &other) = delete;
  operator Value () noexcept;
  template<class T>
  Object(T const* base, ObjectFields<T> const& objectFeilds):
    val_(utillib_json_object_create(base, objectFeilds.fields_)){}

  Object();
  ~Object() noexcept;
  void AddMember(char const *key, Value &&value);

  bool operator== (Object const& other) = delete;
  bool operator<= (Object const& other) = delete;
  bool operator>= (Object const& other) = delete;
  bool operator!= (Object const& other) = delete;
  bool operator< (Object const& other) = delete;
  bool operator> (Object const& other) = delete;

private:
  struct utillib_json_value *val_;
};

template <class ElementT>
class ArrayDescription {
  public:
      ArrayDescription(size_t size, utillib_json_value_create_func_t create_func) noexcept
      {
        utillib_json_array_desc_init(&desc_, sizeof (ElementT), size, create_func);
      }
  private:
      friend class Array;
      struct utillib_json_array_desc desc_;
};

class Array {
public:
  Array(Array const &other) = delete;
  Array &operator=(Array const &other) = delete;
  operator Value() noexcept;
  Array();
  template<class T>
  Array(T const * base, ArrayDescription<T> const& arrayDesc): val_(utillib_json_array_create(base, &arrayDesc.desc_)) {}
  ~Array() noexcept;
  void AddElement(Value &&element);

  bool operator== (Array const& other) = delete;
  bool operator<= (Array const& other) = delete;
  bool operator>= (Array const& other) = delete;
  bool operator!= (Array const& other) = delete;
  bool operator< (Array const& other) = delete;
  bool operator> (Array const& other) = delete;

private:
  struct utillib_json_value *val_;
};

template<class T> Value MakeValue(T value) { return Value(value) ; }
template<class T> Value MakeObject(T const *base, ObjectFields<T> const& fields) {
  return Object(base, fields);
}
template<class T> Value MakeArray(T const *base, ArrayDescription<T> const &desc) {
  return Array(base, desc);
}

} // namespace json
} // namespace utillib

#endif // UTILLIB_CPP_JSON_H
