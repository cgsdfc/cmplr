#include "rtl.h"

void cling_rtl_emitter_init(struct cling_rtl_emitter *self,
                            unsigned int regsize) {
  self->regsize = regsize;
}

/*
 * The first pass from ast_ir to RTL should do the
 * following things:
 * 1. replace constants with their value.
 * 2. resolve all names to memory and all temps
 * to registers or memory and all relative address
 * to absolute address.
 * 3. pool all strings.
 * 4. add possible type conversion code.
 * 5. allocate global registers and temp registers
 * in the simpliest manner: first comes first get.
 * 6. perform none of the optimization.
 */

void cling_rtl_emitter_emit(struct cling_rtl_emitter *self,
                            struct cling_ast_ir_program const *ast_ir,
                            struct cling_rtl_program *program) {}
