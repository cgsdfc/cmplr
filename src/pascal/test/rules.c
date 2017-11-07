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
#include "rules.h"
#include "pascal/rules.h"
#include "pascal/symbols.h"
#include <utillib/test.h>
#include <utillib/ll1_builder.h>

size_t const pascal_ll1_FIRST[][PASCAL_LL1_EXTEND]={
  [SYM_PROGRAM]={ },
  [SYM_SUBPRG]={ },
  [SYM_CONST_DECL]={ },
  [SYM_CONST_DEF]={ },
  [SYM_VAR_DECL]={ },
  [SYM_PROC_HD]={ },
  [SYM_STMT]={ },
  [SYM_ASS_STMT]={ },
  [SYM_COND_STMT]={ },
  [SYM_LOOP_STMT]={ },
  [SYM_CALL_STMT]={ },
  [SYM_COMP_STMT]={ },
  [SYM_READ_STMT]={ },
  [SYM_WRITE_STMT]={ },
  [SYM_EXPR]={ },
  [SYM_TERM]={ },
  [SYM_FACT]={ },
  [SYM_SIGN]={ },
  [SYM_ADD_OP]={ },
  [SYM_MUL_OP]={ },
  [SYM_TERM_LS]={ },
  [SYM_COND]={ },
  [SYM_FACT_LS]={ },
  [SYM_REL_OP]={ },
  [SYM_EXPR_LS]={ },
  [SYM_PROC_DECL]={ },
  [SYM_CONST_DEF_LS]={ },
  [SYM_IDEN_LS]={ },
  [SYM_STMT_LS]={ },
  [SYM_PROC_DECL_LS]={ },

};

size_t const pascal_ll1_FOLLOW[][PASCAL_LL1_EXTEND]={
  [SYM_PROGRAM]={ },
  [SYM_SUBPRG]={ },
  [SYM_CONST_DECL]={ },
  [SYM_CONST_DEF]={ },
  [SYM_VAR_DECL]={ },
  [SYM_PROC_HD]={ },
  [SYM_STMT]={ },
  [SYM_ASS_STMT]={ },
  [SYM_COND_STMT]={ },
  [SYM_LOOP_STMT]={ },
  [SYM_CALL_STMT]={ },
  [SYM_COMP_STMT]={ },
  [SYM_READ_STMT]={ },
  [SYM_WRITE_STMT]={ },
  [SYM_EXPR]={ },
  [SYM_TERM]={ },
  [SYM_FACT]={ },
  [SYM_SIGN]={ },
  [SYM_ADD_OP]={ },
  [SYM_MUL_OP]={ },
  [SYM_TERM_LS]={ },
  [SYM_COND]={ },
  [SYM_FACT_LS]={ },
  [SYM_REL_OP]={ },
  [SYM_EXPR_LS]={ },
  [SYM_PROC_DECL]={ },
  [SYM_CONST_DEF_LS]={ },
  [SYM_IDEN_LS]={ },
  [SYM_STMT_LS]={ },
  [SYM_PROC_DECL_LS]={ },

};
struct utillib_rule_index  RuleIndex ;
struct utillib_ll1_builder LL1Builder;
struct utillib_vector2 LL1Table;


UTILLIB_TEST_SET_UP()
{
  utillib_rule_index_init(&RuleIndex, pascal_symbols, pascal_rules);
  utillib_ll1_builder_init(&LL1Builder, &RuleIndex);
  utillib_ll1_builder_build_table(&LL1Builder, &LL1Table);
}

UTILLIB_TEST_TEAR_DOWN()
{
  utillib_rule_index_destroy(&RuleIndex);
  utillib_ll1_builder_destroy(&LL1Builder);
  utillib_vector2_destroy(&LL1Table);
}

UTILLIB_TEST(pascal_ll1_table_correct) 
{
  struct utillib_json_value_t *val = utillib_vector2_json_array_create(
      &LL1Table, utillib_rule_json_object_create);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST(pascal_ll1_set_correct)
{
  struct utillib_json_value_t *val=utillib_ll1_builder_json_object_create(
      &LL1Builder
  );
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_DEFINE(Pascal_Rules) 
{
  UTILLIB_TEST_BEGIN(Pascal_Rules)
    UTILLIB_TEST_RUN(pascal_ll1_set_correct)
    UTILLIB_TEST_SKIP(pascal_ll1_table_correct)
  UTILLIB_TEST_END(Pascal_Rules)
  UTILLIB_TEST_FIXTURE(struct utillib_test_dummy_t)
  UTILLIB_TEST_RETURN(Pascal_Rules)
}

