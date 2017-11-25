#ifndef CLING_RD_PARSER_H
#define CLING_RD_PARSER_H
#include <utillib/vector.h>
#include <utillib/json.h>
#include <utillib/scanner.h>
#include <setjmp.h>

struct cling_rd_parser {
  jmp_buf fatal_saver;
  struct utillib_vector elist;
};

void cling_rd_parser_init(struct cling_rd_parser *self);
void cling_rd_parser_destroy(struct cling_rd_parser *self);
struct utillib_json_value *
cling_rd_parser_parse(struct cling_rd_parser *self,
    struct utillib_token_scanner *input);

#endif /* CLING_RD_PARSER_H */

