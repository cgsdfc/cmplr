#include "digraph.h"
#include "grammar.h"
#include "item.h"
#include "lookahead_set.h"
#include "rules.h"

digraph_struct digraph_clang;

int main(int argc, char **argv)
{
	/* init_simple(); */
	/* init_easy(); */
	/* init_quick(&grammar_clang); */
	init_clang();
	build_item_set(&grammar_clang);
	init_digraph(&digraph_clang);
	compute_direct_read_relation(&digraph_clang, &grammar_clang);
	digraph(&digraph_clang);

	/* print_item_set_all(&grammar_clang); */
	/* print_core(&grammar_clang); */
}
