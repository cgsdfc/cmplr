#include "tknzr_error.h"
#include "char_buffer.h"
#include "color.h"


static tknzr_error tknzr_errno;
static
const char *tokenizer_err_tab [] =
{
  [TERR_SUCCESS]="no error, success",
  [TERR_NOMEM]="no free memory available",
  [TERR_COMMENT_UNTERMINATED]="unterminated comment",
  [TERR_DIGIT_NOT_OCTONARY]= "invalid digit %s in octal constant",
  [TERR_DIGIT_NOT_HEXADECIMAL]="invalid digit %s in hexadicemal constant",
  [TERR_BAD_INTEGER_SUFFIX]="invalid suffix %s on integer constant",
  [TERR_FLOAT_EXP_NO_DIGIT]="exponent has no digits",
  [TERR_BAD_FLOAT_SUFFIX]="invalid suffix %s on floating point constant",
  [TERR_STRING_UNTERMINATED]="missing terminating \" character",
  [TERR_CHAR_UNTERMINATED]="missing terminating \' character",
  [TERR_EMPTY_CHAR_LITERAL]="empty character constant",
  [TERR_INVALID_IDENTIFIER]="invalid identifier", 
  [TERR_UNKNOWN]="check your code, there should not be TERR_UNKNOWN",
};

char *tknzr_error_string(void)
{
  static char buf[BUFSIZ];
  snprintf(buf,BUFSIZ,COLOR_FORMAT,WHITE,
      tokenizer_err_tab[tknzr_errno]);
  return buf;
}

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


char *tknzr_level_string(void)
{
  if (1< tknzr_errno && tknzr_errno < _TERR_FATAL)
  {
    return RED_FATAL;
  }

  if (_TERR_FATAL < tknzr_errno && tknzr_errno < _TERR_ERROR)
  {
    return RED_ERROR;
  }

  if (_TERR_ERROR < tknzr_errno && tknzr_errno < _TERR_WARNING)
  {
    return PURPLE_WARNING;
  }

  return CYAN_NOTE;
}

int  tknzr_error_handle (void)
{
  extern char_buffer cbuffer;
  char *errline = peek_line (&cbuffer, get_lineno(&cbuffer));
  char *errmsg=tknzr_error_string();
  char *level=tknzr_level_string();

  fprintf(stderr, "%s:%d:%d:%s %s\n",
      cbuffer.filename,
      cbuffer.pos.lineno,
      cbuffer.pos.column,
      level,
      errmsg);

  fprintf(stderr, "\t%s", errline);
  fputs( "\t", stderr);
  for (int i=1;i<cbuffer.pos.column;++i)
  {
    fputs(" ", stderr);
  }

  fprintf(stderr, "%s\n", GREEN_CARET);

  
}
