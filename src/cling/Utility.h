#ifndef CLING_UTILITY_HXX
#define CLING_UTILITY_HXX
#define positive_number(X) ((X) ? (X) : 1)


#include <string>
struct String {
  std::string buffer;
  void AppendChar(char c);
  void Clear(void);
  char const *CStr(void);
};

struct StringIntPair {
  char const *string;
  int value;
};

template<class T>
void GenericDestroy(T *object) {
  object->~T();
}

bool IsIdenBegin(char ch);
int KeywordBseach(StringIntPair const *array, unsigned int len, char const *key);

#endif
