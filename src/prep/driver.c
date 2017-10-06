#include "prep.h"

int main() 
{
  int v=prep_match_predefined_macro("__");
  puts(predefined_macro_tostring(v));

}
