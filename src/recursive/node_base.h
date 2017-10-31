#ifndef RECURSIVE_NODE_BASE_H
#define RECURSIVE_NODE_BASE_H
#include "lexer/lexer.h"
#include "utillib/enum.h"
#include "utillib/vector.h"
#define TO_NODE_BASE(NODE) ((node_base *)NODE)
#define NODE_TAG(NODE) ((NODE)->tag)
#define FOR_NODE_HEAD(NODE) ((ternary_node *)((NODE)->lhs))
#define FOR_NODE_BODY(NODE) ((NODE)->rhs)
#define BINARY_NODE_OP(NODE) ((NODE)->op)
#define UNARY_NODE_OP(NODE) ((NODE)->op)
#define IF_NODE_BODY(NODE) ((ternary_node *)(NODE)->operand)

UTILLIB_ENUM_BEGIN(node_tag)
    UTILLIB_ENUM_ELEM(NODE_TAG_NULLARY)
    UTILLIB_ENUM_ELEM(NODE_TAG_TERMINAL)
    UTILLIB_ENUM_ELEM(NODE_TAG_UNARY)
    UTILLIB_ENUM_ELEM(NODE_TAG_BINARY)
    UTILLIB_ENUM_ELEM(NODE_TAG_TERNARY)
    UTILLIB_ENUM_ELEM(NODE_TAG_VECTOR)
    UTILLIB_ENUM_ELEM(NODE_TAG_EXPR)
    UTILLIB_ENUM_ELEM(NODE_TAG_STMT)
    UTILLIB_ENUM_ELEM(NODE_TAG_DECL)
    UTILLIB_ENUM_END(node_tag)
typedef struct node_base {
	node_tag tag;
} node_base;
typedef struct terminal_node {
	node_base base;
	Token *token;
} terminal_node;

typedef struct binary_node {
	node_base base;
	int op;
	node_base *lhs, *rhs;
} binary_node;

typedef struct unary_node {
	node_base base;
	int op;
	node_base *operand;
} unary_node;

typedef struct vector_node {
	node_base base;
	utillib_vector vec;
} vector_node;

typedef struct ternary_node {
	node_base base;
	node_base *first, *second, *third;
} ternary_node;

typedef struct nullary_node {
	// this node is purely used to carry
	// syntax component info, expr, stmt, decl.
	node_base base;
	node_base *what;
} nullary_node;

// the visitor function
typedef void (vfunction) (void * /* visitor_data */ ,
			  void * /* node_base */ );
// the visitor factory function
typedef vfunction *(vfactory) (void *, node_tag);

void init_node_base(struct node_base *, node_tag);
node_base *make_ternary_node(void);
node_base *make_unary_node(int);
node_base *make_binary_node(int);
node_base *make_terminal_node(Token *);
node_base *make_vector_node(void);
int vector_node_push_back(struct vector_node *, struct node_base *);
node_base *vector_node_at(struct vector_node *, size_t);
size_t vector_node_size(struct vector_node *);
void destroy_vector_node(struct vector_node *);
node_base *make_nullary_node(void);
void visit_node(void *, struct node_base *, vfactory *);
#endif				// RECURSIVE_NODE_BASE_H
