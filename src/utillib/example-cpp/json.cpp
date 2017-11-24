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
#include <iostream>
#include <vector>
#include <utility>

using std::cout;
using namespace utillib::json;

void UsePrimaryValue() 
{
  Value boolv(true);
  /* cout << boolv.ToString() << '\n'; */

  /* Value intv(-100); */
  /* cout << intv.ToString() << '\n'; */

  /* Value doublev(3.14444); */
  /* cout << doublev.ToString() << '\n'; */

  /* Value stringv("hello world"); */
  /* cout << stringv.ToString() << '\n'; */

  /* cout << MakeValue(double(4333)).ToString() << '\n'; */
}

struct Student {
  char const *name;
  size_t id;
  double gpa;
  Value JsonValue() const {
    Object object;
    object.AddMember("name", Value(name));
    object.AddMember("id", Value(id));
    object.AddMember("gpa", Value(gpa));
    return object;
  }
};

void UseStructure() {
  Student student { "John", 1, 3.8 };
  auto json=student.JsonValue();
  cout << json.ToString() << '\n';
  json.PrettyPrint(stdout);
}

void UseArray() {
  std::vector<Student> students{ 
    { "May", 1, 2.2 },
    { "Tom", 2, 2.3 }
  };
  Array array;
  for (auto const& s: students) {
    array.AddElement(s.JsonValue());
  }
  Value(array).PrettyPrint(stdout);
}


int main() {

  UsePrimaryValue();

  /* UseStructure(); */

  /* UseArray(); */
}
