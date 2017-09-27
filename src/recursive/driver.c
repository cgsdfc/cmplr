#include "recursive/parser.h"
#include "recursive/print_xml.h"

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
      AstNode *root = ParserGetRoot (parser);
      xml_printer xmlp;
      init_xml_printer (&xmlp, root, stdout, LexerTerminalTab ());
      printf ("rp: input was accepted!\n");
      print_xml (&xmlp, argv[1]);
    }
  else
    {
      printf ("rp: input was rejected!\n");
    }
}
