#include "char_literal.h"

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

void init_escape_hex_literal(void)
{
  /* => '\ */
  add_intermedia(TK_CHAR_LITERAL_BEGIN,
      TK_CHAR_LITERAL_ESCAPED, CHAR_CLASS_BACKSLASH);

  /* => '\x */
  add_intermedia(TK_CHAR_LITERAL_ESCAPED,TK_CHAR_LITERAL_HEX_BEGIN,
      "x");

  /* => '\xf */
  add_intermedia(TK_CHAR_LITERAL_HEX_BEGIN,TK_CHAR_LITERAL_HEX_END,
      CHAR_CLASS_HEX_BEGIN);

  /* => '\xf' */
  add_accepted(TK_CHAR_LITERAL_HEX_END, TK_CHAR_LITERAL_END,
      CHAR_CLASS_SINGLE_QUOTE);

  /* => '\xff */
  add_intermedia(TK_CHAR_LITERAL_HEX_END,TK_CHAR_LITERAL_PART,
      CHAR_CLASS_HEX_BEGIN);

}

void init_escape_oct_literal(void)
{
  /* => '\0 */
  add_intermedia(TK_CHAR_LITERAL_ZERO,TK_CHAR_LITERAL_OCT_END,
      CHAR_CLASS_OCT_BEGIN);

  /* => '\7 */
  add_intermedia(TK_CHAR_LITERAL_ESCAPED,TK_CHAR_LITERAL_OCT_BEGIN,
      CHAR_CLASS_OCT_BEGIN_NOT_ZERO);

  /* => '\7' */
  add_accepted(TK_CHAR_LITERAL_OCT_BEGIN, TK_CHAR_LITERAL_END,
      CHAR_CLASS_SINGLE_QUOTE);


  /* => '\71 */
  add_intermedia(TK_CHAR_LITERAL_OCT_BEGIN, TK_CHAR_LITERAL_OCT_END,
      CHAR_CLASS_OCT_BEGIN);

  /* => '\713 */
  add_intermedia(TK_CHAR_LITERAL_OCT_END, TK_CHAR_LITERAL_PART,
      CHAR_CLASS_OCT_BEGIN);

  /* => '\71' */
  add_accepted(TK_CHAR_LITERAL_OCT_END, TK_CHAR_LITERAL_END, 
      CHAR_CLASS_SINGLE_QUOTE);

}

void init_escape_single(void)
{
  /* => '\a (a!=0) */
  add_intermedia(TK_CHAR_LITERAL_ESCAPED, TK_CHAR_LITERAL_PART,
      CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO);

  /* => '\0 */
  add_intermedia(TK_CHAR_LITERAL_ESCAPED, TK_CHAR_LITERAL_ZERO,"0");

  /* => '\0' */
  add_accepted(TK_CHAR_LITERAL_ZERO,TK_CHAR_LITERAL_END,CHAR_CLASS_SINGLE_QUOTE);

}

void init_char_literal(void)
{
  init_one_char_literal();
  init_escape_single();
  init_escape_hex_literal();
  init_escape_oct_literal();
 
}

