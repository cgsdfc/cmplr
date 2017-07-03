#ifndef HASH_TABLE_H 
#define HASH_TABLE_H 1
#include "stack_buffer.h"

typedef stack_buffer_entry hash_entry;

typedef enum hash_flag
{
  HF_NOT_USED=0,
  HF_DUMMY,
  HF_IN_USE,
} hash_flag;

typedef struct hash_value
{
  char *key ;
  hash_flag flag;
  element value;
} hash_value;

typedef struct hash_table
{
  int nslot;
  int nused;
  hash_entry **entries;
} hash_table;

#endif

