/* string_literal.c */
#include "string_literal.h"

//TODO: concat of adjacent string 
// use a new accepted state to build 
// a long string
void init_string_literal(void)
{
  add_initial(TK_STRING_LITERAL_BEGIN,CHAR_CLASS_DOUBLE_QUOTE);
  add_selfloop_rev(TK_STRING_LITERAL_BEGIN, CHAR_CLASS_DOUBLE_QUOTE_NEWLINE_BACKSLASH);
  add_intermedia(TK_STRING_LITERAL_BEGIN,TK_STRING_LITERAL_ESCAPED,CHAR_CLASS_BACKSLASH);
  add_intermedia_rev(TK_STRING_LITERAL_ESCAPED,TK_STRING_LITERAL_BEGIN,CHAR_CLASS_NEWLINE);
  add_accepted(TK_STRING_LITERAL_BEGIN, TK_STRING_LITERAL_END,CHAR_CLASS_DOUBLE_QUOTE);
}
