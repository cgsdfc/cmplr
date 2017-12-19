#ifndef CLING_BASIC_BLOCK
#define CLING_BASIC_BLOCK

#include <utillib/vector.h>

/*
 * Divide each function into a number of basic_blocks
 * for optimizer to run on.
 */

struct cling_basic_block {
  int block_id;
  unsigned int begin;
  unsigned int end;
  struct utillib_vector *instrs;
};

struct cling_instrbuf {
  struct utillib_vector * instrs;
  unsigned int *address_map;
  unsigned int count;
};

void cling_instrbuf_init(struct cling_instrbuf *self, struct utillib_vector *instrs, size_t address_size);
void cling_instrbuf_record(struct cling_instrbuf *self, unsigned int old_pc);
void cling_instrbuf_push_back(struct cling_instrbuf *self, void const *ir);
void cling_instrbuf_fix_address(struct cling_instrbuf const *self, void (*fixup_func)(void *ir, unsigned int const *address_map));
void cling_instrbuf_destroy(struct cling_instrbuf *self);

#endif /* CLING_BASIC_BLOCK */
