#ifndef BLOCK_BUFFER_H
#define BLOCK_BUFFER_H

// block alloc memory 

typedef struct block_mem
{
  struct block_mem *prev;
  struct block_mem *next;
  char *mem;
} block_mem;


typedef struct block_pos
{
  block_mem *blk;
  int index;
} block_pos;


typedef struct block_buffer
{
  /* the break between used mem and freemem */
  block_pos freemem;
  /* how many entries in a block */
  int blksz;

  /* how big each entry is */
  int elesz;

  /* how many entries in total */
  int count;

  /* keep the head of list */
  block_mem *head;

} block_buffer;

block_buffer *alloc_block_buffer (int blksz, int init_nblk, int elesz);
char *block_buffer_alloc (block_buffer * buf);
void block_buffer_dealloc (block_buffer * buf);
int block_buffer_prev (block_buffer * buf, block_pos *, block_pos * pblk);
int block_buffer_next (block_buffer * buf, block_pos *, block_pos * pblk);
bool block_pos_equal (block_pos * s, block_pos * t);
void *block_buffer_get_elem (block_buffer * buf, block_pos * pblk);
#endif
