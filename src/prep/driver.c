#include "prep.h"

int main() 
{
  scanner_enum_value v = prep_match_directive("efine");
  puts(directive_tostring(v));

}
