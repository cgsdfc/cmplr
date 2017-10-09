#ifndef UTILLIB_TO_BASE_H
#define UTILLIB_TO_BASE_H

#define UTILLIB_CREATE_BASE(TYPE, NAME, TAG)                                   \
  TYPE *NAME = malloc(sizeof *NAME);                                           \
  (NAME)->base.tag = (TAG);
#define UTILLIB_TO_BASE(BASE_TYPE, PTR) ((BASE_TYPE *)(PTR))
#define UTILLIB_GET_BASE(PTR) (&((PTR)->base))
#endif // UTILLIB_TO_BASE_H
