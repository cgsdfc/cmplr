#include "hash.h"
#include <string.h>

static size_t utillib_charp_hash_impl_simple(char const *str)
{
  return strlen(str);
}

size_t utillib_charp_hash (char const *str)
{
  return utillib_charp_hash_impl_simple(str);
}

