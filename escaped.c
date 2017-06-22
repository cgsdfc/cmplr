#include "escaped.h"

/** caller should make sure the
 * transfer: whatever -> ES_BEGIN
 * has been made
 */
void add_escape_all (node *state, char_class_enum quote)
{
  add_escape_hex_literal(state,quote);
  add_escape_oct_literal(state,quote);
  add_escape_single(state,quote);
}

void add_escape_hex_literal(node *state, char_class_enum quote)
{

  /* => '\x */
  add_intermedia(state[ES_BEGIN],state[ES_HEX_BEGIN],
     CHAR_CLASS_X);

  /* => '\xf */
  add_intermedia(state[ES_HEX_BEGIN],state[ES_HEX_END],
      CHAR_CLASS_HEX_BEGIN);

  /* => '\xf' */
  add_accepted(state [ES_HEX_END], state [ES_END],
      quote);

  /* => '\xff */
  add_intermedia(state [ES_HEX_END],state [ES_PART],
      CHAR_CLASS_HEX_BEGIN);
}

void add_escape_oct_literal(node *state, char_class_enum quote)
{
  /* => '\0 */
  add_intermedia(state[ES_ZERO],state[ES_OCT_END],
      CHAR_CLASS_OCT_BEGIN);

  /* => '\7 */
  add_intermedia(state[ES_BEGIN],state[ES_OCT_BEGIN],
      CHAR_CLASS_OCT_BEGIN_NOT_ZERO);

  /* => '\7' */
  add_accepted(state[ES_OCT_BEGIN],state [ES_END],
      quote);


  /* => '\71 */
  add_intermedia(state[ES_OCT_BEGIN],state [ES_OCT_END],
      CHAR_CLASS_OCT_BEGIN);

  /* => '\713 */
  add_intermedia(state[ES_OCT_END],state [ES_PART],
      CHAR_CLASS_OCT_BEGIN);

  /* => '\71' */
  add_accepted(state[ES_OCT_END],state [ES_END],
      quote);

}

void add_escape_single(node *state, char_class_enum quote)
{
  /* => '\a (a!=0) */
  add_intermedia(state[ES_BEGIN], state[ES_PART],
      CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO);

  /* => '\0 */
  add_intermedia(state[ES_BEGIN], state[ES_ZERO],CHAR_CLASS_ZERO);

  /* => '\0' */
  add_accepted(state[ES_ZERO],state [ES_END],quote);

}
