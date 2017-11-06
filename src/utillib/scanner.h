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
#ifndef UTILLIB_ABSTRACT_SCANNER_H
#define UTILLIB_ABSTRACT_SCANNER_H
#include <stddef.h>  // size_t
#include <stdbool.h> // for bool

struct utillib_scanner_op {
  size_t (* lookahead ) (void *);
  void (* shiftaway ) (void *);
  bool (* reacheof ) (void *);
  void * (* getsymbol) (void *);
};

struct utillib_symbol_scanner {
  size_t const * symbols;
  struct utillib_symbol const * table;
};

extern const struct utillib_scanner_op utillib_symbol_scanner_op;

void utillib_symbol_scanner_init(struct utillib_symbol_scanner *self,
    size_t const * symbols, struct utillib_symbol const *table);

void utillib_symbol_scanner_shiftaway(struct utillib_symbol_scanner *self);
size_t utillib_symbol_scanner_lookahead(struct utillib_symbol_scanner *self);
bool utillib_symbol_scanner_reacheof(struct utillib_symbol_scanner *self);
void * utillib_symbol_scanner_getsymbol(struct utillib_symbol_scanner *self);
#endif // UTILLIB_ABSTRACT_SCANNER_H