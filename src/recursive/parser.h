#ifndef RECUSIVE_PARSER_H
#define RECUSIVE_PARSER_H

#include "recursive/context.h"
#include "recursive/expr.h"
#include "recursive/stmt.h"
#include "recursive/error.h"

typedef pcontext Parser;
Parser *CreateParserFromFile (char *);
Parser *CreateParserFromString (char *);
void DestroyParser (Parser *);
bool ParserDoParsing (Parser *);

#endif // RECUSIVE_PARSER_H
