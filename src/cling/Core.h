#ifndef CLING_CORE_HXX
#define CLING_CORE_HXX
#include "Scanner.h"
#include "Error.h"
#include "Option.h"
#include "Parser.h"

struct Frontend {
  Parser parser;
  Scanner scanner;
  ErrorManager errorManager;

  Frontend(Option const *option, FILE *file)
    :scanner(option, file, &errorManager),
    parser(option, &errorManager) {}
  int Tokenize(FILE *output);
  int Parse(void);
};

int InteractiveTokenize(Option const *option);
#endif
