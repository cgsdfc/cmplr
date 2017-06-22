/* string_literal.c */
#include "string_literal.h"

static node string_escaped [] = {
  
  [ES_BEGIN]=TK_STRING_LITERAL_ESCAPED,  
  [ES_HEX_BEGIN]=TK_STRING_LITERAL_HEX_BEGIN,
  [ES_HEX_END]=TK_STRING_LITERAL_HEX_END, 
  [ES_OCT_BEGIN]=TK_STRING_LITERAL_OCT_BEGIN,
  [ES_OCT_END]=TK_STRING_LITERAL_OCT_END, 
  [ES_ZERO]=TK_STRING_LITERAL_ZERO,
  [ES_END]=TK_STRING_LITERAL_END,
  [ES_PART]=TK_STRING_LITERAL_PART
};


void init_string_literal(void)
{
  /* => " */
  add_initial(TK_STRING_LITERAL_BEGIN,
      CHAR_CLASS_DOUBLE_QUOTE);

  /* => "a */
  /* we can not use CHAR_CLASS_CHAR_LITERAL here */
  /*   since it contains '\"' */
  /*   where the char_class needed */
  /*   here is [^ \n\r\\\" ] */
  /* => "ab */
  add_selfloop_rev (TK_STRING_LITERAL_BEGIN,
      CHAR_CLASS_DOUBLE_QUOTE_NEWLINE_BACKSLASH);
  add_intermedia_rev(TK_STRING_LITERAL_PART,
      TK_STRING_LITERAL_BEGIN, CHAR_CLASS_DOUBLE_QUOTE);
  
  /* => "a\ or "\ */
  /* going back to TK_STRING_LITERAL_BEGIN is */
  /*   very important */
  add_intermedia(TK_STRING_LITERAL_BEGIN,
      TK_STRING_LITERAL_ESCAPED,
      CHAR_CLASS_BACKSLASH);

  add_escape_all(string_escaped, CHAR_CLASS_DOUBLE_QUOTE);

  /* => "" empty */
  add_accepted(TK_STRING_LITERAL_BEGIN,
      TK_STRING_LITERAL_END, CHAR_CLASS_DOUBLE_QUOTE);

  /* => "\a" */
  add_accepted(TK_STRING_LITERAL_PART,
      TK_STRING_LITERAL_END, CHAR_CLASS_DOUBLE_QUOTE);

}

void check_string_literal(void)
{
#if 0
  (check_can_transfer(TK_INIT,
                      TK_STRING_LITERAL_BEGIN,
                      CHAR_CLASS_DOUBLE_QUOTE));

  (check_can_transfer(TK_STRING_LITERAL_BEGIN,
                      TK_STRING_LITERAL_BEGIN,
                      CHAR_CLASS_DOUBLE_QUOTE_NEWLINE));

  (check_can_transfer(TK_STRING_LITERAL_PART,
                      TK_STRING_LITERAL_BEGIN, CHAR_CLASS_DOUBLE_QUOTE));


  (check_can_transfer(TK_STRING_LITERAL_BEGIN,
                      TK_STRING_LITERAL_ESCAPED));

  (check_can_transfer(string_escaped, CHAR_CLASS_DOUBLE_QUOTE));

  (check_can_transfer(TK_STRING_LITERAL_BEGIN,
                      TK_STRING_LITERAL_END, CHAR_CLASS_DOUBLE_QUOTE));
#endif

}
