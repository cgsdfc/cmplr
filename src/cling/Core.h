#ifndef CLING_CORE_HXX
#define CLING_CORE_HXX
#include "Scanner.h"
#include "Error.h"
#include "Option.h"

struct Frontend {
  Scanner scanner;
  EList elist;

  Frontend(Option const *option, FILE *file)
    :scanner(option, file, &elist) {}
  int Tokenize(FILE *output);
};

int InteractiveTokenize(Option const *option);
#endif
