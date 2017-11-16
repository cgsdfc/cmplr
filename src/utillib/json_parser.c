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

#include "json_parser.h"
#include "json_parser_impl.h"

static void json_parser_rule_handler(struct utillib_json_parser *self,
    struct utillib_rule const * rule)
{
  struct utillib_symbol const * lhs=rule->LHS;
  struct utillib_json_value_t const * val;
  switch(lhs->value) {
  case UT_JSON_SYM_ARR:
    val=utillib_json_array_create_empty();
    utillib_vector_push_back(&self->values, val);
    return;
  case UT_JSON_SYM_OBJ:
    val=utillib_json_object_create_empty();
    utillib_vector_push_back(&self->values, val);
    return;
  }

}

static void json_parser_terminal_handler(struct utillib_json_parser *self,
    struct utillib_symbol const *symbol)
{
  switch (symbol->value) {



  }

}

static void json_parser_error_handler(struct utillib_json_parser *self,
    struct utillib_ll1_parser_error const *error)
{


}

static const struct utillib_ll1_parser_op json_parser_op={
  .terminal_handler=json_parser_terminal_handler,
  .rule_handler=json_parser_rule_handler,
  .error_handler=json_parser_rule_handler,
};


void utillib_json_parser_factory_init(struct utillib_json_parser_factory *self)
{
  utillib_ll1_factory_init(&self->factory, utillib_json_symbols, utillib_json_rules);
}

void utillib_json_parser_init(struct utillib_json_parser *self, struct utillib_json_parser_factory *factory)
{
  utillib_ll1_factory_parser_init(&factory->factory, &self->parser, self, &json_parser_op);
  utillib_vector_init(&self->values);
}

void utillib_json_parser_destroy(struct utillib_json_parser *self)
{
  utillib_vector_destroy(&self->values);
  utillib_ll1_parser_destroy(&self->parser);
}

int utillib_json_parser_parse(struct utillib_json_parser *self, char const * str)
{


}

struct utillib_json_value_t * utillib_json_parser_value(struct utillib_json_parser *self)
{




}


