#include <utillib/json.h>

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
UTILLIB_JSON_OBJECT_FIELD_END(struct compilatioin_entry)

int main(void) {
  struct compilatioin_entry ce = {
      .dir_name = "./", .cmd = "gcc -o a.out a.c", .filename = "a.c"};
  struct utillib_json_value_t *val =
      utillib_json_object_create(&ce, sizeof ce, CompilationEntry_Fields);
  utillib_json_pretty_print(val, stdout);
  utillib_json_value_destroy(val);
  return 0;
}
