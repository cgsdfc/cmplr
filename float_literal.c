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


