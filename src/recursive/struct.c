#include "recursive/struct.h"
#include "recursive/decltor.h"
#include "recursive/expr.h" // for expr_is_constant

// ============================================================ //
// struct_or_union_specifier
//============================================================ //
static void
reduce_enum_struct_union(pcontext *context, TokenType what,
    node_base *tag, node_base *body)
{
  binary_node * sp = (binary_node*) make_binary_node(what);
  sp->lhs=tag;
  sp->rhs=body;
  pcontext_push_node(context, TO_NODE_BASE(sp));
}
  
static
DECL_IS_FUNC_DECLARE(struct_decltor)
{
  // struct_decltor := decltor | : const_expr
  // | decltor : const_expr
  // currently not support bit field;
  return decl_is_decltor(context);
}

static
DECL_IS_FUNC_DECLARE(struct_decltor_list)
{
  return util_is_separated_list(context,
      TKT_COMMA, decl_is_struct_decltor,
      true /* allow_empty */);
}
static
DECL_IS_FUNC_DECLARE (struct_declare)
{
  // better call struct member;
  // each struct_declare is a binary_node
  // (specifier_qualifier_list, struct_decltor);
  if (decl_is_specifier_qualifier_list(context)
      && decl_is_struct_decltor_list(context))
  {
    binary_node * struct_declare = (binary_node*)
    make_binary_node(TKT_KW_STRUCT);
    node_base * decltor=pcontext_pop_node(context);
    node_base * sq_list = pcontext_pop_node(context);
    struct_declare->lhs=sq_list;
    struct_declare->rhs=decltor;
    pcontext_push_node(context, TO_NODE_BASE( struct_declare ));
    return true;
  }
  return false;

}
static
DECL_IS_FUNC_DECLARE (struct_declare_list)
{
  return util_is_list (context, decl_is_struct_declare,
		       true /* allow_empty */ );
}
static
DECL_IS_FUNC_DECLARE (struct_or_union)
{
  return util_is_terminal (context, TKT_KW_STRUCT, false) // pushing
    || util_is_terminal (context, TKT_KW_UNION, false);
}

DECL_IS_FUNC_DECLARE (struct_or_union_specifier)
{
  // this function assumes the 'struct_or_union'
  // token has been seen but not shifted;
  // figure out if it is 'struct_or_union'
  Token *t= util_read_first_token(context);
  TokenType what = TOKEN_TYPE(t);
  util_shift_one_token(context);
  t=util_read_first_token(context);
  bool has_tag=false;
  node_base * tag, * body;
  switch (TOKEN_TYPE (t))
    {
    case TKT_IDENTIFIER:
      has_tag=true;
      util_shift_one_token(context);
      pcontext_push_node(context, make_terminal_node(t));
      t=util_read_first_token(context);
      if (! terminal_is_braceL(t)) { // not a body
        reduce_enum_struct_union(context,
            what, pcontext_pop_node(context) /* tag */,
            make_nullary_node() /* body */
            );
        return true;
      } // fails through to '{'
    case TKT_LEFT_BRACE:
      util_shift_one_token(context); // shift away '{'
      if (decl_is_struct_declare_list(context)) {
        if (util_is_braceR(context)) {
          body=pcontext_pop_node(context);
          tag=has_tag?pcontext_pop_node(context):make_nullary_node(context);
          reduce_enum_struct_union(context, what, tag, body);
          return true;
        } die("struct_or_union_specifier: expected '}'");
      } die("struct_or_union_specifier: expected struct_declare_list after '{' token");
    default:
      return false;
    }
}

// ========================================================== //
// enum_specifier
// ========================================================== //
static
DECL_IS_FUNC_DECLARE(assign)
{
  return util_is_terminal(context, TKT_BINARY_OP_ASSIGN, false, /* pushing */);
}
static
DECL_IS_FUNC_DECLARE(enumerator)
{
  // enumerator := identifier [ = const_expr ]
  node_base *identifier, *const_expr;
  binary_node *enum_with_const;
  if (util_is_identifier(context)) { // terminal_node(identifier) on top
    if (decl_is_assign(context)) {
      if (expr_is_constant(context)) { // const_expr on top
        // enumerator with optional part;
        const_expr = pcontext_pop_node(context);
        identifier = pcontext_pop_node(context);
        enum_with_const = (binary_node*) make_binary_node(TKT_KW_ENUM);
        enum_with_const->lhs = identifier;
        enum_with_const->rhs = const_expr;
        pcontext_push_node(context, TO_NODE_BASE(enum_with_const));
        return true;
      } else {
        die("enumerator: expected constant expression after '=' token");
      }
    } else {
      // just a terminal_node
        return true; // without optional part;
    }
  }
  return false;
}

static
DECL_IS_FUNC_DECLARE(enumerator_list)
{
  util_shift_one_token(context);
  if (util_is_separated_list(context, TKT_COMMA, 
        decl_is_enumerator,
        false /* allow_empty */ ))
  {
    decl_is_optional_comma(context);
    if (util_is_braceR(context)) {
      return true;
    } die("enumerator_list: expected '}' after list of enumerators");
  }
  return false;
}

DECL_IS_FUNC_DECLARE(enum_specifier)
{
  // enum_specifier := 'enum' 
  // ( identifier | identifier '{' list '}' | '{' list '}' )
  util_shift_one_token(context); // enum
  node_base *tag, *body;
  switch (TOKEN_TYPE(t)) {
    case TKT_IDENTIFIER:
      util_shift_one_token(context);
      pcontext_push_node(context, make_terminal_node(t));
      t=util_read_first_token(context);
      if (!util_is_braceL(context)) {
        reduce_enum_struct_union(context, 
            TKT_KW_ENUM, /* what */
            pcontext_pop_node(context), /* tag */
            make_nullary_node() /* body */
            );
        return true;
      }
      if (decl_is_enumerator_list(context)) {
        tag=pcontext_pop_node(context); /* tag */
        body=pcontext_pop_node(context);/* body */
        reduce_enum_struct_union(context, TKT_KW_ENUM, tag, body);
        return true;
      } die("enum_specifier: expected enumerator list after '{' token");
    case TKT_LEFT_BRACE:
      if (decl_is_enumerator_list(context)) {
        reduce_enum_struct_union(context,
            TKT_KW_ENUM, /* what */
            make_nullary_node(), /* tag */
            pcontext_pop_node(context) /* body */
            );
        return true;
      } die("enum_specifier: expected enumerat list after '{' token");
    default:
      return false;
  }
}
