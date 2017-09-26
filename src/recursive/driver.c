#include "recursive/parser.h"

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      die ("Usage rp <FILE>");
    }
  Parser *parser = CreateParserFromFile (argv[1]);
  if (!parser)
    {
      die ("rp: cannot open file");
    }
  int i = 0;
  if (ParserDoParsing (parser))
    {
      printf ("rp: input was accepted!\n");
    }
  else
    {
      printf ("rp: input was rejected!\n");
    }
}
