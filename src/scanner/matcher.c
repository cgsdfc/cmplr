#include <ctype.h>
#include <string.h>
#include <strings.h>
#include "scanner.h"
#include "matcher.h"

static int
isidbegin(int c)
{
  return isalpha(c) || c == CH_UNDERSCORE;
}

static int
isidmiddle(int c)
{
  return isalnum(c) || c == CH_UNDERSCORE;
}

ptrdiff_t
scanner_match_identifier(scanner_string tomatch)
{
  scanner_string s=tomatch;
  if (isidbegin(s[0])) {
      for (; *s && isidmiddle(*s); ++s)
      ; // empty
  }
  return s-tomatch;
}
  
static int
isdecimalbegin(int c)
{
  return c != CH_0 && isdigit(c);
}

ptrdiff_t
scanner_match_decimal(scanner_string tomatch)
{
  scanner_string s=tomatch;
  if (isdecimalbegin(s[0])) { 
  for (; *s && isdigit(*s); ++s)
    ; // empty
  }
  return s-tomatch;
}

static int
isodigit(int c)
{
  '0' <= c && c <= '7';
}

static int
iscasechar (int c, int h)
{
  return tolower(c) == tolower(h);
}

ptrdiff_t 
scanner_match_hexdecimal(scanner_string tomatch)
{
  scanner_string s=tomatch;
  if (tomatch[0] == CH_0 && iscasechar(tomatch[1], CH_x)) 
  {
    for (; *s && isxdigit(*s); ++s)
      ; // empty
  }
  return s-tomatch;
}

ptrdiff_t
scanner_match_octal(scanner_string tomatch)
{
  scanner_string s=tomatch;
  if (tomatch[0] == CH_0)
  {
    for (; *s && isodigit(*s); ++s)
      ; // empty
  }
  return s-tomatch;
}

static int
isescaped(int c)
{
  switch(c){
    case 'a':
    case 'f':
    case 'n':
    case 'v':
      return 1;
    default:
      return 0;
  }
}

ptrdiff_t
scanner_match_escaped(scanner_string tomatch)
{
  scanner_string s=tomatch;
  if (s[0] == CH_BACKSLASH) {
    if (isescaped(s[1])) {
      return 2;
    }
  }
  return 0;
}

ptrdiff_t
scanner_match_string_lit(scanner_string tomatch)
{
  scanner_string s=tomatch;
  ptrdiff_t cnt=0;
  if (s[0] == CH_DQUOTE) {
    for (; *s ; ++s) {
      switch(*s) {
        case CH_BACKSLASH:

      }
    }
  }
}
