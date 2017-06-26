#ifndef TOKEN_BUFFER_H
#define TOKEN_BUFFER_H 1


#define N_TKB_BLKSZ 100
#define N_TKB_INIT_NBLK 10

typedef struct block_mem
{
  struct block_mem *prev;
  struct block_mem *next;
  char *mem;
} block_mem;

typedef struct block_buffer
{
  /* who use this buffer */
  char *name;

  /* index into cur->mem */
  int index;

  /* how many entries in a block */
  int blksz;

  /* how big each entry is */
  int elesz;

  /* how many entries in total */
  int count;

  /* which block we are currently at */
  block_mem *cur;

  /* keep the head of list */
  block_mem *head;

} block_buffer;

typedef block_buffer token_buffer;
typedef block_buffer block_char_buffer;

int init_block_buffer(block_buffer *buf, char *name ,int blksz, int init_nblk,int elesz);

block_buffer *alloc_buffer(void);
#endif

