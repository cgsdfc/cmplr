#include "recursive/context.h"
#include "recursive/node_base.h"

int pcontext_init(pcontext * context, Lexer * lexer)
{
	if (!context || !lexer) {
		return 1;
	}
	memset(context, 0, sizeof *context);
	context->lexer = lexer;
	utillib_vector_init(&(context->subtrees));
	utillib_error_stack_init(&(context->error));
	return 0;
}

Token *pcontext_read_token(pcontext * context, unsigned pos)
{
	assert(pos < PCONTEXT_MAX_LOOKAHEAD);
	Token **lookaheads = context->lookaheads;
	for (int i = 0; i <= pos; i++) {
		if (!lookaheads[i]) {
			lookaheads[i] = LexerGetToken(context->lexer);
		}
	}
	return lookaheads[pos];
}

void pcontext_shift_token(pcontext * context, unsigned num)
{
	assert(num <= PCONTEXT_MAX_LOOKAHEAD);
	Token **lookaheads = context->lookaheads;
	unsigned limit = PCONTEXT_MAX_LOOKAHEAD - num;
	// from pos to limit, shift one position
	for (int i = 0; i < limit; ++i) {
		lookaheads[i] = lookaheads[i + num];
	}
}

void pcontext_push_node(pcontext * context, node_base * node)
{
	int r;
	r = utillib_vector_push_back(&(context->subtrees), node);
	assert(r == 0);
}

node_base *pcontext_pop_node(pcontext * context)
{
	return utillib_vector_pop_back(&(context->subtrees));
}

node_base *pcontext_top_node(pcontext * context)
{
	node_base *root = utillib_vector_back(&(context->subtrees));
	assert(root != NULL);
	return root;
}

size_t pcontext_node_size(pcontext * context)
{
	return utillib_vector_size(&(context->subtrees));
}

bool pcontext_get_unary_ontop(struct pcontext * context)
{
	return context->unary_on_top;
}

void pcontext_set_unary_ontop(struct pcontext *context, bool ontop)
{
	context->unary_on_top = ontop;
}

void pcontext_mark_prefix(struct pcontext *context, pcontext_prefix prefix,
			  bool value)
{
	assert(prefix >= 0 && prefix < PCONTEXT_N_PREFIX);
	context->prefix[prefix] = value;
}

bool pcontext_test_prefix(struct pcontext *context, pcontext_prefix prefix)
{
	assert(prefix >= 0 && prefix < PCONTEXT_N_PREFIX);
	return context->prefix[prefix];
}

void pcontext_reduce_ternary(pcontext * context)
{
	assert(pcontext_node_size(context) >= 3);
	ternary_node *ternary = (ternary_node *) make_ternary_node();
	ternary->third = pcontext_pop_node(context);
	ternary->second = pcontext_pop_node(context);
	ternary->first = pcontext_pop_node(context);
	pcontext_push_node(context, TO_NODE_BASE(ternary));
}

void pcontext_reduce_unary(pcontext * context, int what)
{
	assert(pcontext_node_size(context) >= 1);
	unary_node *unary = (unary_node *) make_unary_node(what);
	unary->operand = pcontext_pop_node(context);
	pcontext_push_node(context, TO_NODE_BASE(unary));
}

static void pcontext_reduce_binary_impl(pcontext * context, int what,
					node_base * lhs, node_base * rhs)
{
	binary_node *binary = (binary_node *) make_binary_node(what);
	binary->rhs = rhs;
	binary->lhs = lhs;
	pcontext_push_node(context, TO_NODE_BASE(binary));
}

void pcontext_reduce_binary(pcontext * context, int what)
{
	assert(pcontext_node_size(context) >= 2);
	node_base *rhs = pcontext_pop_node(context);
	node_base *lhs = pcontext_pop_node(context);
	pcontext_reduce_binary_impl(context, what, lhs, rhs);
}

void pcontext_reduce_binaryR(pcontext * context, int what)
{
	assert(pcontext_node_size(context) >= 2);
	node_base *lhs = pcontext_pop_node(context);
	node_base *rhs = pcontext_pop_node(context);
	pcontext_reduce_binary_impl(context, what, lhs, rhs);
}

void pcontext_reduce_terminal(pcontext * context, Token * t)
{
	pcontext_push_node(context, make_terminal_node(t));
}

void pcontext_reduce_vector(pcontext * context, size_t size)
{
	assert(pcontext_node_size(context) >= size);
	vector_node *vec = (vector_node *) make_vector_node();
	for (int i = 0; i < size; ++i) {
		vector_node_push_back(vec, pcontext_pop_node(context));
	}
	pcontext_push_node(context, TO_NODE_BASE(vec));
}

bool pcontext_push_error()
{
}
