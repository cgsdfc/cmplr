#include "recursive/decl.h"
#include "recursive/context.h"
#include "recursive/util.h"
#include "terminal.h"
#include "error.h"

DECL_IS_FUNC_DECLARE (struct_declare)
{

}

DECL_IS_FUNC_DECLARE (struct_declare_list)
{
  return util_is_list (context, decl_is_struct_declare,
		       false /* allow_empty */ );
}


DECL_IS_FUNC_DECLARE (struct_or_union)
{
  return util_is_terminal (context, TKT_KW_STRUCT, false)
    || util_is_terminal (context, TKT_KW_UNION, false);
}

DECL_IS_FUNC_DECLARE (struct_declare_list_in_braces)
{
  return util_is_in_braces (context, decl_is_struct_declare_list);
}

DECL_IS_FUNC_DECLARE (typename)
{


}

DECL_IS_FUNC_DECLARE (struct_or_union_specifier)
{
  Token *t = util_read_first_token (context);
  switch (TOKEN_TYPE (t))
    {
    case TKT_KW_UNION:
    case TKT_KW_STRUCT:
      util_shift_one_token (context);
      if (util_is_nonshortcut_or
	  (context, util_is_identifier,
	   decl_is_struct_declare_list_in_braces))
	{
	  binary_node *struct_or_union =
	    (binary_node *) make_binary_node (TOKEN_TYPE (t));
	  node_base *list = pcontext_pop_node (context);
	  node_base *name = pcontext_pop_node (context);
	  struct_or_union->lhs = name;
	  struct_or_union->rhs = list;
	  pcontext_push_node (context, TO_NODE_BASE (struct_or_union));
	  return true;
	}
      die
	("struct_or_union: expected identifier or struct declaration list in braces");
    default:
      return false;
    }
}

DECL_IS_FUNC_DECLARE (typename_in_parenthesis)
{
  return util_is_in_parentheses (context, decl_is_typename);
}

DECL_IS_FUNC_DECLARE (declarator_in_parenthesis)
{
  return util_is_in_parentheses (context, decl_is_declarator);
}

DECL_IS_FUNC_DECLARE (parameter_list)
{

}

DECL_IS_FUNC_DECLARE (parameter_list_in_parenthesis)
{
  return util_is_in_parentheses (context, decl_is_parameter_list);
}

DECL_IS_FUNC_DECLARE (declare)
{
  return false;
}

DECL_IS_FUNC_DECLARE (direct_declarator)
{
  if (util_is_identifier (context))
    {
      return true;
    }
  if (decl_is_declarator_in_parenthesis (context))
    {
      return true;
    }
}

DECL_IS_FUNC_DECLARE (declarator)
{

}

DECL_IS_FUNC_DECLARE (parameter_declare)
{

}


DECL_IS_FUNC_DECLARE (pointer)
{

}

enum {
  CHAR, CHAR_SIGNED, CHAR_UNSIGNED,

};

// the type_specifier node for struct_or_union is
// a binary_node if it has both tag and body;
// a unary_node if it has either tag or body;
// struct_node = binary_node(terminal_node, vector_node);
// | unary_node(terminal_node);
// | unary_node(vector_node);
// the type_specifier node for builtin types is just a terminal_node;
// type_qualifier is also a terminal_node;
// type_info node = ternary_node(storage_specifier, type_qualifier, type_specifier);
// 
// the node for a complete declaration is a unary_node of ternary_node:
// unary_node(ternary_node(type_info, declarator, initializer));
// 
// the function node is similiar, except the initializer is replaced by
// compound_stmt:
// unary_node(binary_node(type_info, declarator, compound_stmt));
// 
// and the translation is just a vector_node of function or declaration;
DECL_IS_FUNC_DECLARE(type_specifier)
{
  // to simplify things, sign_specifier
  // and size_specifier are created terms
  // for integral types.
  // an integral type can have optional size_specifier,
  // optional sign_specifier in any order. What's more,
  // except int and char, other integral types like short
  // and long can have optional int.
  // float, double and long double and void and identifier
  // are just as it.
  // union and struct require further parsing.
  // NOTE;
  Token *t=util_read_first_token(context);
  Token *lookahead;
  switch (TOKEN_TYPE(t)) {
    // handle easy cases first;
    case TKT_KW_FLOAT:
    case TKT_KW_DOUBLE:
    case TKT_KW_VOID:
    case TKT_IDENTIFIER:
      util_shift_one_token(context);
      pcontext_push_node(context, make_terminal_node(t));
      return true;
    case TKT_KW_LONG:
      lookahead=pcontext_read_token(context, 1);
      if (TOKEN_TYPE(lookahead) == TKT_KW_DOUBLE) {

  }

  }
}
// =============================================================== //
// storage_specifier: static extern typedef auto register 
// =============================================================== //
DECL_IS_FUNC_DECLARE (storage_specifier)
{
  return util_is_terminal_pred (context, 
      terminal_is_storage_specifier,
      true /* pushing */ );
}

// =============================================================== //
// type_qualifier: const volatile
// =============================================================== //
DECL_IS_FUNC_DECLARE(type_qualifier)
{
  return util_is_terminal(context,
      terminal_is_qualifier,
      true // pushing
      );
}

DECL_IS_FUNC_DECLARE(type_qualifier_list)
{
  // any number and any combination of 'const' 
  // and 'volatile' token is acceptable
  // thus it is a vector_node
  return util_is_list(context,
      decl_is_type_qualifier,
      true // allow_empty
      );
}
DECL_IS_FUNC_DECLARE(storage_specifier_list)
{
  // zero or one of the storage_specifier
  if (decl_is_storage_specifier(context)) {
    Token *t=util_read_first_token(context);
    if (terminal_is_storage_specifier(t)) {
      die("decl: multiple storage classes in declaraton specifier");
    } return true;
  } 
  util_push_node_null(context);
  return true;
}


enum { DSP_TPSP, DSP_TPQR, DSP_STORAGE, DSP_N };
static void
declare_specifier_begin(utillib_vector * v)
{
  for (int i=0;i<DSP_N; ++i) {
    utillib_vector_init(&to_init[i]);
  }
}
static void
declare_specifier_end(utillib_vector *v) 
{
  for (int i=0;i<DSP_N; ++i) {
    utillib_vector_destroy (&to_init[i]);
  }
}
static void
reduce_declare_specifier(pcontext *context, utillib_vector *v)
{
  utillib_vector *storage=&v[DSP_STORAGE];
  utillib_vector *qualifier=&v[DSP_STORAGE];
  utillib_vector *specifier=&v[DSP_TPSP];
  assert (utillib_vector_size(storage) <= 1);
  assert (utillib_vector_size(specifier) <= 1);
  ternary_node * declare_specifier = (ternary_node*) 
  make_ternary_node();
  terminal_node * qualifier_node=make_nullary_node();

  if (utillib_vector_size(storage)) {
    declare_specifier->first=utillib_vector_back(storage);
  } else {
    declare_specifier->first=make_nullary_node();
  }
  if (utillib_vector_size(specifier)) {
    declare_specifier->third=utillib_vector_back(specifier);
  } else {
    declare_specifier->third=make_nullary_node();
  }
  for (size_t size=utillib_vector_size(qualifier);
      size > 0; --size)
  {
    terminal_node *terminal=utillib_vector_pop_back(qualifier);
  }
}
  




 


// =============================================================== //
// declare_specifier
// =============================================================== //
DECL_IS_FUNC_DECLARE(declare_specifier)
{
  // a ternary_node of (storage_specifier, type_qualifier, type_specifier);
  // any number of single_declare_specifier
  utillib_vector vec[DSP_N];
  declare_specifier_begin(vec);
  size_t size = 0;
  while (true) {
    if (decl_is_storage_specifier(context)) {
      utillib_vector * storage=&vec[DSP_STORAGE];
      if (utillib_vector_size(storage) > 1) {
        declare_specifier_end(vec);
        die("declare_specifier: multiple storage class");
      }
     utillib_vector_push_back(storage, pcontext_pop_node(context));
    } else if (decl_is_type_qualifier(context)) {
      utillib_vector *qualifier = &vec[DSP_TPQR];
      utillib_vector_push_back(type_qualifier, pcontext_pop_node(context));
    } else if (decl_is_type_specifier(context)) {
      utillib_vector *specifier=&vec[DSP_TPSP];
      if (utillib_vector_size(specifier) > 1) {
        declare_specifier_end(vec);
        die("declare_specifier: two or more data types in declare specifier");
      }
      utillib_vector_push_back(specifier, pcontext_pop_node(context));
    } else { break ;}// end of parsing
  }
  reduce_declare_specifier(context, vec);
  return true;
}

static void reduce_func(pcontext *context)
{
    node_base *compound_stmt = pcontext_pop_node(context);
    node_base *declarator = pcontext_pop_node(context);
    node_base *declare_specifier = pcontext_pop_node(context);
    ternary_node * func= (ternary_node*) make_binary_node(ternary_node);
    unary_node * func_def = (unary_node*) make_unary_node(TKT_FUNCTION);
    func->first=declare_specifier;
    func->second=declarator;
    func->third=compound_stmt;
    func_def->operand=func;
    pcontext_push_node(context, TO_NODE_BASE(func_def));


}
DECL_IS_FUNC_DECLARE(function_impl)
{
  // this is called only after the seq
  // declare_specifier declarator '{' has 
  // been seen and the '{' is not yet shift;
  Token *t = util_read_first_token(context);
  assert (terminal_is_braceL(t));
  if (stmt_is_compound(context)) {
    reduce_func(context);
    return true;
  } return false;
}
// =============================================================== //
// function
// =============================================================== //
DECL_IS_FUNC_DECLARE(function)
{
  // function definition
  if (pcontext_test_prefix(context, PCONTEXT_DESP_DECLR)) {
    // a declare_specifier declarator prefix has been parsed
    // and reduced on the node stack.
    if (decl_is_function_impl(context)) {
      pcontext_mark_prefix(context, PCONTEXT_DESP_DECLR, false);
      return true;
    }die("function: expected compound_stmt after '{' token"); 
  }
  if (util_is_sequence(context,
        decl_is_declare_specifier,
        decl_is_declarator
        NULL)) // parse the prefix.
  {
    // lookahead for '{' .
    Token *t=util_read_first_token(context);
    if (terminal_is_braceL(t)) {
      return decl_is_function_impl(context);
    }
    pcontext_mark_prefix(context, PCONTEXT_DESP_DECLR);
    return false;
}

// =============================================================== //
// external
// =============================================================== //
DECL_IS_FUNC_DECLARE(external)
{
  return util_is_one_of(context,
      decl_is_declare,
      decl_is_function,
      NULL);
}

DECL_IS_FUNC_DECLARE(translation)
{
  return util_is_list(context,
      decl_is_external);
}
