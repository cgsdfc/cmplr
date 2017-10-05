#include "utillib/string.h"

int main()
{
  utillib_string s;
  utillib_string_init(&s, 1);
  for (char s='a';s<'z';++s) {
    utillib_string_append(&s, s);
  }
  puts(s.c_str);
}
