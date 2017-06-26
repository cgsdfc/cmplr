#include "float_literal.h"

// TODO Ff mean for float type
// Ll means for long double type
// none means for double type
// they can not combine
// TODO let .222 be recognized


void init_float_literal(void)
{
  // forbit .2 .44 float_literal for easier tokenize
 add_intermedia(TK_INT_ZERO,TK_FLOAT_BEGIN,CHAR_CLASS_PERIOD);
 add_intermedia(TK_INT_BEGIN,TK_FLOAT_BEGIN,CHAR_CLASS_PERIOD);

 add_selfloop(TK_FLOAT_BEGIN,CHAR_CLASS_DEC_PART);
 add_selfloop(TK_FLOAT_EXPONENT,CHAR_CLASS_DEC_PART);

 add_intermedia(TK_FLOAT_BEGIN,TK_FLOAT_EXPONENT,CHAR_CLASS_E);
 add_intermedia(TK_FLOAT_EXPONENT,TK_FLOAT_SIGN,CHAR_CLASS_SIGN);
 add_intermedia(TK_FLOAT_SIGN,TK_FLOAT_EXPONENT,CHAR_CLASS_DEC_PART);

 add_accepted_rev (TK_FLOAT_EXPONENT, TK_FLOAT_END, CHAR_CLASS_DEC_PART_E_SIGN);
 add_accepted_rev (TK_FLOAT_BEGIN, TK_FLOAT_END, CHAR_CLASS_DEC_PART_E);

}


