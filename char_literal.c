#include "char_literal.h"


static node char_escaped [] = {
  
  [ES_BEGIN]=TK_CHAR_LITERAL_ESCAPED,  
  [ES_HEX_BEGIN]=TK_CHAR_LITERAL_HEX_BEGIN,
  [ES_HEX_END]=TK_CHAR_LITERAL_HEX_END, 
  [ES_OCT_BEGIN]=TK_CHAR_LITERAL_OCT_BEGIN,
  [ES_OCT_END]=TK_CHAR_LITERAL_OCT_END, 
  [ES_ZERO]=TK_CHAR_LITERAL_ZERO,
  [ES_END]=TK_CHAR_LITERAL_END,
  [ES_PART]=TK_CHAR_LITERAL_PART
};

void init_one_char_literal(void) 
{
  /* => ' */
  add_initial(TK_CHAR_LITERAL_BEGIN,CHAR_CLASS_SINGLE_QUOTE);

  /* => 'a */
  add_intermedia(TK_CHAR_LITERAL_BEGIN,
      TK_CHAR_LITERAL_PART, CHAR_CLASS_CHAR_LITERAL);

  /* => 'a' */
  add_accepted(TK_CHAR_LITERAL_PART,
      TK_CHAR_LITERAL_END, CHAR_CLASS_SINGLE_QUOTE);

}

void init_char_literal(void)
{
  init_one_char_literal();
  add_intermedia(TK_CHAR_LITERAL_BEGIN,
      TK_CHAR_LITERAL_ESCAPED,
      CHAR_CLASS_BACKSLASH);
  add_escape_all(char_escaped, CHAR_CLASS_SINGLE_QUOTE);
 
}

