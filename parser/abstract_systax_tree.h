#ifndef ABSTRACT_SYSTAX_TREE 
#define ABSTRACT_SYSTAX_TREE 1

typedef struct ast_node 
{
  ast_node_type type;
} ast_node;

// for terminals: literals, identifiers
typedef struct terminal_ast_node
{
  ast_node _node;
  token *tk;
} terminal_ast_node;

// for fixlen constructs like 
// while (cond) { body }
// for (init;cond;incr) { body }
// 
typedef struct fixlen_ast_node
{
  ast_node _node;
  ast_node *children[AST_FIXLEN_LEN];
} fixlen_ast_node;

// the node that has arbitrary children
// 
typedef struct varlen_ast_node
{
  ast_node _node;
  ast_node **children;
} varlen_ast_node;

typedef struct binary_ast_node
{
  ast_node _node;
  ast_node *left;
  ast_node *right;
} binary_ast_node;


#endif

