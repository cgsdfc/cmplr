#include "Token.h"
#include <string.h>

void TokenValue::print(FILE *file) {
  fprintf(file, "TokenValue(location(%u, %u))\n", location.row, location.col);
}

StringToken::StringToken(Location const& location, char const *string)
  :TokenValue(location), string(strdup(string)) {}

StringToken::~StringToken() { free(string); }

IntegerToken::IntegerToken(Location const& location, int intValue)
  :TokenValue(location), intValue(intValue) {}


