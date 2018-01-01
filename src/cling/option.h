#ifndef CLING_OPTION_H
#define CLING_OPTION_H
#include <stdbool.h>
/*
 * As we may have non-standard extension like
 * no-tricky-jump, allow-comment.
 */

struct cling_option {
  bool allow_comment;
  bool no_tricky_jump;
  bool auto_newline;
};

#endif /* CLING_OPTION_H */
