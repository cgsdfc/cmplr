#ifndef CLING_RTL_H
#define CLING_RTL_H
#include "ast_ir.h"
#include "ir.h"

#include <utillib/enum.h>

/*
 * Since ast_ir is derived
 * directly from ast without
 * too much consideration on
 * registers, memory and storage
 * schema, we need a RTL, or 
 * Register-Trasferring-Langauge
 * to perform optimization and codegen
 * on it.
 * The design goal of RTL is to make it
 * a template for any other assembly languages
 * which can be easily derived from the RTL with
 * minor adjustment.
 * Meanwhile, the RTL should aid optimization which
 * means one piece of RTL code can be transferred to
 * another instance pass by pass.
 */

struct cling_rtl_variable {
  unsigned int wide;
  unsigned int address;
};

struct cling_rtl_emitter {
  unsigned int regsize;
  struct utillib_vector string_pool;
  struct utillib_hashmap string_map;
  struct utillib_hashmap global_map;

  


};

struct cling_rtl_function_emitter {
  struct utillib_hashmap named_const;
  unsigned int stack_ptr;


};

struct cling_rtl_program {
  struct utillib_vector instrs;


};

struct cling_rtl {
#define CLING_RTL_OPERAND_MAX 2
  unsigned int opcode;
  union {
    unsigned int address;
    unsigned int regid;
    unsigned int unsigned_imme;
    unsigned int base;
    int signed_imme;
    int offset;
  } operands[CLING_RTL_OPERAND_MAX];
};

UTILLIB_ENUM_BEGIN(cling_rtl_opcode_kind)
UTILLIB_ENUM_ELEM(RTL_MOV)
UTILLIB_ENUM_ELEM(RTL_ADD)
UTILLIB_ENUM_END(cling_rtl_opcode_kind)
#endif /* CLING_RTL_H */
