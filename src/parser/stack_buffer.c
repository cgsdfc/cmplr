/* stack_buffer.c */
#include "stack_buffer.h"

static block_buffer *stack_entry_buffer;
static stack_buffer all_buffers[STACK_BUFFER_MAX_LEN];
static int all_count;

stack_buffer *alloc_stack_buffer(int elesz)
{
  block_buffer *blk_buf;
  stack_buffer *sb;

  if (!stack_entry_buffer)
  {
    stack_entry_buffer = alloc_block_buffer(
        STACK_BUFFER_BLKSZ,
        STACK_BUFFER_INIT_NBLK,
        sizeof (stack_buffer_entry));
    if (!stack_entry_buffer)
      return NULL;
  }

  sb=&all_buffers[all_count++];
  memset(sb, 0 ,sizeof (stack_buffer));
  blk_buf = alloc_block_buffer(
      STACK_BUFFER_BLKSZ,
      STACK_BUFFER_INIT_NBLK,
      sizeof (elesz));
  if (!blk_buf)
    return NULL;
  sb->blk_buf=blk_buf;
  return sb;
}

int stack_buffer_alloc(stack_buffer *buf, 
    stack_buffer_entry **ent)
{
  if (!buf)
    return -1;
  if (!ent)
    return -1;

  stack_buffer_entry *entry;
  void *value;

  if (!buf->head)
  {
    for (int i=0;i<STACK_BUFFER_NFILL;++i)
    {
      entry = (stack_buffer_entry*)
        block_buffer_alloc(stack_entry_buffer);
      if (!entry)
        return -1;
      value = block_buffer_alloc(buf->blk_buf);
      if (!value)
        return -1;
      entry->value=value;
      entry->next = buf->head;
      buf->head = entry;
      buf->nalloc++;
    }
  }

  *ent=buf->head;
  *ent->next=0;
  buf->head=buf->head->next;
  buf->nused++;
  return 0;

}

int stack_buffer_dealloc(stack_buffer *buf,
    stack_buffer_entry *ent)
{
  if (!buf)
    return -1;
  if (!ent)
    return -1;
  ent->next = buf->head;
  buf->head = ent;
  buf->nused--;
  return 0;

}
