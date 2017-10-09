#include "recursive/specifier.h"
#include "recursive/struct.h"
#include "recursive/terminal.h"
#include "recursive/util.h"

/// =========================================================== //
// type_specifier
// =========================================================== //
static DECL_IS_FUNC_DECLARE(integral_keyword) {
  // int char long short signed unsigned
  return util_is_terminal_pred(context, terminal_is_integral_keyword,
                               true /* pushing */);
}

static DECL_IS_FUNC_DECLARE(integral_type_impl1) {
  return util_is_list(context, decl_is_integral_keyword, false /* allow_empty */
                      );
}

DECL_IS_FUNC_DECLARE(integral_type) {
  return decl_is_integral_type_impl1(context);
}

DECL_IS_FUNC_DECLARE(type_specifier) {
  Token *t = util_read_first_token(context);
  switch (TOKEN_TYPE(t)) {
  // must be single token.
  case TKT_KW_FLOAT:
  case TKT_KW_DOUBLE:
  case TKT_KW_VOID:
  case TKT_IDENTIFIER:
    util_shift_one_token(context);
    pcontext_push_node(context, make_terminal_node(t));
    return true;
  // can take variable length of tokens.
  case TKT_KW_ENUM:
    if (decl_is_enum_specifier(context)) {
      return true;
    }
    die("type_specifier: expected enum specifier after 'enum' token");
  case TKT_KW_STRUCT:
  case TKT_KW_UNION:
    if (decl_is_struct_or_union_specifier(context)) {
      return true;
    }
    die("type_specifier: expected struct_or_union_specifier after "
        "'struct_or_union' token");
  default:
    if (decl_is_integral_type(context)) {
      return true;
    }
    return false;
  }
}

// =============================================================== //
// storage_specifier: static extern typedef auto register
// =============================================================== //
DECL_IS_FUNC_DECLARE(storage_specifier) {
  return util_is_terminal_pred(context, terminal_is_storage_specifier,
                               true /* pushing */);
}

// =============================================================== //
// type_qualifier: const volatile
// =============================================================== //
DECL_IS_FUNC_DECLARE(type_qualifier) {
  return util_is_terminal_pred(context, terminal_is_type_qualifier,
                               true // pushing
                               );
}

DECL_IS_FUNC_DECLARE(type_qualifier_list) {
  return util_is_list(context, decl_is_type_qualifier, true // allow_empty
                      );
}

DECL_IS_FUNC_DECLARE(specifier_qualifier) {
  return util_is_one_of(context, decl_is_type_specifier, decl_is_type_qualifier,
                        NULL);
}

DECL_IS_FUNC_DECLARE(specifier_qualifier_list) {
  return util_is_list(context, decl_is_specifier_qualifier,
                      false /* allow_empty */
                      );
}

// =============================================================== //
// declare_specifier
// =============================================================== //
enum { DSP_TPSP, DSP_TPQR, DSP_STORAGE, DSP_N };
static void declare_specifier_begin(vector_node **v) {
  for (int i = 0; i < DSP_N; ++i) {
    v[i] = (vector_node *)make_vector_node();
  }
}

static void declare_specifier_end(vector_node **v) {
  for (int i = 0; i < DSP_N; ++i) {
    destroy_vector_node(v[i]);
  }
}

static void reduce_declare_specifier(pcontext *context, vector_node **v) {
  vector_node *storage = v[DSP_STORAGE];
  vector_node *qualifier = v[DSP_STORAGE];
  vector_node *specifier = v[DSP_TPSP];
  ternary_node *declare_specifier = (ternary_node *)make_ternary_node();
  declare_specifier->first = TO_NODE_BASE(storage);
  declare_specifier->second = TO_NODE_BASE(qualifier);
  declare_specifier->third = TO_NODE_BASE(specifier);
  pcontext_push_node(context, TO_NODE_BASE(declare_specifier));
}

DECL_IS_FUNC_DECLARE(declare_specifier) {
  // a ternary_node of (storage_specifier, type_qualifier, type_specifier);
  vector_node *vec[DSP_N];
  declare_specifier_begin(vec);
  while (true) {
    if (decl_is_storage_specifier(context)) {
      vector_node *storage = vec[DSP_STORAGE];
      vector_node_push_back(storage, pcontext_pop_node(context));
    } else if (decl_is_type_qualifier(context)) {
      vector_node *qualifier = vec[DSP_TPQR];
      vector_node_push_back(qualifier, pcontext_pop_node(context));
    } else if (decl_is_type_specifier(context)) {
      vector_node *specifier = vec[DSP_TPSP];
      vector_node_push_back(specifier, pcontext_pop_node(context));
    } else {
      break;
    } // end of parsing
  }
  reduce_declare_specifier(context, vec);
  return true;
}
