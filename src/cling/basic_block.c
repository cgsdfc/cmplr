#include "basic_block.h"

void cling_instrbuf_init(struct cling_instrbuf *self, struct utillib_vector *instrs, size_t address_size);
{
  self->instrs=instrs;
  self->count=0;
  self->address_map=malloc(address_size * sizeof self->address_map[0]);
}

void cling_instrbuf_push_back(struct cling_instrbuf *self, void const *ir)
{
  self->address_map[self->count]=utillib_vector_size(&self->instrs);
  utillib_vector_push_back(&self->instrs, ir);
  ++self->count;
}

void cling_instrbuf_fix_address(struct cling_instrbuf *self, void (*fixup_func)(void *ir, unsigned int *address_map))
{
  void *ir;
  UTILLIB_VECTOR_FOREACH(ir, &self->instrs) {
    fixup_func(ir, self->address_map);
  }
}

void cling_instrbuf_move(struct cling_instrbuf *self, struct utillib_vector *dest) {
  utillib_vector_append(dest, &self->instrs);
}

void cling_instrbuf_destroy(struct cling_instrbuf *self)
{
  free(self->address_map);
}


