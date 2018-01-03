#include "Utility.h"
#include <ctype.h>
#include <string.h>

void String::AppendChar(char c) {
  buffer+=c;
}
void String::Clear(void) {
  buffer.clear();
}
char const *String::CStr(void) const {
  return buffer.c_str();
}
/*
 * void *bsearch(const void *key, const void *base,
                     size_t nmemb, size_t size,
                     int (*compar)(const void *, const void *));
 */

int StringIntPairCompareOnString(void const *lhs, void const *rhs) {
  return strcmp(
      static_cast<StringIntPair const*>(lhs)->string,
      static_cast<StringIntPair const*>(rhs)->string);
}

int KeywordBseach(StringIntPair const *array, unsigned int len, char const *key) {
  StringIntPair target(key, 0);
  void *p=bsearch(&target, array, len, sizeof array[0], StringIntPairCompareOnString);
  if (p)
    return static_cast<StringIntPair*>(p)->value;
  return -1;
}
