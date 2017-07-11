#ifndef GRAMMAR_H
#define GRAMMAR_H 1
#include "rules.h"
#include "item.h"

void init_all(grammar *);


extern int program;
extern int tran_unit;
extern int ext_dclr;
extern int func_def;
extern int dclr;
extern int dclr_spfr;
extern int dcltor;
extern int dclr_list;
 

extern int init_dcltor_list;
extern int stcl_spfr;
extern int type_spfr;
extern int type_qlfr;
extern int struct_union_spfr;
extern int enum_spfr;
extern int struct_or_union;
extern int struct_dclr_list;
extern int struct_dclr;
extern int init;
extern int init_dclr;
extern int spfr_qlfr_list;
extern int struct_dcltor_list;
extern int struct_dcltor;
extern int id;
extern int float_const;
extern int int_const;
extern int string_const;
extern int char_const;
extern int eof;
extern int left_brace;
extern int right_brace;
extern int left_bkt;
extern int right_bkt;
extern int left_pare;
extern int right_pare;
extern int equal;
extern int _enum;
extern int star;
extern int comma;
extern int dot;
extern int colon;
extern int semi;
extern int _sizeof;
extern int question;
extern int plus_plus;
extern int minus_minus;
extern int _while;

extern int block;
extern int _sizeof;
extern int comma;
extern int dot;
extern int colon;
extern int float_const;
extern int int_const;
extern int string_const;
extern int char_const;
extern int assign_oper;
extern int assign_expr;
extern int cond_expr;
extern int lg_or_expr;
extern int lg_and_expr;
extern int incl_or_expr;
extern int excl_or_expr;
extern int unary_expr;
extern int and_expr;
extern int eq_expr;
extern int rel_expr;
extern int shift_expr;
extern int add_expr;
extern int mul_expr;
extern int cast_expr;
extern int unary_oper;
extern int postfix;
extern int primary;
extern int arglist;
extern int rel_oper;
extern int mul_oper;
extern int add_oper;
extern int eq_oper;
extern int shift_oper;
extern int plus_plus;
extern int minus_minus;
extern int question;
extern int const_expr;
extern int expr;
extern int enumtor_list;
extern int enumtor;
extern int _enum;
extern int enum_spfr;
extern int type_name;
extern int stmt;
extern int labeled_stmt;
extern int expr_stmt;
extern int selec_stmt;
extern int iter_stmt;
extern int jmp_stmt;
extern int stmt_list;
extern int _while;
extern int dclr_list;

#endif
