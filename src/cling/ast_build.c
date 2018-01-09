#include "ast_build.h"
#include "symbols.h"

#include <stddef.h>

void cling_ast_set_op(struct utillib_json_value *self, size_t op) {
        utillib_json_object_push_back(self, "op", utillib_json_size_t_create(&op));
}

void cling_ast_set_opstr(struct utillib_json_value *self, size_t op) {
        char const *opstr = cling_symbol_kind_tostring(op);
        utillib_json_object_push_back(self, "opstr",
                        utillib_json_string_create(&opstr));
}

void cling_ast_set_lhs(struct utillib_json_value *self,
                struct utillib_json_value *lhs) {
        utillib_json_object_push_back(self, "lhs", lhs);
}

void cling_ast_set_rhs(struct utillib_json_value *self,
                struct utillib_json_value *rhs) {
        utillib_json_object_push_back(self, "rhs", rhs);
}

void cling_ast_set_extend(struct utillib_json_value *self,
                struct utillib_json_value *extend) {
        utillib_json_object_push_back(self, "extend", extend);
}

char *cling_ast_set_name(struct utillib_json_value *self, char const *name) {
        struct utillib_json_value *string = utillib_json_string_create(&name);
        utillib_json_object_push_back(self, "name", string);
        return string->as_ptr;
}

void cling_ast_set_type(struct utillib_json_value *self, size_t type) {
        utillib_json_object_push_back(self, "type",
                        utillib_json_size_t_create(&type));
}

struct utillib_json_value *cling_ast_string(char const *string) {
        return utillib_json_string_create(&string);
}

struct utillib_json_value *cling_ast_statement(size_t type) {
        struct utillib_json_value *object = utillib_json_object_create_empty();
        utillib_json_object_push_back(object, "type",
                        utillib_json_size_t_create(&type));
        return object;
}

struct utillib_json_value *cling_ast_factor(size_t code, char const *string) {
        struct utillib_json_value *object = utillib_json_object_create_empty();
        cling_ast_set_type(object, code);
        utillib_json_object_push_back(object, "value",
                        utillib_json_string_create(&string));
        return object;
}
