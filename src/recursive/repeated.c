#include "recursive/repeated.h"
#include "recursive/parser_info.h"
#include "recursive/parser_function.h"

// repeated applies parser on the input as long as the
// parser accepts the input.
// when parser rejects the input, repeated return true;
bool repeated(parser_function * parser, parser_info * info)
{
  for (;parser->apply(info);)
    ; /* nothing */
  return true;
}

