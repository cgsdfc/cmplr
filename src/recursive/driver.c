#include "recursive/parser.h"
#include "recursive/print_xml.h"
#include "recursive/error.h"
#include "construct.h"


static int
main_do_parse (int argc, char **argv)
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
      init_xml_printer (&xmlp, stdout, argv[1], LexerTerminalTab ());
      printf ("rp: input was accepted!\n");
      print_xml (&xmlp, root);
      return 0;
    }
  else
    {
      printf ("rp: input was rejected!\n");
      return 1;
    }
}


int
main (int argc, char **argv)
{
  return main_do_parse (argc, argv);
}
