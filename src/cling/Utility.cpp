#include "Utility.h"
#include <ctype.h>

void String::AppendChar(char c) {
  buffer+=c;
}
void String::Clear(void) {
  buffer.clear();
}
char const *String::CStr(void) {
  return buffer.c_str();
}
bool IsIdenBegin(char ch) {
  return ch == '_' || isalpha(ch);
}
int KeywordBseach(KeyWordPair const *array, unsigned int len, char const *key) {



}
