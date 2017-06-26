/* integer_literal.c */
#include "integer_literal.h"

// TODO: LL long long 
void init_integer_literal(void)
{
  /* initial */ 
  add_initial(TK_INT_ZERO, CHAR_CLASS_ZERO);
  add_initial(TK_INT_BEGIN,CHAR_CLASS_DEC_BEGIN);

  /* intermedia */
  add_intermedia(TK_INT_ZERO,TK_INT_OCT_BEGIN,CHAR_CLASS_OCT_BEGIN);
  add_intermedia(TK_INT_ZERO,TK_INT_HEX_BEGIN,CHAR_CLASS_XX);

  /* selfloop */
  add_selfloop(TK_INT_BEGIN,CHAR_CLASS_DEC_PART);
  add_selfloop(TK_INT_HEX_BEGIN,CHAR_CLASS_HEX_BEGIN);
  add_selfloop(TK_INT_OCT_BEGIN,CHAR_CLASS_OCT_BEGIN);

  /* intermedia */
  add_intermedia(TK_INT_BEGIN,TK_INT_UNSIGNED, CHAR_CLASS_U);
  add_intermedia(TK_INT_BEGIN,TK_INT_LONG,CHAR_CLASS_L);

  add_intermedia(TK_INT_ZERO,TK_INT_UNSIGNED, CHAR_CLASS_U);
  add_intermedia(TK_INT_ZERO,TK_INT_LONG,CHAR_CLASS_L);

  add_intermedia(TK_INT_OCT_BEGIN,TK_INT_UNSIGNED, CHAR_CLASS_U);
  add_intermedia(TK_INT_OCT_BEGIN,TK_INT_LONG,CHAR_CLASS_L);

  add_intermedia(TK_INT_HEX_BEGIN,TK_INT_UNSIGNED, CHAR_CLASS_U);
  add_intermedia(TK_INT_HEX_BEGIN,TK_INT_LONG,CHAR_CLASS_L);

  add_intermedia(TK_INT_UNSIGNED,TK_INT_LONG,CHAR_CLASS_L);
  add_intermedia(TK_INT_LONG,TK_INT_UNSIGNED,CHAR_CLASS_U);

  /* accepted_rev */
  add_accepted_rev(TK_INT_ZERO,TK_INT_END,CHAR_CLASS_OCT_BEGIN_X_U_L_PERIOD);
  add_accepted_rev(TK_INT_HEX_BEGIN,TK_INT_END,CHAR_CLASS_HEX_BEGIN_U_L);
  add_accepted_rev(TK_INT_OCT_BEGIN,TK_INT_END,CHAR_CLASS_OCT_BEGIN_U_L);
  add_accepted_rev(TK_INT_BEGIN,TK_INT_END,CHAR_CLASS_DEC_PART_PERIOD_U_L);

  /* accepted */
  add_accepted_rev(TK_INT_LONG,TK_INT_END,CHAR_CLASS_U);
  add_accepted_rev(TK_INT_UNSIGNED,TK_INT_END,CHAR_CLASS_L);

}
