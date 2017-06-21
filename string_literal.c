#include "string_literal.h"

void init_string_literal(void)
{
#if 0
  add_initial(TK_STRING_LITERAL_BEGIN,
      CHAR_CLASS_DOUBLE_QUOTE);

  add_selfloop_rev (TK_STRING_LITERAL_BEGIN,
      CHAR_CLASS_DOUBLE_QUOTE CHAR_CLASS_NEWLINE);

  add_intermedia(TK_STRING_LITERAL_BEGIN,
      TK_STRING_ESCAPED_BEGIN,0)

  add_accepted(TK_STRING_LITERAL_BEGIN,
      TK_STRING_LITERAL_END, CHAR_CLASS_DOUBLE_QUOTE);
#endif

}

