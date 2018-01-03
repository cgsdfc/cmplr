#ifndef CLING_PARSER_HXX
#define CLING_PARSER_HXX

/*
 * Our parser is a wrapper around Lemon Parser
 */
struct Option;
struct Program;
struct Scanner;
struct ErrorManager;

struct Parser {
  Option const* option;
  ErrorManager *errorManager;
  void *lemonImpl;
  Program *program;
  Parser(Option const *option, ErrorManager *errorManager);
  int ParseAll(Scanner *scanner);
  ~Parser();



};

#endif 
