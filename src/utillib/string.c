#include "utillib/string.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define UTILLIB_STR_CMP(S,T, OP)\
(strcmp (UTILLIB_C_STR(S), UTILLIB_C_STR(T)) OP (0))

int utillib_string_init(utillib_string * s, utillib_string_size_type init_cap)
{
  // must init to be able to hold init_cap of character
  assert (init_cap != 0);
  // init_cap should be 
  utillib_string_char_type * c_str =
  calloc(sizeof *c_str , init_cap+1); /* for NULL */ 
  if (!c_str) { return -1; }
  s->c_str=c_str;
  s->capacity = init_cap;
  s->size=0;
  return 0;
}

static int
utillib_string_reserve(utillib_string *s, utillib_string_size_type new_capa)
{
  assert (new_capa > s->size + 1);
  // change s's capacity to new_capa
  assert (new_capa);
  utillib_string_char_type *  newspace = calloc(sizeof *newspace, new_capa);
  if (!newspace) { return -1; }
  memcpy (newspace, s->c_str, s->size+1); // for NULL
  free(s->c_str);
  s->c_str=newspace;
  s->capacity=new_capa;
  return 0;
}

static int // 0
utillib_string_append_aux(utillib_string *s, utillib_string_char_type x)
{
  assert (x);
  assert (s->size < s->capacity-1);
  utillib_string_char_type * end=s->c_str + s->size;
  *end++=x; *end=0;
  s->size++;
  return 0;
}

int utillib_string_append(utillib_string *s, utillib_string_char_type x)
{
  if (s->capacity-1 == s->size) {
    if (utillib_string_reserve(s, s->size << 1)==0) {
      return utillib_string_append_aux(s, x);
    } return -1;
  }
  return utillib_string_append_aux(s, x);
}

utillib_string_size_type
utillib_string_size(utillib_string *s)
{ return s->size; }

void utillib_string_destroy(utillib_string * s)
{
  free (s->c_str);
}

bool utillib_string_empty(utillib_string *s)
{
  return s->size == 0;
}

bool utillib_string_richcmp (utillib_string *s, utillib_string *t, 
    string_cmpop op)
{
  switch (op) {
    case STRING_EQ:
      return UTILLIB_STR_CMP(s, t, ==);
    case STRING_GT:
      return UTILLIB_STR_CMP(s, t, >);
    case STRING_GE:
      return UTILLIB_STR_CMP(s, t, >=);
    case STRING_LT:
      return UTILLIB_STR_CMP(s, t, <);
    case STRING_LE:
      return UTILLIB_STR_CMP(s, t, <=);
    case STRING_NE:
      return UTILLIB_STR_CMP(s, t, !=);
    default:
      assert (false);
  }
}


