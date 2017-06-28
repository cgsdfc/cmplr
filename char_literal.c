/* char_literal.c */
#include "char_literal.h"

void init_char_literal(void)
{
  /* initial */
  add_initial(TK_CHAR_LITERAL_BEGIN,CHAR_CLASS_SINGLE_QUOTE);

  /* intermedia */
  add_intermedia_rev(TK_CHAR_LITERAL_BEGIN,TK_CHAR_LITERAL_PART,CHAR_CLASS_SINGLE_QUOTE_NEWLINE_BACKSLASH);
  add_intermedia(TK_CHAR_LITERAL_BEGIN,TK_CHAR_LITERAL_ESCAPED,CHAR_CLASS_BACKSLASH);
  add_intermedia_rev(TK_CHAR_LITERAL_ESCAPED,TK_CHAR_LITERAL_PART,CHAR_CLASS_NEWLINE);

  add_selfloop_rev(TK_CHAR_LITERAL_PART, CHAR_CLASS_SINGLE_QUOTE_NEWLINE_BACKSLASH);
  add_accepted(TK_CHAR_LITERAL_PART, TK_CHAR_LITERAL_END,CHAR_CLASS_SINGLE_QUOTE);
}

#if 0
void init_char_literal(void)
{
  int cc_sq=alloc_char_class("\'");
  int cc_nl_rev=alloc_char_class_rev("\n\r");
  int cc_sq_nl_bs_rev=alloc_char_class_rev("\'\n\r\\");
  TK_CHAR_LITERAL_END=alloc_state(false);
  TK_CHAR_LITERAL_PART=alloc_state(true);
  TK_CHAR_LITERAL_BEGIN=alloc_state(true);
  TK_CHAR_LITERAL_ESCAPED=alloc_state(true);

  config_from(0);
    config_condition(cc_sq);
      add_to(TK_CHAR_LITERAL_BEGIN);
  config_from(TK_CHAR_LITERAL_BEGIN);
    config_condition(cc_sq_nl_bs_rev);
      add_to(TK_CHAR_LITERAL_PART);
    config_condition(cc_bs);
      add_to(TK_CHAR_LITERAL_ESCAPED);
  config_from(TK_CHAR_LITERAL_ESCAPED);
    config_condition(cc_nl_rev);
      add_to(TK_CHAR_LITERAL_PART);
  config_from(TK_CHAR_LITERAL_PART);
    config_condition(cc_sq_nl_bs_rev);
      add_to(TK_CHAR_LITERAL_PART);
    config_condition(cc_sq);
      add_to(TK_CHAR_LITERAL_END);

}

#endif
