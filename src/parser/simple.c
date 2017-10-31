#include "item.h"

void init_simple(void)
{
	grammar gr;
	language lang = {
		.name = "Simple",.num_nonterm = 4,.num_symbol = 8,
	};

	init_grammar(&gr, &lang);

	/* non terminal */
	int Goal = def_nonterm(&gr, "Goal");
	int Sum = def_nonterm(&gr, "Sum");
	int Product = def_nonterm(&gr, "Product");
	int Value = def_nonterm(&gr, "Value");

	/* terminal */
	int Int = def_terminal(&gr, "Int");
	int Id = def_terminal(&gr, "Id");
	int plus = def_terminal(&gr, "+");
	int mult = def_terminal(&gr, "*");

	// r0 Goal := Sum eof
	def_rule(&gr, Goal, Sum, -1);
	// r1 Sum := Product
	def_rule(&gr, Sum, Product, -1);
	// r2 Sum := Sum + Product
	def_rule(&gr, Sum, Sum, plus, Product, -1);
	// r3 Product := Value
	def_rule(&gr, Product, Value, -1);
	// r4 Product := Product * Value
	def_rule(&gr, Product, Product, mult, Value, -1);
	// r5 Value := Int
	def_rule(&gr, Value, Int, -1);
	// r5 Value := Id
	def_rule(&gr, Value, Id, -1);

	show_rules(&gr);

	build_item_set(&gr);
	print_item_set_all(&gr);
}

void init_easy(void)
{
	grammar gr;
	language lang = {.name = "Easy",.num_symbol = 7,.num_nonterm = 3 };
	init_grammar(&gr, &lang);

	// nonterms
	int Begin = def_nonterm(&gr, "Begin");
	int E = def_nonterm(&gr, "E");
	int B = def_nonterm(&gr, "B");

	// terminals
	int zero = def_terminal(&gr, "0");
	int one = def_terminal(&gr, "1");
	int plus = def_terminal(&gr, "+");
	int mul = def_terminal(&gr, "*");

	// rules
	// Begin := E eof
	def_rule(&gr, Begin, E, -1);
	// E:=E*B
	def_rule(&gr, E, E, mul, B, -1);
	// E:=E+B
	def_rule(&gr, E, E, plus, B, -1);
	// E:=B
	def_rule(&gr, E, B, -1);
	// B:=0
	def_rule(&gr, B, zero, -1);
	// B:=1
	def_rule(&gr, B, one, -1);

	build_item_set(&gr);
	print_item_set_all(&gr);
}

void init_quick(grammar * gr)
{
	language lang = {
		.name = "quick",.num_symbol = 7,.num_nonterm = 4,
	};

	init_grammar(gr, &lang);
	int S = def_nonterm(gr, "S");
	int N = def_nonterm(gr, "N");
	int V = def_nonterm(gr, "V");
	int E = def_nonterm(gr, "E");

	int equal = def_terminal(gr, "=");
	int x = def_terminal(gr, "x");
	int star = def_terminal(gr, "*");

	def_rule(gr, S, N, -1);
	def_rule(gr, N, V, equal, E, -1);
	def_rule(gr, N, E, -1);
	def_rule(gr, E, V, -1);
	def_rule(gr, V, x, -1);
	def_rule(gr, V, star, E, -1);
	show_rules(gr);
	build_item_set(gr);
	puts("");
	print_item_set_all(gr);
	print_core(gr);
}
