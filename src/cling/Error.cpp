#include "Error.h"
#include "Utility.h"

Error::Error(Scanner const *scanner, int context):row(scanner.row), col(scanner.col) {
  context=SymbolString(context);
}

void Error::print(FILE *file) {
  fprintf(file, "ERROR at %lu:%lu in %s: ", positive_number(row),
          positive_number(col), context);
}

void EList::AddError(Error *error) {
  elist.push_back(error);
}

EList::~EList() {
  for (auto e: elist) {
    GenericDestroy(e);
  }
  GenericDestroy(elist);
}
  
RedefinedError::RedefinedError(Scanner const *scanner, int context, char const *name):Error(scanner, context), name(strdup(name)) {}

void RedefinedError::print(FILE *file) {
  Error::print(file);
  fprintf(file, "name '%s' was redefined\n", name);
}

RedefinedError::~RedefinedError() { free(name); }
