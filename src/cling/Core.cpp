#include "Core.h"
#include "Symbol.h"

int Frontend::Tokenize(FILE *output) {
  int code;
  while ((code=scanner.GetToken())) {
    char const *string=scanner.GetString();
    char const *name = SymbolString(code);
    fprintf(output, "'%s'\t", name);
    switch (code) {
      case SYM_INTEGER:
      case SYM_IDEN:
        fprintf(output, "%s\n", string);
        break;
      case SYM_STRING:
        fprintf(output, "\"%s\"\n", string);
        break;
      case SYM_CHAR:
        fprintf(output, "'%s'\n", string);
        break;
      default:
        fputs("\n", output);
        break;
    }
  }
  if (errorManager.HasError()) {
    errorManager.Print(stderr);
    return -1;
  }
  return 0;
}

int InteractiveTokenize(Option const *option) {
  Frontend frontend(option, stdin);
  return frontend.Tokenize(stdout);
}

int Frontend::Parse(void) {
  return parser.ParseAll(&scanner);
}

