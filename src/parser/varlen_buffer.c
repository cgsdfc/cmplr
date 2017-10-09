#include "varlen_buffer.h"

static varlen_buffer *vbuffer;
static varlen_buffer buf;
static block_buffer *blk_buf;
int fill_varlen_block(void) {
  varlen_block *vblk;
  varlen_block **head;

  head = &vbuffer->curblk;
  vblk = block_buffer_alloc(blk_buf);
  vblk->mem = malloc(VARLEN_BUFFER_SZ);

  if (!vblk)
    return -1;
  memset(vblk, 0, sizeof(varlen_block));
  vblk->next = *head;
  *head = vblk;
  vbuffer->nblk++;
  return 0;
}

int init_varlen_buffer(void) {
  varlen_block *vblk, *prev;
  vbuffer = &buf;
  blk_buf = alloc_block_buffer(VARLEN_BUFFER_BLKSZ, 1, sizeof(varlen_block));

  for (int i = 0; i < VARLEN_BUFFER_BLKSZ; ++i) {
    if (fill_varlen_block() < 0)
      return -1;
  }
  return 0;
}

int varlen_newlen(int len) { return (len & 1) ? (len << 1) : (len * 3 + 5); }

int varlen_check_len(void **array, int len, int *max, int elemsz) {
  varlen_block *vblk;
  void *new_array;
  void *old_array;
  int old_size;
  int new_size;
  int newlen;

  if (len < *max) {
    return 0;
  }
  old_size = len * elemsz;
  newlen = varlen_newlen(len);
  new_size = newlen * elemsz;
  vblk = vbuffer->curblk;

  for (int i = 1; i < vbuffer->nblk; ++i) {
    if (vblk->size >= new_size) {
      memcpy(vblk->mem, *array, old_size);
      old_array = *array;
      *array = vblk->mem;
      vblk->mem = old_array;
      vblk->size = old_size;
      *max = newlen;
      return 0;
    }
    vblk = vblk->next;
  }
  new_array = malloc(new_size);
  memcpy(new_array, *array, old_size);
  old_array = *array;
  *array = new_array;
  for (int i = 1; i < vbuffer->nblk; ++i) {
    if (vblk->size == 0) {
      vblk->size = old_size;
      vblk->array = old_array;
      return 0;
    }
  }

  vblk = block_buffer_alloc(blk_buf);
  vblk->mem = old_array;
  vblk->next = vbuffer->curblk;
  vbuffer->curblk = vblk;
  return 0;
}
