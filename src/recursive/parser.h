#ifndef RECUSIVE_PARSER_H
#define RECUSIVE_PARSER_H

#include "recursive/context.h"
#include "recursive/expr.h"
#include "recursive/stmt.h"
#include "recursive/error.h"
#include "recursive/node_base.h"

typedef pcontext Parser;
typedef node_base AstNode;
Parser *CreateParserFromFile (char *);
Parser *CreateParserFromString (char *);
void DestroyParser (Parser *);
bool ParserDoParsing (Parser *);
AstNode *ParserGetRoot (Parser *);

#endif // RECUSIVE_PARSER_H
