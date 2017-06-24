#include "float_literal.h"


void init_float_literal(void)
{
  // forbit .2 .44 float_literal for easier tokenize
  // TODO: put it into error handle 
 /* add_intermedia(TK_PERIOD,TK_FLOAT_BEGIN,CHAR_CLASS_DEC_PART); */
 add_intermedia(TK_INT_ZERO,TK_FLOAT_BEGIN,CHAR_CLASS_PERIOD);
 add_intermedia(TK_INT_BEGIN,TK_FLOAT_BEGIN,CHAR_CLASS_PERIOD);

 add_selfloop(TK_FLOAT_BEGIN,CHAR_CLASS_DEC_PART);
 add_selfloop(TK_FLOAT_EXPONENT,CHAR_CLASS_DEC_PART);

 add_intermedia(TK_FLOAT_BEGIN,TK_FLOAT_EXPONENT,CHAR_CLASS_E);
 add_intermedia(TK_FLOAT_EXPONENT,TK_FLOAT_SIGN,CHAR_CLASS_SIGN);
 add_intermedia(TK_FLOAT_SIGN,TK_FLOAT_EXPONENT,CHAR_CLASS_DEC_PART);

 add_accepted_rev (TK_FLOAT_EXPONENT, TK_FLOAT_END, CHAR_CLASS_DEC_PART);
 add_accepted_rev (TK_FLOAT_BEGIN, TK_FLOAT_END, CHAR_CLASS_DEC_PART);

}




/*   /1* integer, float and period *1/ */
/*   TK_INT_BEGIN, */
/*   TK_INT_ZERO, */
/*   TK_INT_HEX_BEGIN, */
/*   TK_INT_OCT_BEGIN, */
/*   TK_INT_LONG, */
/*   TK_INT_UNSIGNED, */
/*   TK_PERIOD, */
/*   TK_FLOAT_BEGIN, */
/*   TK_FLOAT_FRACTION, */
/*   TK_FLOAT_EXPONENT, */
/*   TK_FLOAT_SIGN, */
/*   TK_FLOAT_EXPONENT_PART, */


