#ifndef VARLEN_BUFFER_H
#define VARLEN_BUFFER_H 1
#include "tknzr/block_buffer.h"

// this buffer is aimed to solve the problem of
// appending or building a continuous block of memory
// whose len can be told in advance.
// the way I treated this problem by now is to record
// a 3-tuple (void*, len, max) and check every time the
// buf is appended. It was too much. but I don't want to
// run into some like vector<int> template.
// so I decided to use a 3 parts system:
// 1. varlen struct (void *, len)
// 2. varbuffer (elemsz, max)
// 3. something like a linked list of blocks of diff bytes
// 4. varblock (size, void*mem, next)
//
// the usr does the things:
// 1. call alloc_varbuffer (elemsz) to
#define VARLEN_BUFFER_BLKSZ 16
#define VARLEN_BUFFER_SZ (sizeof(int) * 16)

typedef struct varlen_block {
  int size;
  void *mem;
  struct varlen_block *next;
} varlen_block;

typedef struct varlen_buffer {
  varlen_block *curblk;
  int nblk;
} varlen_buffer;

#endif
