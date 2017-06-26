#include "tknzr_error.h"
#include "tknzr_state.h"
#include "char_buffer.h"

extern char *filename;
extern char_buffer cbuffer;

static int tknzr_errno;
static
const char *tokenizer_err_tab [] =
{
 [TERR_SUCCESS]="no error, success"
};

tknzr_error  tknzr_error_get(void)
{
  return tknzr_errno;
}

void tknzr_error_set(tknzr_error err)
{
  assert (0<= err && err < _TERR_NULL);
  tknzr_errno = err;
}

void tknzr_error_clear(void)
{
  tknzr_errno = TERR_SUCCESS;
}

char *tknzr_error_string(void)
{
  static char buf[BUFSIZ];
  strncpy(buf,tokenizer_err_tab[tknzr_errno],BUFSIZ);
  return buf;
}

void tknzr_error_handle (tknzr_state last_state)
{
  char *errline = peek_line (&cbuffer, get_lineno(&cbuffer));

  fprintf(stderr, "file %s state %s, line %d, column %d: error: %s\n\n",
      filename,
      tknzr_state_string(last_state),
      cbuffer.pos.lineno,
      cbuffer.pos.column,
      tknzr_error_string());

  fprintf(stderr, "-> %s", errline);

  switch (tknzr_errno) {
    case TERR_UNEXPECTED_END:
      break;

    case TERR_UNEXPECTED_CHAR:
      fprintf(stderr,"`%c\' is unexpected\n", peek_char (&cbuffer));
      break;
    default:
      break;
  }

}

