#include "identifier.h"


void init_identifier(void)
{
  add_initial(TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);
  add_selfloop (TK_IDENTIFIER_BEGIN, CHAR_CLASS_IDENTIFIER_PART);
  add_accepted(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END, CHAR_CLASS_SEPARATOR);
}


void init_punctuation(void)
{
}

