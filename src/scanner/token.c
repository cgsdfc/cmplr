#include "token.h"
#include <string.h>
#include <stdlib.h>

scanner_token *
scanner_make_token(char *const str, int type, utillib_position *pos)
{
  scanner_token * token = malloc(sizeof *token);
  token->str=strdup(str);
  token->type=type;
  memcpy (&(token->pos), pos, sizeof *pos);
  return token;
}

void
scanner_destroy_token(scanner_token * token)
{
  free(token->str);
  free(token);
}
  
