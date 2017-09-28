#include "recursive/decl.h"
#include "recursive/context.h"
#include "recursive/util.h"
#include "terminal.h"
#include "error.h"

DECL_IS_FUNC_DECLARE (storage_specifier)
{
  return util_is_terminal_pred (context, terminal_is_storage_specifier,
				true /* pushing */ );
}

DECL_IS_FUNC_DECLARE (type_qualifier)
{
  return util_is_terminal_pred (context, terminal_is_type_qualifier, true);
}

DECL_IS_FUNC_DECLARE(struct_declare)
{

}

DECL_IS_FUNC_DECLARE (struct_declare_list)
{
  return util_is_list (context, decl_is_struct_declare);
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
	  (context, util_is_identifier, decl_is_struct_declare_list_in_braces))
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
DECL_IS_FUNC_DECLARE(declarator_in_parenthesis)
{
  return util_is_in_parentheses(context, decl_is_declarator);
}
DECL_IS_FUNC_DECLARE(parameter_list)
{
  return util_is_list(context, parameter_declare);
}

DECL_IS_FUNC_DECLARE(parameter_list_in_parenthesis)
{
  return util_is_in_parentheses(context, parameter_list);
}

DECL_IS_FUNC_DECLARE (declare)
{

}

DECL_IS_FUNC_DECLARE(direct_declarator)
{
  if (util_is_identifier(context)) {
    return true;
  }
  if (decl_is_declarator_in_parenthesis(context)) {
    return true;
  }
  if 

DECL_IS_FUNC_DECLARE (declarator)
{

}
