#include "float_literal.h"

// TODO Ff mean for float type
// Ll means for long double type
// none means for double type
// they can not combine
// TODO let .222 be recognized

// hex or oct float_literal is not supported
void init_float_literal(void)
{
  // entering fraction part .
  add_intermedia(TK_DOT, TK_FLOAT_FRACTION, CHAR_CLASS_DEC_DIGITS);
  add_intermedia(TK_INT_ZERO, TK_FLOAT_FRACTION, CHAR_CLASS_DOT);
  add_intermedia(TK_INT_DEC_BEGIN,TK_FLOAT_FRACTION,CHAR_CLASS_DOT);

  // entering exponential part e
  add_intermedia(TK_INT_ZERO,TK_FLOAT_EXPONENT_BEGIN,CHAR_CLASS_E);
  add_intermedia(TK_INT_DEC_BEGIN,TK_FLOAT_EXPONENT_BEGIN,CHAR_CLASS_E);
  // exponential must follow fraction
  add_intermedia(TK_FLOAT_FRACTION,TK_FLOAT_EXPONENT_BEGIN,CHAR_CLASS_E);

  // optional sign -/+
  add_intermedia(TK_FLOAT_EXPONENT_BEGIN,TK_FLOAT_SIGN,CHAR_CLASS_SIGN);
  add_intermedia(TK_FLOAT_SIGN,TK_FLOAT_EXPONENT,CHAR_CLASS_DEC_DIGITS);

  /* exponent must have at least one digit */
  add_intermedia(TK_FLOAT_EXPONENT_BEGIN,TK_FLOAT_EXPONENT,CHAR_CLASS_DEC_DIGITS);

  // optional suffix fFlL
  add_intermedia(TK_FLOAT_EXPONENT,TK_FLOAT_SUFFIX, CHAR_CLASS_FLOAT_SUFFIX);
  add_intermedia(TK_FLOAT_FRACTION,TK_FLOAT_SUFFIX, CHAR_CLASS_FLOAT_SUFFIX);

  // collect digits 
  add_selfloop(TK_FLOAT_FRACTION,CHAR_CLASS_DEC_DIGITS);
  add_selfloop(TK_FLOAT_EXPONENT,CHAR_CLASS_DEC_DIGITS);
  // collect suffix 
  add_selfloop(TK_FLOAT_SUFFIX, CHAR_CLASS_FLOAT_SUFFIX);

  add_accepted_rev (TK_FLOAT_EXPONENT, TK_FLOAT_END, CHAR_CLASS_DEC_DIGITS_FLOAT_SUFFIX);
  add_accepted_rev (TK_FLOAT_FRACTION, TK_FLOAT_END, CHAR_CLASS_DEC_DIGITS_FLOAT_SUFFIX_E);
  add_accepted_rev (TK_FLOAT_SUFFIX, TK_FLOAT_END, CHAR_CLASS_DEC_DIGITS_FLOAT_SUFFIX);

}

#if 0
int TK_FLOAT_END;
void init_float_literal(void)
{
 extern int TK_DOT;
 extern int TK_INT_ZERO;
 extern int TK_INT_DEC_BEGIN;
 int TK_FLOAT_FRACTION=alloc_state(true);
 int TK_FLOAT_SIGN=alloc_state(true);
 int TK_FLOAT_EXPONENT=alloc_state(true);
 int TK_FLOAT_EXPONENT_BEGIN=alloc_state(true);
 int TK_FLOAT_END=alloc_state(false);
 int TK_FLOAT_SUFFIX=alloc_state(true);
 int CC_E=alloc_char_class("\E");
 int CC_DOT=alloc_char_class(".");
 int CC_SUFFIX=alloc_char_class("\F");
 int CC_DEC_SUF_E_REV=alloc_char_class_rev("\D\F\E");
 int CC_DEC=alloc_char_class("\D"); 

  config_action (TKA_ALLOC_BUF);
    config_to (TK_FLOAT_FRACTION);
      config_condition(CC_DEC);
        add_from(TK_DOT);
      config_condition (CC_DOT);
        add_from(TK_INT_ZERO);
        add_from (TK_INT_DEC_BEGIN);
  
  config_action(TKA_COLLECT_CHAR);
    config_condition(CC_E);
      config_to(TK_FLOAT_EXPONENT_BEGIN);
        add_from(TK_INT_ZERO);
        add_from(TK_INT_DEC_BEGIN);
        add_from(TK_FLOAT_FRACTION);
    config_condition(CC_SIGN);
      config_to(TK_FLOAT_SIGN);
        add_from(TK_FLOAT_EXPONENT_BEGIN);
    config_condition(CC_DEC);
      config_to(TK_FLOAT_EXPONENT);
        add_from(TK_FLOAT_EXPONENT);
        add_from(TK_FLOAT_SIGN);
      config_to(TK_FLOAT_FRACTION);
        add_from(TK_FLOAT_FRACTION);
    config_condition(CC_SUFFIX);
      config_to(TK_FLOAT_SUFFIX);
        add_from(TK_FLOAT_EXPONENT);
        add_from(TK_FLOAT_FRACTION);
        add_from(TK_FLOAT_SUFFIX);

  config_action(TKA_ACC_LIT);
    config_to(TK_FLOAT_END);
      config_condition(CC_DEC_SUF_REV);
        add_from(TK_FLOAT_EXPONENT);
        add_from(TK_FLOAT_SUFFIX);
      config_condition(CC_DEC_SUF_E_REV);
        add_from(TK_FLOAT_FRACTION);

}
#endif




