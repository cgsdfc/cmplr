#ifndef RECUSIVE_PARSER_H
#define RECUSIVE_PARSER_H

#include "recursive/context.h"   // for pcontext
#include "recursive/node_base.h" // for node_base
#include <stdbool.h>             // for bool
typedef pcontext Parser;
typedef node_base AstNode;
Parser *CreateParserFromFile(char *);
Parser *CreateParserFromString(char *);
void DestroyParser(Parser *);
bool ParserDoParsing(Parser *);
AstNode *ParserGetRoot(Parser *);

#endif // RECUSIVE_PARSER_H
