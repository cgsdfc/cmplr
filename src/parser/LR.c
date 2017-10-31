#include "tknzr/dfa.h"
#include "tknzr/token_buffer.h"
#include "tknzr/token_type.h"
#define N_LR_STATES 20

int state[10];
int END;

dfa_table *lr_action;
dfa_table *lr_goto;
typedef enum non_terminal {
	Goal,
	Sums,
	Products,
	Value,
} non_terminal;

typedef enum LR_action { Shift, Reduce, Accept, Error } LR_action;

int cond_equal(dfa_state * entry, int cond)
{
	return entry->cond == cond;
}

void init_action(void)
{
	config_from(0);
	config_action(Shift);
	config_cond(TKT_INTEGER_LITERAL);
	add_to(state[8]);
	config_cond(TKT_IDENTIFIER);
	add_to(state[9]);

	config_from(state[1]);
	config_action(Shift);
	config_cond(TKT_PLUS);
	add_to(state[2]);
	config_action(Accept);
	config_cond(EOF);
	add_to(END);

	config_from(state[2]);
	config_action(Shift);
	config_cond(TKT_INTEGER_LITERAL);
	add_to(state[8]);
	config_cond(TKT_IDENTIFIER);
	add_to(state[9]);

	config_from(state[3]);
	config_cond(TKT_STAR);
	config_action(Shift);
	add_to(state[5]);
	config_cond(TKT_PLUS);
	config_usrd(rule[1]);
	config_action(Reduce);
	add_to(END);
	config_cond(EOF);
	add_to(END);

	config_from(state[4]);
	config_cond(TKT_STAR);
	config_action(Shift);
	add_to(state[5]);
	config_action(Reduce);
	config_cond(TKT_PLUS);
	config_usrd(rule[2]);
	add_to(END);
	config_cond(EOF);
	add_to(END);

	config_from(state[5]);
	config_action(Shift);
	config_cond(TKT_INTEGER_LITERAL);
	add_to(state[8]);
	config_cond(TKT_IDENTIFIER);
	add_to(state[9]);

	config_action(Reduce);
	config_usrd(rule[3]);
	config_to(END);
	config_cond(TKT_STAR);
add_from(state[}

	       void init_states(void) {
	       for (int i = 1; i <= N_LR_STATES; ++i)
	       states[i] = alloc_state(true); END = alloc_state(false);}

	       void init_lr(void) {
	       lr_action = alloc_dfa(N_LR_STATES, cond_equal);
	       lr_goto = alloc_dfa(N_LR_STATES, cond_equal);
	       init_states(); init_action(); init_goto();}

	       int parse_loop(token_buffer * buf) {
	       int state = 0;
	       int symbol = TKT_UNKNOWN;
	       token * tk;
	       dfa_state * entry;
	       push_stack(&state_stack, 0); while (true) {
	       state = stack_top(&state_stack);
	       r = peek_token(buf, &tk);
	       symbol = r == EOF ? EOF : tk->type;
	       r = transfer(state, symbol, &entry); switch (r) {
case 0:
	       switch (entry->action) {
case Shift:
push_stack(&state_stack, entry->state); push_stack(&symbol_stack, symbol); get_token(buf, &tk); break; case Reduce:
rule = entry->usrd; do_reduce(break; case 1:
break; case -1:
break;}
}
}
