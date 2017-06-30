/* integer_literal.c */
#include "integer_literal.h"

void init_integer_literal(void)
{
  /* initial */ 
  add_initial(TK_INT_ZERO, CHAR_CLASS_ZERO);
  add_initial(TK_INT_DEC_BEGIN,CHAR_CLASS_DEC_DIGITS_NON_ZERO);

  /* intermedia */
  add_intermedia(TK_INT_ZERO,TK_INT_OCT_BEGIN,CHAR_CLASS_OCT_DIGITS);
  add_intermedia(TK_INT_ZERO,TK_INT_HEX_PREFIX, CHAR_CLASS_XX);
  add_intermedia(TK_INT_HEX_PREFIX,TK_INT_HEX_BEGIN,CHAR_CLASS_HEX_DIGITS);

  /* selfloop */
  add_selfloop(TK_INT_DEC_BEGIN,CHAR_CLASS_DEC_DIGITS);
  add_selfloop(TK_INT_HEX_BEGIN,CHAR_CLASS_HEX_DIGITS);
  add_selfloop(TK_INT_OCT_BEGIN,CHAR_CLASS_OCT_DIGITS);
  add_selfloop(TK_INT_SUFFIX, CHAR_CLASS_INT_SUFFIX);

  /* intermedia */
  add_intermedia(TK_INT_DEC_BEGIN,TK_INT_SUFFIX, CHAR_CLASS_INT_SUFFIX);
  add_intermedia(TK_INT_OCT_BEGIN,TK_INT_SUFFIX, CHAR_CLASS_INT_SUFFIX);
  add_intermedia(TK_INT_HEX_BEGIN,TK_INT_SUFFIX, CHAR_CLASS_INT_SUFFIX);
  add_intermedia(TK_INT_ZERO,TK_INT_SUFFIX, CHAR_CLASS_INT_SUFFIX);

  /* accepted */
  add_accepted_rev (TK_INT_SUFFIX,TK_INT_END,CHAR_CLASS_INT_SUFFIX);
  add_accepted_rev (TK_INT_DEC_BEGIN,TK_INT_END,CHAR_CLASS_DEC_DIGITS_INT_SUFFIX_DOT_E);
  add_accepted_rev (TK_INT_OCT_BEGIN,TK_INT_END,CHAR_CLASS_OCT_DIGITS_INT_SUFFIX_DOT);
  add_accepted_rev (TK_INT_HEX_BEGIN,TK_INT_END,CHAR_CLASS_HEX_DIGITS_INT_SUFFIX_DOT);
  add_accepted_rev (TK_INT_ZERO,TK_INT_END,CHAR_CLASS_OCT_DIGITS_INT_SUFFIX_DOT_X_E);
}

#if 0
int TK_INT_END;
void init_integer_literal(void)
{
  int zero=alloc_char_class("\\Z");
  int dec=alloc_char_class("\\d");
  int Oct=alloc_char_class("\\O");
  int Hex=alloc_char_class("\\H");
  int suffix=alloc_char_class("\\I");
  int x=alloc_char_class("\X");
  int oct_suffix_dot=alloc_char_class("\\O\\I.");
  int hex_suffix_dot=alloc_char_class("\\H\\I.");
  int dec_suffix_dot_e=alloc_char_class("\\D.\\E");

  int dec_begin=alloc_state(true);
  int oct_begin=alloc_state(true);
  int hex_begin=alloc_state(true);
  int int_suffix=alloc_state(true);
  int hex_prefix=alloc_state(true);


  config_action(TKA_ALLOC_BUF);
    config_from(0);
      config_condition(zero);
        add_to(TK_INT_ZERO);
      config_condition(dec);
        add_to(dec_begin);
  config_action(TKA_COLLECT_CHAR);
    config_from(TK_INT_ZERO);
      config_condition(Oct);
        add_to(oct_begin);
      config_condition(x);
        add_to(hex_prefix);
    config_from(hex_prefix);
      config_condition(Hex);
        add_to(hex_begin);
    config_from(dec_begin);
      config_condition(Dec);
        add_to(dec_begin);
    config_from(oct_begin);
      config_condition(Oct);
        add_to(oct_begin);
    config_from(hex_begin);
      config_condition(Hex);
        add_to(hex_begin);
    config_to(int_suffix);
      config_condition(suffix);
        add_from(dec_begin);
        add_from(oct_begin);
        add_from(hex_begin);
        add_from(TK_INT_ZERO);
  config_action(TKA_ACC_LIT);
    config_to(TK_INT_END);
      config_usrd(true);
        config_condition(suffix);
          add_from(int_suffix);
        config_condition(dec_suffix_dot_e);
          add_from(dec_begin);
        config_condition(oct_suffix_dot);
          add_from(oct_begin);
        config_condition(hex_suffix_dot);
          add_from(hex_begin);
        config_condition(oct_suffix_doc_x_e);
          add_from(TK_INT_ZERO);
  config_end();
}

#endif


