#include <stddef.h>
#include <stdio.h>

typedef struct utillib_field_entry_t {
  char const *field_name;
  size_t field_offset;
  size_t field_size;
} utillib_field_entry_t;

typedef struct utillib_struct_desc_t {
  utillib_field_entry_t *fields;
  char const *name;
} utillib_struct_desc_t;

typedef struct utillib_json_object_t {
  utillib_struct_desc_t *desc;
  utillib_vector members;
} utillib_json_object_t;

typedef struct utillib_json_array_t {
  utillib_vector elements;
} utillib_json_array_t;

typedef struct utillib_json_value_t {
  int kind;
  union {
    as_real;
    as_ptr;
    as_int;
  } value;
} utillib_json_value_t;

typedef struct Employee {
  char const *Name;
  size_t ID;
} Employee;

#define UT_BEGIN(STRUCT)                                                       \
  const utillib_struct_desc_t *STRUCT##_desc() {                               \
    static STRUCT *get_field_size;                                             \
  static utillib_field_entry_t static_entries[] = {
#define UTFT(STRUCT, FIELD)                                                    \
  {.field_name = #FIELD,                                                       \
   .field_offset = offsetof(STRUCT, FIELD),                                    \
   .field_size = sizeof get_field_size->FIELD},
#define UT_END(STRUCT)                                                         \
  { 0 }                                                                        \
  }                                                                            \
  ;                                                                            \
  static const utillib_struct_desc_t static_struct_desc = {                    \
      .name = #STRUCT, .fields = static_entries};                              \
  return &static_struct_desc;                                                  \
  }

UT_BEGIN(Employee)
UTFT(Employee, Name)
UTFT(Employee, ID)
UT_END(Employee)

void utillib_struct_desc_print(utillib_struct_desc_t *self) {
  printf("struct `%s'\n", self->name);
  for (utillib_field_entry_t *p = self->fields; p->field_name != NULL; ++p) {
    printf("field `%s', offset `%lu', size `%lu'\n", p->field_name,
           p->field_offset, p->field_size);
  }
}

int main() { utillib_struct_desc_print(Employee_desc()); }
