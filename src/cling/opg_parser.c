#include "opg_parser.h"

void cling_opg_parser_init(struct cling_opg_parser *self)
{
  utillib_vector_init(&self->stack);
}

void cling_opg_parser_destroy(struct cling_opg_parser *self)
{
  utillib_vector_destroy(&self->stack);
}

struct utillib_json_value *
cling_opg_parser_parse(struct cling_opg_parser *self,
    struct utillib_token_scanner *input)
{





}

 
