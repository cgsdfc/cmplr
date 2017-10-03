#ifndef UTILLIB_ENUM
#define UTILLIB_ENUM
#include <stdio.h> // for FILE*
// ENUM
#define UTILLIB_ETAB_TOSTRING(NAME) const char * NAME ## _tostring(int tag)
#define UTILLIB_ENUM_ELEM(TAG) TAG,
#define UTILLIB_ENUM_BEGIN(NAME) typedef enum NAME {
#define UTILLIB_ENUM_END(NAME) NAME ## _N } NAME;UTILLIB_ETAB_TOSTRING(NAME);
// ETAB
#define UTILLIB_ETAB_ELEM_ARRAY(TAB) [TAB] = # TAB,
#define UTILLIB_ETAB_ELEM_SWITCH(TAB) case (TAB): return # TAB;
#define UTILLIB_ETAB_BIGIN_IMPL_COMMON(NAME) UTILLIB_ETAB_TOSTRING(NAME){
#define UTILLIB_ETAB_BEGIN_SWITCH(NAME) switch(tag) {
#define UTILLIB_ETAB_END_SWITCH(NAME) default: return "(null)"; }
#define UTILLIB_ETAB_BEGIN_ARRAY(NAME)\
  if (tag < 0 || tag >= NAME ## _N) { return "(null)"; }\
  const static char * NAME ## _tab [] = {
#define UTILLIB_ETAB_END_ARRAY(NAME) }; return NAME ## _tab [ tag ];
#define UTILLIB_ETAB_END_IMPL_COMMON(NAME) }

#ifdef UTILLIB_ETAB_USE_SWITCH
// begin
# define UTILLIB_ETAB_BEGIN_IMPL(NAME)\
  UTILLIB_ETAB_BIGIN_IMPL_COMMON(NAME)\
  UTILLIB_ETAB_BEGIN_SWITCH(NAME)
// elem
# define UTILLIB_ETAB_ELEM_IMPL(NAME)\
  UTILLIB_ETAB_ELEM_SWITCH(NAME)
// end
# define UTILLIB_ETAB_END_IMPL(NAME)\
  UTILLIB_ETAB_END_SWITCH(NAME)\
  UTILLIB_ETAB_END_IMPL_COMMON(NAME)

#else
// begin
# define UTILLIB_ETAB_BEGIN_IMPL(NAME)\
  UTILLIB_ETAB_BIGIN_IMPL_COMMON(NAME)\
  UTILLIB_ETAB_BEGIN_ARRAY(NAME)
// elem
# define UTILLIB_ETAB_ELEM_IMPL(NAME)\
  UTILLIB_ETAB_ELEM_ARRAY(NAME)
// end
# define UTILLIB_ETAB_END_IMPL(NAME)\
  UTILLIB_ETAB_END_ARRAY(NAME)\
  UTILLIB_ETAB_END_IMPL_COMMON(NAME)

#endif

#define UTILLIB_ETAB_BEGIN(NAME) UTILLIB_ETAB_BEGIN_IMPL(NAME)
#define UTILLIB_ETAB_END(NAME) UTILLIB_ETAB_END_IMPL(NAME)
#define UTILLIB_ETAB_ELEM(NAME) UTILLIB_ETAB_ELEM_IMPL(NAME)

typedef const char * (e2s_function) (int);
void utillib_print_enum_range(FILE *, e2s_function *, const char *, int , int );
void utillib_print_enum(e2s_function * , int );

#endif // UTILLIB_ENUM
