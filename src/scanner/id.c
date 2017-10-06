#include <ctype.h>

bool
scanner_match_identifier(scanner_string tomatch)
{
  int first_ch = tomatch[0];
  if (isalpha(first_ch)) {
    for (scanner_string s=tomatch+1;
        *s; ++s)
    {
      if (isalnum(*s)) { continue; }
      return false;
    }
    return true;
  }
  return false;
}
  

