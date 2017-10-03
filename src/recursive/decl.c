#include "recursive/decl.h"
#include "recursive/decltor.h"
#include "expr.h"
#include "specifier.h"
#include "stmt.h"

DECL_IS_FUNC_DECLARE (optional_comma)
{
  util_is_comma (context);
  return true;
}

// =============================================================== //
// function
// =============================================================== //
static void
reduce_func (pcontext * context)
{
  node_base *compound_stmt = pcontext_pop_node (context);
  node_base *decltor = pcontext_pop_node (context);
  node_base *declare_specifier = pcontext_pop_node (context);
  ternary_node *func = (ternary_node *) make_ternary_node ();
  unary_node *func_def = (unary_node *) make_unary_node (TKT_FUNCTION);
  func->first = declare_specifier;
  func->second = decltor;
  func->third = compound_stmt;
  func_def->operand = TO_NODE_BASE (func);
  pcontext_push_node (context, TO_NODE_BASE (func_def));
}

static
DECL_IS_FUNC_DECLARE (function_impl)
{
  // this is called only after the seq
  // declare_specifier decltor '{' has 
  // been seen and the '{' is not yet shift;
  Token *t = util_read_first_token (context);
  assert (terminal_is_braceL (t));
  if (stmt_is_compound (context))
    {
      reduce_func (context);
      return true;
    }
  return false;
}

DECL_IS_FUNC_DECLARE (function)
{
  // function definition
  if (pcontext_test_prefix (context, PCONTEXT_DESP_DECLR))
    {
      // a declare_specifier decltor prefix has been parsed
      // and reduced on the node stack.
      if (decl_is_function_impl (context))
	{
	  pcontext_mark_prefix (context, PCONTEXT_DESP_DECLR, false);
	  return true;
	}
      die ("function: expected compound_stmt after '{' token");
    }
  if (util_is_sequence (context, decl_is_declare_specifier, decl_is_decltor NULL))	// parse the prefix.
    {
      // lookahead for '{' .
      Token *t = util_read_first_token (context);
      if (terminal_is_braceL (t))
	{
	  return decl_is_function_impl (context);
	}
      pcontext_mark_prefix (context, PCONTEXT_DESP_DECLR, true);
      return false;
    }
}

DECL_IS_FUNC_DECLARE (assign)
{
  return util_is_terminal (context, TKT_BINARY_OP_ASSIGN,
			   false /* pushing */ );
}

// =============================================================== //
// declare
// =============================================================== //

static void reduce_declare(pcontext *);
static DECL_IS_FUNC_DECLARE(initializer);
static
DECL_IS_FUNC_DECLARE (init_decltor)
{
  if (decl_is_decltor(context)) {
    if (decl_is_assign(context)) {
      if (decl_is_initializer(context)) {
        reduce_decltor(context);
        return true;
      } die("init_decltor: expected initializer after '=' token");
    } 
  } return false;
}

static
DECL_IS_FUNC_DECLARE (init_decltor_list)
{
  return 
  util_is_comma_sep_list(context, decl_is_init_decltor, 
      true /* allow_empty */
    );
}

static DECL_IS_FUNC_DECLARE (initializer_list);
static DECL_IS_FUNC_DECLARE (initializer_seq)
{
  return util_is_sequence(context,
      decl_is_initializer_list,
      decl_is_optional_comma,
      NULL);
}
static DECL_IS_FUNC_DECLARE (initializer_in_braces)
{
  return util_is_in_braces(context,
      decl_is_initializer_seq);
}

static
DECL_IS_FUNC_DECLARE (initializer)
{
  if (util_is_braceL (context))
    {
      if (decl_is_initializer_in_braces(context)) {
        return true;
        } die ("initializer: expected '}' after initializer_list");
    } else if (expr_is_assign(context)) {
      return true;
    }
  return false;
}

static
DECL_IS_FUNC_DECLARE (initializer_list)
{
  return util_is_comma_sep_list (context,
      decl_is_initializer,
      true	/* allow_empty */
    );
}
static void
reduce_declare(pcontext * context)
{
  binary_node * declare=(binary_node*)make_binary_node(TKT_DECLARE);
  node_base * init_decltors = pcontext_pop_node(context);
  node_base * specifier = pcontext_pop_node(context);
  declare->lhs=specifier;
  declare->rhs=init_decltors;
  pcontext_push_node(context, TO_NODE_BASE(declare));
}

DECL_IS_FUNC_DECLARE (declare)
{
  if (decl_is_declare_specifier (context)) {
    // optional init_decltor_list
    reduce_declare(context);
    if (util_is_semicolon(context)) {
      return true;
    } die("declare: expected ';' at the end of declaration");
  }
  return false;
}

// =============================================================== //
// external
// =============================================================== //
DECL_IS_FUNC_DECLARE (external)
{
  return util_is_one_of (context, decl_is_declare, decl_is_function, NULL);
}

DECL_IS_FUNC_DECLARE (translation)
{
  return util_is_list (context, decl_is_external, true /* allow_empty */ );
}

// ============================================================ //
// typename
//============================================================ //
DECL_IS_FUNC_DECLARE (typename_)
{
  // typename := (specifier|qualifier)+ [pointer] [abstract_dirdecltor]
  if (decl_is_specifier_qualifier_list (context))
    {
      decl_is_optional_pointer (context);
      decl_is_optional_abstract_decltor (context);
      ternary_node *ternary = (ternary_node *) make_ternary_node ();
      node_base *abs = pcontext_pop_node (context);
      node_base *p = pcontext_pop_node (context);
      node_base *sqlist = pcontext_pop_node (context);
      ternary->first = sqlist;
      ternary->second = p;
      ternary->third = abs;
      pcontext_push_node (context, TO_NODE_BASE (ternary));
      return true;
    }
  return false;
}

DECL_IS_FUNC_DECLARE (typename_in_parenthesis)
{
  return util_is_in_parentheses (context, decl_is_typename_);
}
DECL_IS_FUNC_DECLARE (declare_list)
{
  return util_is_list (context, decl_is_declare, true /* allow_empty */ );
}
