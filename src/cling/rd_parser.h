#ifndef CLING_RD_PARSER_H
#define CLING_RD_PARSER_H
#include <utillib/vector.h>

struct cling_rd_parser {
  struct utillib_vector elist;
};

void cling_rd_parser_init(struct cling_rd_parser *self);
void cling_rd_parser_destroy(struct cling_rd_parser *self);
#endif /* CLING_RD_PARSER_H */

