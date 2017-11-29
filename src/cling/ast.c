#include "ast.h"
#include "symbols.h"

#include <utillib/json_foreach.h>
/**
 * \function cling_ast_insert_const
 * Inserts a single_const_decl into the symbol_table.
 * Assumes self is not null.
 * Assumes all the symbol in `self' are not redefined.
 * Inserts into the current scope.
 */

void cling_ast_insert_const(struct utillib_json_value * self,
    struct cling_symbol_table *symbols)
{
  struct utillib_json_value *type=utillib_json_object_at(self, "type");
  struct utillib_json_value *const_defs=utillib_json_object_at(self, "const_defs");
  int kind=CL_CONST | (type->as_size_t == SYM_KW_INT ? CL_INT : CL_CHAR);
  int retv;

  UTILLIB_JSON_ARRAY_FOREACH(obj, const_defs) {
    struct utillib_json_value * identifier=utillib_json_object_at(obj, "identifier");
    assert(identifier);
    cling_symbol_table_update(symbols, kind, identifier->as_ptr, obj);
   }
}

void cling_ast_insert_variable(struct utillib_json_value * self,
    struct cling_symbol_table *symbols)
{
  struct utillib_json_value * type=utillib_json_object_at(self, "type");
  struct utillib_json_value * var_defs=utillib_json_object_at(self, "var_defs");
  int kind=type->as_size_t == SYM_KW_INT?CL_INT: CL_CHAR;
  int retv;

  UTILLIB_JSON_ARRAY_FOREACH(obj, var_defs) {
    struct utillib_json_value * identifier=utillib_json_object_at(obj, "identifier");
    assert(identifier);
    cling_symbol_table_update(symbols, 
        utillib_json_object_at(obj, "extend")?kind|CL_ARRAY : kind ,
        identifier->as_ptr, obj);
  }
}

