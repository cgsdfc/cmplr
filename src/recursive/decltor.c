#include "recursive/decltor.h"
#include "recursive/specifier.h"

// ============================================================ //
// decltor
//============================================================ //
static void
reduce_decltor(pcontext * context , TokenType what)
{
  binary_node * decltor=(binary_node*) make_binary_node(what);
  node_base * rhs = pcontext_pop_node(context);
  node_base * lhs = pcontext_pop_node(context);
  decltor->rhs=rhs;
  decltor->lhs=lhs;
  pcontext_push_node(context);
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
  if (decl_is_specifier_qualifier_list(context)) {
    if (decl_is_decltor(context))


}
static
DECL_IS_FUNC_DECLARE (parameter_list)
{
  return util_is_comma_sep_list(context,
      parameter_declare,
      true);
}
static
DECL_IS_FUNC_DECLARE(optional_parameter_list)
{
  return util_is_optional(context,
      parameter_list);
}
static
DECL_IS_FUNC_DECLARE(star)
{
  return util_is_terminal(context,
      TKT_STAR, true /* pushing */
      );
}

static
DECL_IS_FUNC_DECLARE (pointer)
{
  // pointer := '*' ('*'|type_qualifier) *
  if (decl_is_star(context)) {
    vector_node *v = (vector_node*) make_vector_node();
    node_base * p = pcontext_pop_node(context);
    vector_node_push_back(v, p);
    while (true) {
      if (decl_is_star(context) ||
          decl_is_storage_specifier(context))
      {
        p= pcontext_pop_node(context);
        vector_node_push_back(v, p);
      } else {
        pcontext_push_node(context, v);
        return true;
      }
    }
  }
  return false;
}

DECL_IS_FUNC_DECLARE (decltor_in_parenthesis)
{
  return util_is_in_parentheses (context, decl_is_decltor);
}

DECL_IS_FUNC_DECLARE (parameter_list_in_parenthesis)
{
  return util_is_in_parentheses (context, decl_is_parameter_list);
}

static
DECL_IS_FUNC_DECLARE(dirdecltor_primary)
{
  // identifier | '(' decltor ')'
  Token *t=util_read_first_token(context);
  switch (TOKEN_TYPE(t)) {
    case TKT_IDENTIFIER:
      util_shift_one_token(context);
      pcontext_push_node(context, make_terminal_node(TKT_IDENTIFIER));
      return true;
    case TKT_LEFT_PARENTHESIS:
      return decl_is_decltor_in_parenthesis(context);
    default:
      return false;
  }
}
DECL_IS_FUNC_DECLARE (dirdecltor_list)
{
  // dirdecltor := identifier 
  // | dirdecltor '[' [const_expr] ']'
  // | dirdecltor '(' parameter_list ')'
  // | '(' decltor ')'
  if (util_is_identifier (context))
  {
    Token *t=util_read_first_token(context);
    switch (TOKEN_TYPE(t)) {
      case TKT_LEFT_BRACKET:
        util_shift_one_token(context);
        t=util_read_first_token(context);
        if (terminal_is_braceR(t)) {
          util_shift_one_token(context);

        break;
      case TKT_LEFT_PARENTHESIS:
        break;
    }
  }
  return false;
}
static
DECL_IS_FUNC_DECLARE(dirdecltor)
{
  if (decl_is_dirdecltor_primary(context)) {
    while (decl_is_dirdecltor_list(context))
      ;
    return true;
  }
  return false;
}

DECL_IS_FUNC_DECLARE (decltor)
{
  // decltor := [pointer] direct_decltor
  util_is_optional(context, decl_is_pointer);
  if (decl_is_direct_decltor(context)) {
    node_base * direct = pcontext_pop_node(context);
    node_base * pointer = pcontext_pop_node(context);
    binary_node * decltor = (binary_node*)make_binary_node(TKT_UNARY_OP_DEREFERENCE);
    decltor->lhs=pointer;
    decltor->rhs=direct;
    pcontext_push_node(context, decltor);
    return true;
  }
  die("decltor: expected direct decltor after pointer");
}
// ========================================================== //
// abstract_decltor bool (*) [100] (int *, char *);
// ========================================================== //

static void
reduce_abstract_decltor(pcontext *context, TokenType what)
{
  unary_node * modifier = (unary_node*) make_unary_node(what);
  node_base * extend_or_arg = pcontext_pop_node(context);
  modifier->operand = extend_or_arg;
  pcontext_push_node(context, modifier);
}

static
DECL_IS_FUNC_DECLARE(abstract_decltor_primary)
{
  // abstract_decltor_primary := '(' abstract_decltor ')'
  // | '[' [const_expr] ']'
  // | '(' parameter_list ')'
  if (util_is_bracketL(context)) {
    expr_is_optional_constant(context);
    if (util_is_bracketR(context)) {
      reduce_abstract_decltor(context, TKT_BINARY_OP_SUBSCRIPT);
      return true;
    } die("abstract_decltor: expected ']' after constant expression");
  } else if (util_is_parenthesisL(context)) {
    if (decl_is_abstract_decltor(context)) { 
      if (util_is_parenthesisR(context)) { return true; }
      die("abstract_decltor: expected ')' after abstract_decltor");
    }
    decl_is_optional_parameter_list(context);
    if (util_is_parenthesisR(context)) { 
      reduce_abstract_decltor(context, TKT_BINARY_OP_INVOKE);
      return true;
    }
    die("abstract_decltor: expected ')' after abstract_dirdecltor");
  } else {
    return false;
  }
}
static
DECL_IS_FUNC_DECLARE(abstract_dirdecltor)
{
  return util_is_list(context,
      abstract_decltor_primary, 
      false /* allow_empty */
      );
}

DECL_IS_FUNC_DECLARE(optional_pointer)
{
  return util_is_optional(context,
      decl_is_pointer);
}

DECL_IS_FUNC_DECLARE(optional_abstract_dirdecltor)
{
  return util_is_optional(context,
      decl_is_abstract_dirdecltor);
}
DECL_IS_FUNC_DECLARE(abstract_decltor)
{
  if (decl_is_pointer(context)){
    if (decl_is_abstract_dirdecltor(context)) {
      reduce_abstract_decltor

}
