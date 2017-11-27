#ifndef CLING_OPG_PARSER
#define CLING_OPG_PARSER
#include "symbols.h"
#include <utillib/vector.h>

/**
 * \struct cling_opg_parser
 * This parser will parse every
 * occurance of EXPRESSION.
 * It uses operator-priority-precedence
 * way to parse thing buttom-up.
 * The result is the reversed-polan
 * representation in a json array.
 * If error happened, there result
 * will be null.
 * The for each element in the array,
 * the object is either an operator or
 * an operand so there is an `is_operand'
 * bool field to tell that.
 * If `is_operand' is true, this object
 * has `value' and `kind' field.
 * If `is_operand' is false, this object
 * has a `op' size_t field to indicate
 * the operator.
 */

struct cling_opg_parser {
  struct utillib_vector stack;
};

#endif /* CLING_OPG_PARSER */
