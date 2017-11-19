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

/*
 * First, defines a const array as meta information.
 */

UTILLIB_JSON_OBJECT_FIELD_BEGIN(CompilationEntry_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct compilatioin_entry, "directroy", dir_name,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct compilatioin_entry, "command", cmd,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct compilatioin_entry, "file", filename,
                               utillib_json_string_create)
UTILLIB_JSON_OBJECT_FIELD_END(CompilationEntry_Fields);

int main(void) {
  /* Prepares your C data */
  struct compilatioin_entry entry = {
      .dir_name = "./", .cmd = "gcc -o a.out a.c", .filename = "a.c"};

  /* To get the JOSN string, first you need to create a `utillib_json_value'.
   */
  struct utillib_json_value *val =
      utillib_json_object_create(&entry, CompilationEntry_Fields);

  /* A convenient function to print json with indent. */
  utillib_json_pretty_print(val, stdout);
  /* Or you can use this:
   * struct utillib_string str;
   * utillib_json_tostring(val, &str);
   * puts(utillib_string_c_str(&str));
   */

  /* Cleans up */
  utillib_json_value_destroy(val);
  return 0;
}
