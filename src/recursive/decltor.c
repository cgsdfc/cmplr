#include "recursive/decltor.h"
#include "recursive/specifier.h"
#include "recursive/expr.h"	// for expr_is_constant
#include "recursive/util.h"
#include "recursive/construct.h"

// ============================================================ //
// decltor
//============================================================ //
static DECL_IS_FUNC_DECLARE (pointer);
static DECL_IS_FUNC_DECLARE (parameter_list);

DECL_IS_FUNC_DECLARE (optional_pointer)
{
  return util_is_optional (context, decl_is_pointer);
}

DECL_IS_FUNC_DECLARE (optional_abstract_decltor)
{
  return util_is_optional (context, decl_is_abstract_decltor);
}

DECL_IS_FUNC_DECLARE (decltor_in_parenthesis)
{
  return util_is_in_parentheses (context, decl_is_decltor);
}

DECL_IS_FUNC_DECLARE (constant_in_brackets)
{
  return util_is_in_brackets (context, expr_is_constant);
}

DECL_IS_FUNC_DECLARE (parameter_list_in_parentheses)
{
  return util_is_in_parentheses (context, decl_is_parameter_list);
}


static
DECL_IS_FUNC_DECLARE (parameter_declare)
{
  // K&R said it begins with declare_specifier,
  // which may include storage_specifier, but 
  // how can we use storage in a function's format
  // arguments ?
  // so it was modified to specifier_qualifier_list
  // NOTICE;
  if (decl_is_specifier_qualifier_list (context))
    {
      if (decl_is_pointer (context))
	{
	  pcontext_mark_prefix (context, PCONTEXT_POINTER, true);
	}
    }


}

static
DECL_IS_FUNC_DECLARE (parameter_list)
{
  return util_is_comma_sep_list (context, decl_is_parameter_declare, true	/* allow_empty */
    );
}

/* static */
/* DECL_IS_FUNC_DECLARE (optional_parameter_list) */
/* { */
/*   return util_is_optional (context, decl_is_parameter_list); */
/* } */

static
DECL_IS_FUNC_DECLARE (star)
{
  return util_is_terminal (context, TKT_STAR, true	/* pushing */
    );
}

static
DECL_IS_FUNC_DECLARE (pointer)
{
  // pointer := '*' ('*'|type_qualifier) *
  if (decl_is_star (context))
    {
      vector_node *v = (vector_node *) make_vector_node ();
      node_base *p = pcontext_pop_node (context);
      vector_node_push_back (v, p);
      while (true)
	{
	  if (decl_is_star (context) || decl_is_storage_specifier (context))
	    {
	      p = pcontext_pop_node (context);
	      vector_node_push_back (v, p);
	    }
	  else
	    {
	      pcontext_push_node (context, TO_NODE_BASE (v));
	      return true;
	    }
	}
    }
  return false;
}

static
DECL_IS_FUNC_DECLARE (decltor_primary)
{
  // identifier | '(' decltor ')'
  Token *t = util_read_first_token (context);
  switch (TOKEN_TYPE (t))
    {
    case TKT_IDENTIFIER:
      util_shift_one_token (context);
      pcontext_push_node (context, make_terminal_node (t));
      return true;
    case TKT_LEFT_PARENTHESIS:
      return decl_is_decltor_in_parenthesis (context);
    default:
      return false;
    }
}

DECL_IS_FUNC_DECLARE (decltor_list)
{
  // dirdecltor := identifier 
  // | dirdecltor '[' [const_expr] ']'
  // | dirdecltor '(' parameter_list ')'
  // | '(' decltor ')'
  Token *t = util_read_first_token (context);
  switch (TOKEN_TYPE (t))
    {
    case TKT_LEFT_BRACKET:
      if (decl_is_constant_in_brackets (context))
	{
	  pcontext_reduce_binary (context, OP_SUBSCRIPT);
	  return true;
	}
      die ("decltor_list: expected constant expression after '[' token");
      break;
    case TKT_LEFT_PARENTHESIS:
      if (decl_is_parameter_list_in_parentheses (context))
	{
	  pcontext_reduce_binary (context, OP_INVOKE);
	  return true;
	}
      die ("decltor_list: expected parameter_list after '(' token");
      break;
    default:
      return false;
    }
}

DECL_IS_FUNC_DECLARE (decltor)
{
  // decltor := [pointer] direct_decltor
  decl_is_optional_pointer (context);
  if (decl_is_decltor_primary (context))
    {
      while (decl_is_decltor_list (context))
	;
      pcontext_reduce_binary (context, OP_DEREF);
      return true;
    }
  return false;
}

// ========================================================== //
// abstract_decltor bool (*) [100] (int *, char *);
// ========================================================== //

static
DECL_IS_FUNC_DECLARE (abstract_decltor_primary)
{
  // abstract_decltor_primary := '(' abstract_decltor ')'
  // | '[' [const_expr] ']'
  // | '(' parameter_list ')'
  if (util_is_bracketL (context))
    {
      expr_is_optional_constant (context);
      if (util_is_bracketR (context))
	{
	  pcontext_reduce_binary (context, OP_SUBSCRIPT);
	  return true;
	}
      die ("abstract_decltor: expected ']' after constant expression");
    }
  else if (util_is_parenthesisL (context))
    {
      if (decl_is_abstract_decltor (context))
	{
	  if (util_is_parenthesisR (context))
	    {
	      return true;
	    }
	  die ("abstract_decltor: expected ')' after abstract_decltor");
	}
      decl_is_parameter_list (context);
      if (util_is_parenthesisR (context))
	{
	  pcontext_reduce_binary (context, OP_INVOKE);
	  return true;
	}
      die ("abstract_decltor: expected ')' after abstract_dirdecltor");
    }
  else
    {
      return false;
    }
}

DECL_IS_FUNC_DECLARE (abstract_decltor)
{
  decl_is_optional_pointer (context);
  size_t i = 0;
  while (decl_is_abstract_decltor_primary (context))
    {
      i++;
    }
  return i != 0;
}
