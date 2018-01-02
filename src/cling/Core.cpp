#include "Core.h"
#include "Symbol.h"

int Frontend::Tokenize(FILE *output) {
  int code;
  while (code=scanner.GetToken()) {
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
  if (elist.HasError()) {
    elist.Print(stderr);
    return -1;
  }
  return 0;
}
