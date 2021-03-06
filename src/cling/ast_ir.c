/*
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/
#include "ast_ir.h"
#include "option.h"
#include "misc.h"
#include "symbol_table.h"
#include "symbols.h"
#include <utillib/json_foreach.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UTILLIB_ETAB_BEGIN(ast_opcode_kind)
UTILLIB_ETAB_ELEM_INIT(OP_ADD, "+")
UTILLIB_ETAB_ELEM_INIT(OP_SUB, "-")
UTILLIB_ETAB_ELEM_INIT(OP_DIV, "/")
UTILLIB_ETAB_ELEM_INIT(OP_MUL, "*")
UTILLIB_ETAB_ELEM_INIT(OP_EQ, "==")
UTILLIB_ETAB_ELEM_INIT(OP_NE, "!=")
UTILLIB_ETAB_ELEM_INIT(OP_LT, "<")
UTILLIB_ETAB_ELEM_INIT(OP_LE, "<=")
UTILLIB_ETAB_ELEM_INIT(OP_GT, ">")
UTILLIB_ETAB_ELEM_INIT(OP_GE, ">=")
UTILLIB_ETAB_END(ast_opcode_kind);

static const struct ast_ir ast_ir_nop = {.opcode = OP_NOP};
static const struct ast_ir ast_ir_newline = {.opcode=OP_NL};

static void emit_composite(struct ast_ir_global *self,
                struct utillib_json_value const *object);
static void emit_statement(struct ast_ir_global *self,
                struct utillib_json_value const *object);
static int emit_expr(struct ast_ir_global *self,
                struct utillib_json_value const *object);

static struct ast_ir *create_ir(int opcode) {
        struct ast_ir *self = calloc(1, sizeof *self);
        self->opcode = opcode;
        return self;
}

void ast_ir_destroy(struct ast_ir *self) {
        switch(self->opcode) {
                case OP_NOP:
                case OP_NL:
                        return;
                case OP_CAL:
                        free(self->call.argv);
                        break;
        }
        free(self);
}

void ast_ir_print(struct ast_ir const *self, FILE *file) {
        char const *opstr = ast_opcode_kind_tostring(self->opcode);
        char const *size_name;

        switch (self->opcode) {
        case OP_NOP:
                fputs("nop\n", file);
                break;
        case OP_DEFCON:
                size_name = size_tostring(self->defcon.size);
                if (self->defcon.size == MIPS_WORD_SIZE) {
                        fprintf(file, "const %s %s = %d\n", self->defcon.name, size_name,
                                        self->defcon.value);
                } else {
                        fprintf(file, "const %s %s = '%c'\n", self->defcon.name, size_name,
                                        (char)self->defcon.value);
                }
                break;
        case OP_PARA:
                size_name = size_tostring(self->para.size);
                fprintf(file, "para %s %s\n", size_name, self->para.name);
                break;
        case OP_DEFVAR:
                size_name = size_tostring(self->defvar.size);
                fprintf(file, "var %s %s\n", size_name, self->defvar.name);
                break;
        case OP_DEFUNC:
                size_name = size_tostring(self->defunc.return_size);
                fprintf(file, "%s %s()\n", size_name, self->defunc.name);
                break;
        case OP_DEFARR:
                size_name = size_tostring(self->defarr.base_size);
                fprintf(file, "var %s %s[%lu]\n", size_name, self->defarr.name,
                                self->defarr.extend);
                break;
        case OP_CAL:
                for (int i=0; i<self->call.argc; ++i) {
                        fprintf(file, "push t%d; ", self->call.argv[i]);
                }
                if (self->call.has_result)
                        fprintf(file, "t%d = call %s\n", self->call.result, self->call.name);
                else
                        fprintf(file, "call %s\n", self->call.name);
                break;
        case OP_INDEX:
                fprintf(file, "t%d = t%d [t%d]\n", self->index.result,
                                self->index.array_addr, self->index.index_result);
                break;
        case OP_BEZ:
                fprintf(file, "bez t%d (addr=%d)\n", self->bez.temp, self->bez.addr);
                break;
        case OP_BNE:
                fprintf(file, "bne t%d t%d (addr=%d)\n", self->bne.temp1, self->bne.temp2,
                                self->bne.addr);
                break;
        case OP_JMP:
                fprintf(file, "jmp %d\n", self->jmp.addr);
                break;
        case OP_RET:
                if (self->ret.has_result)
                        fprintf(file, "ret (addr=%d) t%d\n", self->ret.addr, self->ret.result);
                else
                        fprintf(file, "ret (addr=%d)\n", self->ret.addr);
                break;
        case OP_READ:
                fprintf(file, "read t%d (kind=%d)\n", self->read.temp, self->read.kind);
                break;
        case OP_WRITE:
                fprintf(file, "write t%d (kind=%d)\n", self->write.temp, self->write.kind);
                break;
        case OP_LDSTR:
                fprintf(file, "ldstr t%d \"%s\"\n", self->ldstr.temp, self->ldstr.string);
                break;
        case OP_LDIMM:
                if (self->ldimm.size == MIPS_WORD_SIZE)
                        fprintf(file, "ldimm t%d %d\n", self->ldimm.temp, self->ldimm.value);
                else
                        fprintf(file, "ldimm t%d '%c'\n", self->ldimm.temp,
                                        (char)self->ldimm.value);
                break;
        case OP_LDADR:
                fprintf(file, "ldadr t%d %s (scope=%d)\n", self->ldadr.temp, self->ldadr.name, self->ldadr.scope);
                break;
        case OP_DEREF:
                fprintf(file, "deref t%d (size=%d)\n", self->deref.addr, self->deref.size);
                break;
        case OP_LDNAM:
                fprintf(file, "ldnam t%d %s (scope=%d) (size=%d)\n", self->ldnam.temp, self->ldnam.name, self->ldnam.scope, self->ldnam.size);
                break;
        case OP_STADR:
                fprintf(file, "stadr t%d t%d (size=%d)\n", self->stadr.addr, self->stadr.value, self->stadr.size);
                break;
        case OP_STNAM:
                fprintf(file, "stnam %s t%d (size=%d)\n", self->stnam.name, self->stnam.value, self->stnam.size);
                break;
        case OP_ADD:
        case OP_SUB:
        case OP_DIV:
        case OP_MUL:
        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
                fprintf(file, "t%d = t%d %s t%d\n", self->binop.result, self->binop.temp1,
                                opstr, self->binop.temp2);
                break;
        case OP_NL:
                fputs("newline\n", file);
                break;
        default:
                assert(false);
        }
}

void ast_ir_vector_print(struct utillib_vector const *instrs,
                FILE *file) {
        int i = 0;
        struct ast_ir const *ir;
        UTILLIB_VECTOR_FOREACH(ir, instrs) {
                fprintf(file, "%4d: ", i);
                ast_ir_print(ir, file);
                ++i;
        }
}

static inline struct symbol_entry * find_name(struct ast_ir_global const *self, char const *name)
{
        return symbol_table_find(self->symbol_table, name, CL_LEXICAL);
}

static inline int last_temp(struct ast_ir_global const *self) {
        return self->temps-1;
}

static inline int make_temp(struct ast_ir_global *self) {
        return self->temps++;
}

static int load_array(struct ast_ir_global *self, char const *name, int *base_size)
{
        struct symbol_entry const *entry;
        struct ast_ir *ir;

        entry=find_name(self, name);
        ir=create_ir(OP_LDADR);
        ir->ldadr.temp=make_temp(self);
        ir->ldadr.name=name;
        ir->ldadr.scope=entry->scope;
        *base_size=type_to_size(entry->array.base_type);
        utillib_vector_push_back(self->instrs, ir);
        return ir->ldadr.temp;
}

/*
 * Index should only compute _addr_
 */
static int emit_index(struct ast_ir_global *self, 
                struct utillib_json_value const *object, int *base_size, int vflag)
{
        struct utillib_json_value const *lhs, *rhs, *value;
        struct ast_ir *ir;

        lhs=utillib_json_object_at(object, "lhs"); 
        rhs=utillib_json_object_at(object, "rhs");
        value=utillib_json_object_at(lhs, "value");
        ir = create_ir(OP_INDEX);
        ir->index.array_addr=load_array(self, value->as_ptr, base_size);
        ir->index.base_size=*base_size;
        ir->index.index_result=emit_expr(self, rhs);
        ir->index.result = make_temp(self);
        utillib_vector_push_back(self->instrs, ir);

        if (vflag == CL_RVALUE) {
                ir=create_ir(OP_DEREF);
                ir->deref.addr=last_temp(self);
                ir->deref.size=*base_size;
                utillib_vector_push_back(self->instrs, ir);
                return ir->deref.addr;
        }
        return ir->index.result;
}

static int emit_call(struct ast_ir_global *self, struct utillib_json_value const *object)
{
        struct utillib_json_value *lhs, *rhs;
        struct utillib_json_value *value; 
        struct symbol_entry const *entry;
        struct ast_ir *ir;
        int temp;

        lhs=utillib_json_object_at(object, "lhs");
        rhs=utillib_json_object_at(object, "rhs");
        value = utillib_json_object_at(lhs, "value");
        entry = symbol_table_find(self->symbol_table, value->as_ptr, CL_GLOBAL);
        ir = create_ir(OP_CAL);
        ir->call.name = value->as_ptr;
        ir->call.argc=entry->function.argc;
        ir->call.argv=malloc(sizeof ir->call.argv[0] * ir->call.argc);

        for (int i = 0; i < entry->function.argc; ++i) {
                ir->call.argv[i]=emit_expr(self, utillib_json_array_at(rhs, i));
        }
        if (entry->function.return_type != CL_VOID) {
                ir->call.has_result = true;
                ir->call.result=make_temp(self);
                temp=ir->call.result;
        } else {
                ir->call.has_result = false;
                temp=0;
        }
        utillib_vector_push_back(self->instrs, ir);
        return temp;
}

static int emit_binary(struct ast_ir_global *self, size_t op, struct utillib_json_value const *object)
{
        struct utillib_json_value const *lhs, *rhs;
        struct ast_ir *ir;

        ir = create_ir(symbol_to_ast_opcode(op));
        lhs=utillib_json_object_at(object, "lhs");
        rhs=utillib_json_object_at(object, "rhs");
        ir->binop.temp1 = emit_expr(self, lhs);
        ir->binop.temp2 = emit_expr(self, rhs);
        ir->binop.result = make_temp(self);
        utillib_vector_push_back(self->instrs, ir);
        return ir->binop.result;
}

static void emit_assign(struct ast_ir_global *self, struct utillib_json_value const *object)
{
        struct utillib_json_value const *lhs, *rhs, *value;
        struct ast_ir *ir;
        int rhs_value;

        rhs=utillib_json_object_at(object, "rhs");
        rhs_value=emit_expr(self, rhs);
        lhs=utillib_json_object_at(object, "lhs");
        value=utillib_json_object_at(lhs, "value");
        if (value) {
                /*
                 * Variable
                 */
                struct symbol_entry const *entry;
                entry=find_name(self, value->as_ptr);
                if (entry->scope == 0) {
                        /*
                         * Global Variable
                         */
                        ir=create_ir(OP_LDADR);
                        ir->ldadr.scope=0;
                        ir->ldadr.name=value->as_ptr;
                        ir->ldadr.temp=make_temp(self);
                        utillib_vector_push_back(self->instrs, ir);
                        ir=create_ir(OP_STADR);
                        ir->stadr.addr=last_temp(self);
                        ir->stadr.value=rhs_value;
                        ir->stadr.size=type_to_size(entry->kind);
                        utillib_vector_push_back(self->instrs, ir);
                        return;
                }
                /*
                 * Local Variable
                 */
                ir=create_ir(OP_STNAM);
                ir->stnam.name=value->as_ptr;
                ir->stnam.size=type_to_size(entry->kind);
                ir->stnam.value=rhs_value;
                utillib_vector_push_back(self->instrs, ir);
                return;
        }
        /*
         * Array.
         */
        ir=create_ir(OP_STADR);
        ir->stadr.value=rhs_value;
        ir->stadr.addr=emit_index(self, lhs, &ir->stadr.size, CL_LVALUE);
        utillib_vector_push_back(self->instrs, ir);
}

/*
 * object should be a token and treated as rvalue
 */
static int emit_rvalue(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *type, *name;
        struct symbol_entry const *entry;
        struct ast_ir *ir;
        int size, temp, value;

        name = utillib_json_object_at(object, "value");
        type = utillib_json_object_at(object, "type");
        switch (type->as_size_t) {
        case SYM_IDEN:
                entry = symbol_table_find(self->symbol_table, name->as_ptr, CL_LEXICAL);
                switch (entry->kind) {
                case CL_CONST:
                        size = type_to_size(entry->constant.type);
                        value = entry->constant.value;
                        goto make_ldimm;
                case CL_ARRAY:
                case CL_FUNC:
                        goto make_ldadr;
                case CL_INT:
                case CL_CHAR:
                        goto make_ldnam;
                default:
                        assert(false);
                }
        case SYM_STRING:
                goto make_ldstr;
        case SYM_CHAR:
        case SYM_INTEGER:
                size = symbol_to_size(type->as_size_t);
                value = symbol_to_immediate(type->as_size_t, name->as_ptr);
                goto make_ldimm;
        default:
                assert(false);
        }
make_ldadr:
        ir=create_ir(OP_LDADR);
        ir->ldadr.scope=entry->scope;
        ir->ldadr.name=name->as_ptr;
        ir->ldadr.temp=make_temp(self);
        temp=ir->ldadr.temp;
        goto done;

make_ldnam:
        ir=create_ir(OP_LDNAM);
        ir->ldnam.size=type_to_size(entry->kind);
        ir->ldnam.scope=entry->scope;
        ir->ldnam.name=name->as_ptr;
        ir->ldnam.temp=make_temp(self);
        temp=ir->ldnam.temp;
        goto done;

make_ldimm:
        ir = create_ir(OP_LDIMM);
        ir->ldimm.size = size;
        ir->ldimm.value = value;
        ir->ldimm.temp=make_temp(self);
        temp=ir->ldimm.temp;
        goto done;

make_ldstr:
        ir = create_ir(OP_LDSTR);
        ir->ldstr.string = name->as_ptr;
        ir->ldstr.temp = make_temp(self);
        temp=ir->ldstr.temp;
        goto done;

done:
        utillib_vector_push_back(self->instrs, ir);
        return temp;
}

/*
 * This function calls the above, so gets to the last
 */
static int emit_expr(struct ast_ir_global *self, struct utillib_json_value const *object)
{
        struct utillib_json_value const *op;
        int base_size;
        op=utillib_json_object_at(object, "op");
        if (!op) {
                return emit_rvalue(self, object);
        }
        switch(op->as_size_t) {
        case SYM_RK:
                /*
                 * true for is_rvalue, emit_expr always
                 * gets called at rhs
                 */
                return emit_index(self, object, &base_size, CL_RVALUE);
        case SYM_RP:
                return emit_call(self, object);
        case SYM_EQ:
                emit_assign(self, object);
                return 0;
        default:
                return emit_binary(self, op->as_size_t, object);
        }
}

static void emit_scanf_stmt(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value *name, *arglist;
        struct utillib_json_value const *arg;
        struct symbol_entry const *entry;
        struct ast_ir *ir;

        arglist = utillib_json_object_at(object, "arglist");
        UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
                name = utillib_json_object_at(arg, "value");
                entry = find_name(self, name->as_ptr);
                ir=create_ir(OP_LDADR);
                ir->ldadr.scope=entry->scope;
                ir->ldadr.name=name->as_ptr;
                ir->ldadr.temp = make_temp(self);
                utillib_vector_push_back(self->instrs, ir);
                ir = create_ir(OP_READ);
                ir->read.temp = last_temp(self);
                ir->read.kind=type_to_read(entry->kind);
                utillib_vector_push_back(self->instrs, ir);
        }
}

static int write_kind(struct ast_ir_global const *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *json;
        struct symbol_entry const *entry;

        json = utillib_json_object_at(object, "op");
        if (json) { return OP_WRINT; }
        json = utillib_json_object_at(object, "type");
        switch(json->as_size_t) {
        case SYM_CHAR:
                return OP_WRCHR;
        case SYM_INTEGER:
                return OP_WRINT;
        case SYM_STRING:
                return OP_WRSTR;
        case SYM_IDEN:
                json = utillib_json_object_at(object, "value");
                entry = find_name(self, json->as_ptr);
                switch(entry->kind) {
                case CL_INT:
                        return OP_WRINT;
                case CL_CHAR:
                        return OP_WRCHR;
                default:
                        return OP_WRINT;
                }
        default: 
                assert(false);
        }
}

static void emit_printf_stmt(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *arglist;
        struct utillib_json_value const *arg;
        struct ast_ir *ir;
        arglist = utillib_json_object_at(object, "arglist");

        UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
                ir = create_ir(OP_WRITE);
                ir->write.temp = emit_expr(self, arg);
                ir->write.kind = write_kind(self, arg);
                utillib_vector_push_back(self->instrs, ir);
        }
        if (self->option->auto_newline) {
                utillib_vector_push_back(self->instrs, &ast_ir_newline);
        }
}

/*
 * An ordinary for-stmt is:
 * <init>
 * tricky_jump <body>
 * <cond>
 * cond_test: bez cond_expr <endfor>
 * <body>
 * <step>
 * loop_jump <cond>
 * <endfor>
 *
 * Notes, tricky_jump is inserted
 * for special purpose, see below.
 */
static void emit_for_stmt(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *init, *cond, *step, *stmt;
        /*
         * Since the grammar asks for an abnormal interpretation
         * of the ordinary for-statement, we add one `tricky_jmp'
         * to fulfill its odd need.
         * Essentially it asks us to execute the for-body once before
         * the for-cond is ever considered, which is achieved by inserting
         * an unconditional jump between for-init and for-cond jumping
         * directly to for-body.
         * The JTA is refilled when the cond was emitted, so we keep a ref
         * to tricky_jmp.
         */
        struct ast_ir *tricky_jump, *cond_test, *loop_jump;
        int loop_jump_jta;

        init = utillib_json_object_at(object, "init");
        cond = utillib_json_object_at(object, "cond");
        step = utillib_json_object_at(object, "step");
        stmt = utillib_json_object_at(object, "stmt");

        emit_expr(self, init);
        tricky_jump = create_ir(OP_JMP);
        utillib_vector_push_back(self->instrs, tricky_jump);
        /*
         * The JTA of loop_jump
         * is the next instr of the tricky_jump,
         * which is also the beginning of cond_test.
         */
        loop_jump_jta = utillib_vector_size(self->instrs);

        /*
         * cond.
         */
        cond_test = create_ir(OP_BEZ);
        /*
         * The result of cond is the judgement of cond_test.
         */
        cond_test->bez.temp = emit_expr(self, cond);
        utillib_vector_push_back(self->instrs, cond_test);
        /*
         * The JTA of tricky_jump is the next instr of
         * cond_test, which is also the beginning of
         * body.
         */
        if (!self->option->no_tricky_jump) {
                tricky_jump->jmp.addr = utillib_vector_size(self->instrs);
        }

        /*
         * body
         */
        emit_statement(self, stmt);

        /*
         * step
         */
        emit_expr(self, step);
        /*
         * The JTA of cond_test is the next instr
         * of loop_jump.
         */
        loop_jump = create_ir(OP_JMP);
        loop_jump->jmp.addr = loop_jump_jta;
        utillib_vector_push_back(self->instrs, loop_jump);
        cond_test->bez.addr = utillib_vector_size(self->instrs);
}

/*
 * A case_gaurd is the first instr of a
 * case clause.
 * It is a OP_BNE taking the switch-expr and
 * case_label as operands to compare and its
 * BTA is the next case_gaurd.
 * The function creates a case_gaurd without
 * BTA which should be filled in later.
 */
/*
 * Ordinary switch-case statement
 * can be interpreted as:
 * <eval-switch-expr>
 * bne <case-label-1> <switch-expr> <next-bne>
 * <case-1-stmt>
 * break_jump <endswitch>
 * bne <case-label-2> <switch-expr> <default>
 * <case-2-stmt>
 * break_jump <endswitch>
 * <default>:
 * <default-stmt>
 * <endswitch>
 */

static void emit_switch_stmt(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *label, *case_clause, *expr;
        struct utillib_json_value const *value, *case_, *stmt, *type;
        struct ast_ir *case_gaurd, *break_jump, *load_label;
        /*
         * break_jumps keeps all the break_jump of
         * the case_clauses to fill their JTA to
         * the end of switch_stmt later.
         */
        struct utillib_vector break_jumps;
        int break_jump_jta;
        int loaded_const;

        utillib_vector_init(&break_jumps);
        expr = utillib_json_object_at(object, "expr");
        case_clause = utillib_json_object_at(object, "cases");

        UTILLIB_JSON_ARRAY_FOREACH(label, case_clause) {
                case_ = utillib_json_object_at(label, "case");
                stmt = utillib_json_object_at(label, "stmt");
                if (case_) {
                        value = utillib_json_object_at(case_, "value");
                        type = utillib_json_object_at(case_, "type");

                        /*
                         * Load label constant.
                         */
                        load_label = create_ir(OP_LDIMM);
                        loaded_const = make_temp(self);
                        load_label->ldimm.temp = loaded_const;
                        load_label->ldimm.value =
                                symbol_to_immediate(type->as_size_t, value->as_ptr);
                        load_label->ldimm.size = symbol_to_size(type->as_size_t);
                        utillib_vector_push_back(self->instrs, load_label);

                        /*
                         * Case gaurd.
                         */
                        case_gaurd = create_ir(OP_BNE);
                        case_gaurd->bne.temp1 = loaded_const;
                        case_gaurd->bne.temp2 = emit_expr(self, expr);
                        utillib_vector_push_back(self->instrs, case_gaurd);

                        /*
                         * Statement.
                         */
                        emit_statement(self, stmt);
                        break_jump = create_ir(OP_JMP);
                        utillib_vector_push_back(self->instrs, break_jump);
                        utillib_vector_push_back(&break_jumps, break_jump);
                        /*
                         * Fills in the BTA of case_gaurd, which
                         * is the next instr of break_jump.
                         */
                        case_gaurd->bne.addr = utillib_vector_size(self->instrs);
                } else {
                        /*
                         * default clause does not need a break_jump.
                         */
                        emit_statement(self, stmt);
                        break_jump_jta = utillib_vector_size(self->instrs);
                }
        }

        UTILLIB_VECTOR_FOREACH(break_jump, &break_jumps) {
                break_jump->jmp.addr = break_jump_jta;
        }

        utillib_vector_destroy(&break_jumps);
}

static void emit_expr_stmt(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        emit_expr(self, utillib_json_object_at(object, "expr"));
}

/*
 * ret just transfter control flow to the clean up
 * code of the current function.
 */
static void emit_return_stmt(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *expr;
        struct ast_ir *ir;

        expr = utillib_json_object_at(object, "expr");
        ir = create_ir(OP_RET);
        if (expr) {
                ir->ret.has_result = true;
                ir->ret.result = emit_expr(self, expr);
        } else {
                ir->ret.has_result = false;
        }
        utillib_vector_push_back(self->instrs, ir);
}

static void emit_if_stmt(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *expr, *then_clause, *else_clause;
        struct ast_ir *skip_branch, *jump;

        expr = utillib_json_object_at(object, "expr");
        then_clause = utillib_json_object_at(object, "then");
        skip_branch = create_ir(OP_BEZ);
        skip_branch->bez.temp = emit_expr(self, expr);
        utillib_vector_push_back(self->instrs, skip_branch);

        emit_statement(self, then_clause);
        else_clause = utillib_json_object_at(object, "else");
        if (else_clause) {
                jump = create_ir(OP_JMP);
                utillib_vector_push_back(self->instrs, jump);
                skip_branch->bez.addr = utillib_vector_size(self->instrs);
                emit_statement(self, else_clause);
                jump->jmp.addr = utillib_vector_size(self->instrs);
        } else {
                skip_branch->bez.addr = utillib_vector_size(self->instrs);
        }
}

static void emit_statement(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        if (object == &utillib_json_null)
                return;
        struct utillib_json_value const *type =
                utillib_json_object_at(object, "type");
        switch (type->as_size_t) {
        case SYM_PRINTF_STMT:
                emit_printf_stmt(self, object);
                return;
        case SYM_SCANF_STMT:
                emit_scanf_stmt(self, object);
                return;
        case SYM_FOR_STMT:
                emit_for_stmt(self, object);
                return;
        case SYM_IF_STMT:
                emit_if_stmt(self, object);
                return;
        case SYM_SWITCH_STMT:
                emit_switch_stmt(self, object);
                return;
        case SYM_RETURN_STMT:
                emit_return_stmt(self, object);
                return;
        case SYM_EXPR_STMT:
                emit_expr_stmt(self, object);
                return;
        case SYM_COMP_STMT:
                emit_composite(self, object);
                return;
        default:
                assert(false);
        }
}

/*
 * Emits defvar ir.
 * defvar name(size|scope) extend(is_array)
 */
static void emit_var_defs(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *defs, *decl;
        struct utillib_json_value *name;
        struct ast_ir *ir;
        struct symbol_entry const *entry;

        defs = utillib_json_object_at(object, "var_defs");
        UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
                name = utillib_json_object_at(decl, "name");
                entry =
                        symbol_table_find(self->symbol_table, name->as_ptr, CL_LEXICAL);
                if (entry->kind == CL_ARRAY) {
                        ir = create_ir(OP_DEFARR);
                        ir->defarr.name = name->as_ptr;
                        ir->defarr.extend = entry->array.extend;
                        ir->defarr.base_size = type_to_size(entry->array.base_type);
                } else {
                        ir = create_ir(OP_DEFVAR);
                        ir->defvar.name = name->as_ptr;
                        ir->defvar.size = type_to_size(entry->kind);
                }
                utillib_vector_push_back(self->instrs, ir);
        }
}

static void emit_const_defs(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *defs, *decl;
        struct utillib_json_value const *name;
        struct symbol_entry const *entry;
        struct ast_ir *ir;

        defs = utillib_json_object_at(object, "const_defs");
        UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
                name = utillib_json_object_at(decl, "name");
                entry =
                        symbol_table_find(self->symbol_table, name->as_ptr, CL_LEXICAL);
                ir = create_ir(OP_DEFCON);
                ir->defcon.name = name->as_ptr;
                ir->defcon.size = type_to_size(entry->constant.type);
                ir->defcon.value = entry->constant.value;
                utillib_vector_push_back(self->instrs, ir);
        }
}

/*
 * Wrapper around emit_const_defs
 * and emit_var_defs.
 * Notes it will insert const_decls and
 * var_decls into symbol_table.
 */
static void maybe_emit_decls(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *decl, *const_decls, *var_decls;

        const_decls = utillib_json_object_at(object, "const_decls");
        var_decls = utillib_json_object_at(object, "var_decls");
        if (const_decls) {
                UTILLIB_JSON_ARRAY_FOREACH(decl, const_decls) {
                        emit_const_defs(self, decl);
                }
        }
        if (var_decls) {
                UTILLIB_JSON_ARRAY_FOREACH(decl, var_decls) { emit_var_defs(self, decl); }
        }
}

static void maybe_insert_decls(struct utillib_json_value const *self,
                struct symbol_table *symbol_table) {
        struct utillib_json_value const *object, *const_decls, *var_decls;

        const_decls = utillib_json_object_at(self, "const_decls");
        var_decls = utillib_json_object_at(self, "var_decls");
        if (const_decls) {
                UTILLIB_JSON_ARRAY_FOREACH(object, const_decls) {
                        symbol_table_insert_const(symbol_table, object);
                }
        }
        if (var_decls) {
                UTILLIB_JSON_ARRAY_FOREACH(object, var_decls) {
                        symbol_table_insert_variable(symbol_table, object);
                }
        }
}

static void emit_composite(struct ast_ir_global *self,
                struct utillib_json_value const *object) {
        struct utillib_json_value const *statement, *stmts;
        stmts = utillib_json_object_at(object, "stmts");
        /*
         * FIXME: change AST format, use array here
         */
        if (stmts)
                UTILLIB_JSON_ARRAY_FOREACH(statement, stmts) {
                        emit_statement(self, statement);
                }
}

static struct ast_function *ast_function_create(char const *name) {
        struct ast_function *self = malloc(sizeof *self);
        self->name = name;
        utillib_vector_init(&self->instrs);
        utillib_vector_init(&self->init_code);
        return self;
}

static void ast_function_destroy(struct ast_function *self) {
        utillib_vector_destroy_owning(&self->instrs, ast_ir_destroy);
        utillib_vector_destroy_owning(&self->init_code, ast_ir_destroy);
        free(self);
}

void ast_program_init(struct ast_program *self, struct option const *option)
{
        self->option=option;
        utillib_vector_init(&self->init_code);
        utillib_vector_init(&self->funcs);
}

void ast_program_destroy(struct ast_program *self) {
        utillib_vector_destroy_owning(&self->funcs, ast_function_destroy);
        utillib_vector_destroy_owning(&self->init_code, ast_ir_destroy);
}

static void emit_ret_addr(struct utillib_vector *instrs) {
        int addr;
        struct ast_ir *ir;

        addr = utillib_vector_size(instrs);
        UTILLIB_VECTOR_FOREACH(ir, instrs)
                if (ir->opcode == OP_RET)
                        ir->ret.addr = addr;
        utillib_vector_push_back(instrs, &ast_ir_nop);
}

/*
 * Create an ast_function from the func_node.
 */
        static struct ast_function *
ast_ir_emit_function(struct ast_ir_global *global,
                struct utillib_json_value const *func_node)
{
        struct ast_function *self;
        struct utillib_json_value const *name, *arglist, *arg, *comp;
        struct symbol_entry const *entry;
        struct ast_ir *ir;

        /*
         * AST Retrieve
         */
        name = utillib_json_object_at(func_node, "name");
        entry =
                symbol_table_find(global->symbol_table, name->as_ptr, CL_GLOBAL);
        arglist = utillib_json_object_at(func_node, "arglist");
        comp = utillib_json_object_at(func_node, "comp");

        /*
         * symbol_table insersion.
         * Attention! maybe_insert_decls must come
         * before maybe_emit_decls for all those symbols
         * to be reachable!
         * FIXME: enhance the symbol_table so that every local
         * scope is retained and need no reinsertion
         */
        maybe_insert_decls(comp, global->symbol_table);
        symbol_table_insert_arglist(global->symbol_table, arglist);

        /*
         * init_code emision
         * defunc ...
         * para ...
         * const
         * var...
         */
        self = ast_function_create(name->as_ptr);
        ir = create_ir(OP_DEFUNC);
        ir->defunc.name = name->as_ptr;
        /*
         * FIXME: unify the type and size with type-system so all the convertor
         * disappear
         */
        ir->defunc.return_size = type_to_size(entry->function.return_type);
        utillib_vector_push_back(&self->init_code, ir);
        UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
                name = utillib_json_object_at(arg, "name");
                entry =
                        symbol_table_find(global->symbol_table, name->as_ptr, CL_LOCAL);
                ir = create_ir(OP_PARA);
                ir->para.name = name->as_ptr;
                ir->para.size = type_to_size(entry->kind);
                utillib_vector_push_back(&self->init_code, ir);
        }
        global->instrs = &self->init_code;
        maybe_emit_decls(global, comp);

        /*
         * instrs emision
         */
        global->instrs=&self->instrs;
        global->temps=0;
        emit_composite(global, comp);
        emit_ret_addr(&self->instrs);
        self->temps=global->temps;
        return self;
}

void ast_ir_emit_program(struct ast_program *self,
                struct utillib_json_value const *object,
                struct symbol_table *symbol_table) {
        struct ast_ir_global global;
        struct utillib_json_value const *func_decls, *func_node;
        struct ast_function *ast_func;

        global.symbol_table=symbol_table;
        global.instrs = &self->init_code;
        global.option=self->option;

        maybe_emit_decls(&global, object);
        func_decls = utillib_json_object_at(object, "func_decls");

        UTILLIB_JSON_ARRAY_FOREACH(func_node, func_decls) {
                symbol_table_enter_scope(symbol_table);
                ast_func = ast_ir_emit_function(&global, func_node);
                symbol_table_leave_scope(symbol_table);
                utillib_vector_push_back(&self->funcs, ast_func);
        }
}

void ast_program_print(struct ast_program const *self, FILE *file) {
        struct ast_function const *func;
        ast_ir_vector_print(&self->init_code, file);
        fputs("\n", file);
        UTILLIB_VECTOR_FOREACH(func, &self->funcs) {
                ast_ir_vector_print(&func->init_code, file);
                ast_ir_vector_print(&func->instrs, file);
                fputs("\n", file);
        }
}

/*
 * Fixup address from old to new in one pass
 * FIXME: jump address code spreads 3 places which
 * result in error easily. use a better ast_ir structure?
 */
void ast_ir_fix_address(struct utillib_vector *instrs,
                unsigned int const *address_map) {
        struct ast_ir *ast_ir;
        unsigned int old_address;

        UTILLIB_VECTOR_FOREACH(ast_ir, instrs) {
                switch (ast_ir->opcode) {
                case OP_BEZ:
                        old_address = ast_ir->bez.addr;
                        ast_ir->bez.addr = address_map[old_address];
                        break;
                case OP_BNE:
                        old_address = ast_ir->bne.addr;
                        ast_ir->bne.addr = address_map[old_address];
                        break;
                case OP_JMP:
                        old_address = ast_ir->jmp.addr;
                        ast_ir->jmp.addr = address_map[old_address];
                        break;
                case OP_RET:
                        old_address=ast_ir->ret.addr;
                        ast_ir->ret.addr=address_map[old_address];
                        break;
                default:
                        assert(!ast_ir_is_local_jump(ast_ir));
                }
        }
}

bool ast_ir_is_local_jump(struct ast_ir const *ast_ir) {
        switch(ast_ir->opcode) {
        case OP_BEZ:
        case OP_BNE:
        case OP_JMP:
        case OP_RET:
                return true;
        default:
                return false;
        }
}

int ast_ir_get_jump_address(struct ast_ir const *ast_ir) {
        switch(ast_ir->opcode) {
        case OP_BEZ:
                return ast_ir->bez.addr;
        case OP_BNE:
                return ast_ir->bne.addr;
        case OP_JMP:
                return ast_ir->jmp.addr;
        case OP_RET:
                return ast_ir->ret.addr;
        default:
                assert(!ast_ir_is_local_jump(ast_ir));
        }
}

bool ast_ir_useless_jump(struct ast_ir const *ast_ir, int ast_pc) {
        int jump_addr=ast_ir_get_jump_address(ast_ir);
        return jump_addr==ast_pc+1;
}

int ast_ir_get_assign_target(struct ast_ir const *ir)
{
        switch(ir->opcode) {
        default: 
                return -1;
        case OP_ADD:
        case OP_SUB:
        case OP_DIV:
        case OP_MUL:
        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
                return ir->binop.result;
        case OP_DEREF:
                return ir->deref.addr;
        case OP_LDADR:
                return ir->ldadr.temp;
        case OP_LDNAM:
                return ir->ldnam.temp;
        case OP_LDIMM:
                return ir->ldimm.temp;
        case OP_LDSTR:
                return ir->ldstr.temp;
        case OP_READ:
                return ir->read.temp;
        case OP_CAL:
                if (ir->call.has_result)
                        return ir->call.result;
                return -1;
        case OP_INDEX:
                return ir->index.result;
        }
}
