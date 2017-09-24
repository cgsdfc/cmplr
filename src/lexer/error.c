#include "lexer/error.h"
#include "lexer/char_buffer.h"
#include "lexer/color.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


static int cmplr_errno;
static const err_entry error_tab[] = {
  [ER_NO_INPUT] = {"no input file specified, compilation stops", LV_FATAL},
  [ER_BAD_TOKEN] = {"bad token", LV_ERROR},
  [ER_NOMEM] = {"no free memory available", LV_FATAL},
  [ER_COMMENT_UNTERMINATED] = {"unterminated comment", LV_ERROR},
  [ER_DIGIT_NOT_OCTONARY] = {"invalid digit in octal constant", LV_ERROR},
  [ER_DIGIT_NOT_HEXADECIMAL] =
    {"invalid digit in hexadicemal constant", LV_ERROR},
  [ER_BAD_INTEGER_SUFFIX] = {"invalid suffix on integer constant", LV_ERROR},
  [ER_FLOAT_EXP_NO_DIGIT] = {"exponent has no digits", LV_ERROR},
  [ER_BAD_FLOAT_SUFFIX] =
    {"invalid suffix on floating point constant", LV_ERROR},
  [ER_STRING_UNTERMINATED] = {"missing terminating \" character", LV_ERROR},
  [ER_CHAR_UNTERMINATED] = {"missing terminating \' character", LV_ERROR},
  [ER_EMPTY_CHAR_LITERAL] = {"empty character constant", LV_ERROR},
  [ER_INVALID_CHAR] = {"invalid character in source", LV_ERROR},
};

static const char *indicator[] = {
  [LV_FATAL] = RED_FATAL,
  [LV_ERROR] = RED_ERROR,
  [LV_NOTE] = CYAN_NOTE,
  [LV_WARNING] = PURPLE_WARNING
};

static char *
tknzr_error_string (void)
{
  static char buf[BUFSIZ];
  const err_entry *ent = &error_tab[cmplr_errno];
  const char *idctr = indicator[ent->lv];

  snprintf (buf, BUFSIZ, COLOR_FORMAT, idctr, WHITE, ent->errmsg);
  return buf;
}


void
tknzr_error_set (int err)
{
  cmplr_errno = err;
}

int
tknzr_error_handle (char_buffer *cb)
{
  put_char (cb);
  char *errline = peek_line (cb, get_lineno (cb));
  char *errmsg = tknzr_error_string ();

  fprintf (stderr, "%s:%d:%d:%s\n",
	   cb->filename, cb->pos.lineno, cb->pos.column, errmsg);

  fprintf (stderr, "\t%s", errline);
  fputs ("\t", stderr);
  for (int i = 1; i < cb->pos.column; ++i)
    {
      fputs (" ", stderr);
    }

  fprintf (stderr, "%s\n", GREEN_CARET);
  return 0;


}
