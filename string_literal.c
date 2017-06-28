/* string_literal.c */
#include "string_literal.h"

//TODO: concat of adjacent string 
// use a new accepted state to build 
// a long string
// let parser do it
void init_string_literal(void)
{
  add_initial(TK_STRING_LITERAL_BEGIN,CHAR_CLASS_DOUBLE_QUOTE);
  // string doesnot require at least one char
  add_selfloop_rev(TK_STRING_LITERAL_BEGIN, CHAR_CLASS_DOUBLE_QUOTE_NEWLINE_BACKSLASH);
  add_intermedia(TK_STRING_LITERAL_BEGIN,TK_STRING_LITERAL_ESCAPED,CHAR_CLASS_BACKSLASH);
  add_intermedia_rev(TK_STRING_LITERAL_ESCAPED,TK_STRING_LITERAL_BEGIN,CHAR_CLASS_NEWLINE);
  add_accepted(TK_STRING_LITERAL_BEGIN, TK_STRING_LITERAL_END,CHAR_CLASS_DOUBLE_QUOTE);
}

#if 0
int TK_STRING_LITERAL_END;
void init_string_literal(void)
{
  int TK_STRING_LITERAL_BEGIN=alloc_state(true);
  int TK_STRING_LITERAL_ESCAPED=alloc_state(true);
  TK_STRING_LITERAL_END=alloc_state(false);
  int cc_dq_nl_bs=alloc_char_class("\"\n\\");
  int cc_nl=alloc_char_class("\n\r");
  int cc_dq=alloc_char_class("\"");
  int cc_dq_nl_bs_rev=alloc_char_class("\"\n\\");

  config_action(TKA_ALLOC_BUF);
    config_from(0);
      config_condition(cc_dq);
        add_to(TK_STRING_LITERAL_BEGIN);

  config_from(TK_STRING_LITERAL_BEGIN);
    config_action(TKA_COLLECT_CHAR);
      config_condition(cc_bs);
        add_to(TK_STRING_LITERAL_ESCAPED);
      config_condition(cc_dq_nl_bs_rev);
        add_to(TK_STRING_LITERAL_BEGIN);
    config_action(TKA_ACC_LIT);
      config_condition(cc_dq);
        add_to(TK_STRING_LITERAL_END);

  config_from(TK_STRING_LITERAL_ESCAPED);
    config_action(TKA_COLLECT_CHAR);
      add_to(TK_STRING_LITERAL_BEGIN);
}
#endif
