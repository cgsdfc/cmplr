#include "identifier.h"


void init_identifier(void)
{
  add_initial(TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);
  add_selfloop (TK_IDENTIFIER_BEGIN, CHAR_CLASS_IDENTIFIER_PART);
  add_accepted_rev(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END, CHAR_CLASS_IDENTIFIER_PART);
}


void init_punctuation(void)
{
  add_initial(TK_PUNCTUATION_BEGIN,CHAR_CLASS_PUNCTUATION);
  add_accepted_rev (TK_PERIOD,TK_PERIOD_END,CHAR_CLASS_DEC_PART);
  add_accepted_rev(TK_PUNCTUATION_BEGIN, TK_PUNCTUATION_END, CHAR_CLASS_EMPTY);
}

