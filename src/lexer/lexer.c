#include "lexer.h"

token_buffer *
create_lexer(const char * file)
{
  char_buffer * cb = malloc(sizeof *cb);
  if (!cb) {
    return NULL;
  }
  if (0!=init_char_buffer_from_file(cb, file)) {
    free(cb);
    return NULL;
  }
  token_buffer * tb = malloc(sizeof *tb);
  if (!tb) {
    free(cb);
    return NULL;
  }
  if (0!=init_token_buffer(tb, cb)) {
    free(cb);
    free(tb);
    return NULL;
  }
  return tb;

}
void destroy_lexer(token_buffer * lexer)
{


}

