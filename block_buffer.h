#ifndef BLOCK_BUFFER_H
#define BLOCK_BUFFER_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
// block alloc memory 


typedef struct block_mem
{
  struct block_mem *prev;
  struct block_mem *next;
  char *mem;
} block_mem;

typedef struct block_buffer
{

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

block_buffer *alloc_block_buffer(int blksz, int init_nblk,int elesz);
char *block_buffer_alloc(block_buffer *buf);
void block_buffer_dealloc(block_buffer *buf);
char *block_buffer_peek(block_buffer *buf, int n);

#endif

