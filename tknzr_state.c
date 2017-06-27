#include "tknzr_state.h"
#include <string.h>

char *tknzr_state_string(tknzr_state state)
{
  static char buf[BUFSIZ];
  strncpy(buf, tknzr_state_tab[state],BUFSIZ);
  return buf;
}

bool is_operator_accept (tknzr_state state)
{
  return _TK_OPERATOR_ACCEPT_BEGIN < state && state < _TK_OPERATOR_ACCEPT_END;
}

bool is_varlen_init (tknzr_state state)
{
  return _TK_VARLEN_INIT_BEGIN < state && state < _TK_VARLEN_INIT_END;
}

bool is_varlen_accept (tknzr_state state)
{
  return _TK_VARLEN_ACCEPT_BEGIN < state && state < _TK_VARLEN_ACCEPT_END;
}

bool is_punctuation_char (char ch)
{
  return is_punctuation_accept(ch);
}

bool is_punctuation_accept (char ch)
{
  return char_is_in_class(ch, CHAR_CLASS_PUNCTUATION);
}

bool is_operator_char(char ch)
{
  return char_is_in_class(ch, CHAR_CLASS_OPERATOR);
}

