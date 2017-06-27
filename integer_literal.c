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

  /* intermedia */
  add_intermedia(TK_INT_DEC_BEGIN,TK_INT_SUFFIX_0, CHAR_CLASS_INT_SUFFIX);
  add_intermedia(TK_INT_OCT_BEGIN,TK_INT_SUFFIX_0, CHAR_CLASS_INT_SUFFIX);
  add_intermedia(TK_INT_HEX_BEGIN,TK_INT_SUFFIX_0, CHAR_CLASS_INT_SUFFIX);
  add_intermedia(TK_INT_ZERO,TK_INT_SUFFIX_0, CHAR_CLASS_INT_SUFFIX);

  add_intermedia(TK_INT_SUFFIX_0,TK_INT_SUFFIX_1,CHAR_CLASS_INT_SUFFIX);
  add_intermedia(TK_INT_SUFFIX_1,TK_INT_END,CHAR_CLASS_INT_SUFFIX);

  /* accepted */
  add_accepted(TK_INT_SUFFIX_0,TK_INT_END,CHAR_CLASS_SEPARATOR);
  add_accepted(TK_INT_SUFFIX_1,TK_INT_END,CHAR_CLASS_SEPARATOR);
  add_accepted(TK_INT_DEC_BEGIN,TK_INT_END,CHAR_CLASS_SEPARATOR);
  add_accepted(TK_INT_OCT_BEGIN,TK_INT_END,CHAR_CLASS_SEPARATOR);
  add_accepted(TK_INT_HEX_BEGIN,TK_INT_END,CHAR_CLASS_SEPARATOR);
  add_accepted(TK_INT_ZERO,TK_INT_END,CHAR_CLASS_SEPARATOR);
}
