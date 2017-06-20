#ifndef OPERATOR_H
#define OPERATOR_H
#include "state.h"

void init_operator(void);
void add_initial(node from, char *char_class);
void add_intermedia_rev (node from, node to, char *char_class ); 
void add_intermedia (node from, node to, char* char_class); 
void add_selfloop (node from, char *char_class);;
void add_selfloop_rev (node from, char *char_class);;
void add_accepted_rev(node from,  node to, char * char_class); 
void add_accepted(node from, node to , char *char_class); 
void add_tilde_like_operator (tokenizer_state state, char *char_class);;
void init_exclaim_like_operator(void);;
void init_less_like_operator (void); ;
void init_ampersand_like_operator (void);;
#endif

