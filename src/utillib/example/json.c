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
/*
 * JSON is a data format that is both easy-to-parse and human readable.
 * The following code example shows how to create JSON string from C
 * data structure.
 */

struct compilatioin_entry {
  char const *dir_name;
  char const *cmd;
  char const *filename;
};

UTILLIB_JSON_OBJECT_FIELD_BEGIN(CompilationEntry_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct compilatioin_entry, "directroy", dir_name,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct compilatioin_entry, "command", cmd,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct compilatioin_entry, "file", filename,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_END(CompilationEntry_Fields);

void print_json_stdout(char const *msg, struct utillib_json_value const *val) {
  puts(msg);
  utillib_json_pretty_print(val);
}

void use_primary_type(void) {
  puts("Use JSON with primary C types like int, double and bool");
  int number = 404;
  struct utillib_json_value *val = utillib_json_int_create(&number);
  print_json_stdout("int", val);
  utillib_json_value_destroy(val);

  bool boolean = false;
  val = utillib_json_bool_create(&boolean);
  print_json_stdout("bool", val);
  utillib_json_value_destroy(val);

  char const *string = "Hellow JSON";
  val = utillib_json_bool_create(&boolean);
  print_json_stdout("string", val);
  utillib_json_value_destroy(val);
}

void use_pod_structure(void) {
  puts("Use JSON with POD structure");
  struct compilatioin_entry entry = {
      .dir_name = "./", .cmd = "gcc -o a.out a.c", .filename = "a.c"};

  struct utillib_json_value *val =
      utillib_json_object_create(&entry, CompilationEntry_Fields);
  print_json_stdout("struct compilatioin_entry", val);
  utillib_json_value_destroy(val);
}

void use_builtin_array(void) {
  char const *const strings[] = {"Use", "JSON", "in", "array", "of", "strings"};
  struct utillib_json_array_desc desc;
  utillib_json_array_desc_init(&desc, sizeof(char const *),
                               sizeof strings / sizeof strings[0],
                               utillib_json_string_create);
  struct utillib_json_value *val = utillib_json_array_create(strings, &desc);
  print_json_stdout("char const * const[]", val);
  utillib_json_value_destroy(val);
}

int main(void) {
  use_primary_type();
  use_pod_structure();
  use_builtin_array();
  return 0;
}
