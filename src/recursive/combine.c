#include "recursive/combine.h"
#include "recursive/parser_info.h"
#include "recursive/function.h"
#include <stdarg.h>

// repeated applies parser on the input as long as the
// parser accepts the input.
// when parser rejects the input, repeated return true;
bool
repeated (parser_function * parser, parser_info * info)
{
  for (; parser->apply (info);)
    ;				/* nothing */
  return true;
}

bool
one_of (parser_info * info, unsigned num, ...)
{
  va_list ap;
  va_start (ap, num);
  parser_function *parser;
  for (int i = 0; i < num; ++i)
    {
      parser = va_arg (ap, parser_function *);
      if (parser->apply (info))
	{
	  return true;
	}
    }
  va_end (ap);
  return false;
}

bool
accept_when (parser_function * parser, parser_info * info,
	     pred_function * pred)
{
  if (parser->apply (info) && pred->apply (info))
    {
      return true;
    }
  return false;
}

bool
optional (parser_function * parser, parser_info * info)
{
  return parser->apply (info);
}
