#include "hash_table.h"

/** invariant: 
 * HF_DUMMY || HF_NOT_USED <=> entry->string == 0;
 * HF_IN_USE == entry->string is valid
 * 
 */
static stack_buffer *entry_allocator;

  stack_buffer_entry *
alloc_hash_entry(void)
{
  hash_entry *entry;

  if (!entry_allocator)
  {
    entry_allocator = alloc_stack_buffer(sizeof (hash_table_entry));
    if (!entry_allocator)
    {
      return NULL;
    }
  }

  if (stack_buffer_alloc(entry_allocator, &entry)<0)
    return NULL;
  return entry;
}

int init_hash_table(hash_table *tb)
{
  if (!tb)
    return -1;

  tb->entries = malloc (sizeof(stack_buffer_entry*) * nslot);
  if (!tb->entries)
    return -1;

  stack_buffer_entry *ent;
  for (int i=0;i<nslot;++i)
  {
    ent = hash_table_alloc_entry();
    if (!ent)
      return -1;
    tb->entries[i] = ent;
  }

  tb->nslot = nslot;
  tb->nused = 0;
  tb->new_ent = 0;
  return tb;
}

  static unsigned
do_hash(char *string)
{
  unsigned hash=string[0];
  for (char *s=string+1; *s; ++s)
  {
    hash ^= *s;
  }
  return hash;
}

int do_rehash(hash_table *tb)
{
  hash_table_entry **newspace;
  int new_nslot;
  int index;

  new_nslot=tb->nslot << 1;
  newspace=malloc(sizeof(hash_table_entry*) * new_nslot);
  if (!newspace)
    return -1;

  for (int i=0;i<tb->nslot;++i)
  {
    entry = tb->entries[i];
    if (entry->flag == HF_NOT_USED)
      continue;

    hash = do_hash(entry->string);
    index = hash & new_nslot;

    while (true)
    {
      if (!newspace[index])
      {
        newspace[index]=entry;
        break;
      }
      index = ((index << 1)+1) & newspace;
    }
  }

  for (int i=0;i<new_nslot;i++)
  {
    if (!newspace[i])
    {
      entry=block_buffer_alloc(entry_allocator);
      memset (entry, 0 ,sizeof(hash_table_entry));
    }
  }

  free (tb->entries);
  tb->entries=newspace;
  tb->nslot=new_nslot;

  return 0;
}



int hash_table_insert (hash_table *tb, char *key, element val) 
{
  if (!tb)
    return -1;
  if (!key)
    return -1;

  unsigned hash = do_hash(string);
  int index = hash & tb->nslot;
  hash_table_entry *ent = tb->entries[index];
  bool rehash=false;

  if (( tb->nused << 1 ) >= tb->nslot && do_rehash(tb)<0)
  {
    return -1;
  }

  while (true)
  {
    if (ent->flag == HF_NOT_USED
        || ent->flag == HF_DUMMY)
    {
      ent->flag=HF_IN_USE;
      ent->string=strdup(key);
      ent->value=val;
      tb->nused++;
      return 0;
    }

    if (strcmp (ent->string, key)==0)
    {
      // found a used slot, keep it in its place
      ent->value=val;
      return 0;
    }

    index = ((index << 1) + 1) & tb->nslot;
    ent = tb->entries[index];
  }


  return 0;
}

int hash_table_lookup(hash_table *tb, char *key, element *val)
{
  if (!tb)
    return -1;
  if (!key)
    return -1;
  if (!val)
    return -1;

  unsigned hash = do_hash(key);
  int index = hash & tb->nslot;
  hash_table_entry *ent = tb->entries[index];

  while (true)
  {
    if (strcmp (ent->string, key) == 0)
    {
      *val = ent->value;
      return 0;
    }
    if (ent->flag == HF_NOT_USED)
    {
      return 1;
    }
    index = ((index << 1) + 1) & tb->nslot;
    ent = tb->entries[index];
  }

  assert (0);
  return 0;
}

int hash_table_remove(hash_table *tb, char *val)
{
  if (!tb)
    return -1;
  if (!val)
    return -1;

  unsigned hash = do_hash(key);
  int index = hash & tb->nslot;
  hash_table_entry *ent = tb->entries[index];

  while (true)
  {
    if (ent->flag == HF_IN_USE)
    {
      ent->flag = HF_DUMMY;
      free (ent->string);
      ent->string = 0;
      tb->nused--;
      return 0;
    }

    if (ent->flag == HF_NOT_USED)
    {
      return 1;
    }
    index = ((index << 1) + 1) & tb->nslot;
    ent = tb->entries[index];
  }

  return 0;
}






