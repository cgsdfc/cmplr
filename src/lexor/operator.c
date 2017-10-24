#include "lexer/chcl.h"
#include "lexer/dfa.h"
#include "lexer/tokenizer.h"

#define N_AMPERSAND (sizeof b / sizeof b[0])
#define N_LESS (sizeof c / sizeof c[0])
#define N_EXCLAIM (sizeof a / sizeof a[0])
extern int TK_SLASH;
extern int TK_NEGATIVE;

const static char *a[] = {
    "!", "=", "*", "%", "^",
};

const static char *b[] = {
    "&", "|", "+",
};

const static char *c[] = {
    "<", ">",
};

const static char *B[] = {"&=", "=|", "=+"};

const static char *C[] = {
    "<=", ">=",
};

void init_operator(void) {
  int equal = alloc_char_class("=");
  int accepted = alloc_state(false);
  int begin;
  int middle;
  int op;
  int op_equal;

  for (int i = 0; i < N_EXCLAIM; ++i) {
    op = alloc_char_class(a[i]);
    begin = alloc_state(true);

    config_action(TKA_ALLOC_BUF);
    config_from(0);
    config_condition(op);
    add_to(begin);

    config_action(TKA_ACC_OPER);
    config_from(begin);
    config_to(accepted);
    config_condition(equal);
    add_config();
    config_usrd(true);
    add_config();
    config_end();
  }
  for (int i = 0; i < N_AMPERSAND; ++i) {
    op = alloc_char_class(b[i]);
    op_equal = alloc_char_class(B[i]);

    begin = alloc_state(true);

    config_action(TKA_ALLOC_BUF);
    config_from(0);
    config_condition(op);
    add_to(begin);

    config_action(TKA_ACC_OPER);
    config_to(accepted);
    config_from(begin);
    config_condition(equal);
    add_config();
    config_condition(op);
    add_config();
    config_condition(op_equal);
    config_usrd(true);
    add_config();
    config_end();
  }

  for (int i = 0; i < N_LESS; ++i) {
    op = alloc_char_class(c[i]);
    op_equal = alloc_char_class(C[i]);

    begin = alloc_state(true);
    middle = alloc_state(true);

    config_from(0);
    config_action(TKA_ALLOC_BUF);
    config_condition(op);
    add_to(begin);

    config_action(TKA_COLLECT_CHAR);
    config_from(begin);
    config_condition(op);
    add_to(middle);

    config_action(TKA_ACC_OPER);
    config_to(accepted);
    config_condition(equal);
    add_from(begin);
    add_from(middle);
    config_usrd(true);
    add_from(middle);
    config_condition(op_equal);
    add_from(begin);

    config_end();
  }

  int slash_begin = TK_SLASH;
  int nega_begin = TK_NEGATIVE;
  int tilde_begin = alloc_state(true);

  config_action(TKA_ALLOC_BUF);
  config_from(0);
  config_condition(alloc_char_class("-"));
  add_to(nega_begin);
  config_condition(alloc_char_class("/"));
  add_to(slash_begin);
  config_condition(alloc_char_class("~"));
  add_to(tilde_begin);

  config_action(TKA_ACC_OPER);
  config_to(accepted);
  config_from(nega_begin);
  config_condition(alloc_char_class(">"));
  add_config();
  config_condition(alloc_char_class("-"));
  add_config();
  config_condition(equal);
  add_config();
  config_condition(alloc_char_class("->="));
  config_usrd(true);
  add_config();
  config_usrd(false);

  config_from(slash_begin);
  config_condition(equal);
  add_config();
  config_condition(alloc_char_class("/*="));
  config_usrd(true);
  add_config();

  config_from(tilde_begin);
  config_usrd(true);
  config_condition(alloc_char_class(""));
  add_config();
  config_end();
}
