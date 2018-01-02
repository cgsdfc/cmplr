#ifndef CLING_UTILITY_HXX
#define CLING_UTILITY_HXX
#define positive_number(X) ((X) ? (X) : 1)

#include <assert.h>
#include <string>
#include <stdio.h>

/*
 * value should be _int_ exactly
 */
#define UNREACHABLE(value) do { \
  fprintf(stderr, "Bad switch value '%d'\n", (value));\
  assert(false);\
} while(0)

struct String {
  std::string buffer;
  void AppendChar(char c);
  void Clear(void);
  char const *CStr(void)const ;
};

struct StringIntPair {
  char const *string;
  int value;
  StringIntPair(char const *string, int value)
    :string(string), value(value){}
};

template<class T>
void GenericDestroy(T *object) {
  object->~T();
}

bool IsIdenBegin(char ch);
int KeywordBseach(StringIntPair const *array, unsigned int len, char const *key);
int StringIntPairCompareOnString(void const *lhs, void const *rhs);

#endif
