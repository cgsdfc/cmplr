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
#ifndef CLING_RD_PARSER_IMPH_H
#define CLING_RD_PARSER_IMPH_H

#include <utillib/scanner.h>

#define RD_PARSER_SKIP_MAX 4


struct rd_parser_skip_target {
  size_t expected;
  int tars[RD_PARSER_SKIP_MAX];
};

void rd_parser_skip_target_init(struct rd_parser_skip_target * self,
    size_t expected);

size_t rd_parser_skipto(struct rd_parser_skip_target const *self,
    struct utillib_token_scanner *input); 


#endif /* CLING_RD_PARSER_IMPH_H */
