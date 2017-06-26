#include "hash_table.h"

typedef union slot_value
{
  char *string;
} slot_value;

typedef struct open_addr_hash
{
  char **keys;
  char **vals;
  int count;
  int nslots;

} open_addr_hash;

int init_open_addr_hash(open_addr_hash *hash,
