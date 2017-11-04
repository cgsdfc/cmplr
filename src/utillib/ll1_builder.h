#ifndef UTILLIB_LL1_BUILDER_H
#define UTILLIB_LL1_BUILDER_H
#include "vector.h"
#include "vector2.h"
#include "enum.h"
#include "rule.h"
#include "bitset.h"

UTILLIB_ENUM_BEGIN(utillib_ll1_error_kind)
  UTILLIB_ENUM_ELEM(UT_LL1_OK)
  UTILLIB_ENUM_ELEM(UT_LL1_ENOTLL1)
UTILLIB_ENUM_END(utillib_ll1_error_kind)

/**
 * \struct utillib_ll1_set
 * A set used for both `FIRST_SET' and `FOLLOW_SET'.
 */
struct utillib_ll1_set {
  /* whether contains special symbol like `epsilon' or `eof'. */
  bool flag;
  struct utillib_bitset bitset;
};

/**
 * \struct utillib_ll1_builder
 * Builder of the LL(1) parser table.
 */
struct utillib_ll1_builder {
  struct utillib_rule_index const* rule_index;
  struct utillib_vector FIRST;
  struct utillib_vector FIRST_RULE;
  struct utillib_vector FOLLOW;
  struct utillib_vector errors;
};

void utillib_ll1_builder_init(struct utillib_ll1_builder *self,
    struct utillib_rule_index const * rule_index);
void utillib_ll1_builder_destroy(struct utillib_ll1_builder *self);
void utillib_ll1_builder_build_table(struct utillib_ll1_builder *self,
    struct utillib_vector2 * table);
int utillib_ll1_builder_check(struct utillib_ll1_builder *self);


#endif // UTILLIB_LL1_BUILDER_H
