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
#include <utillib/flags.h>
#include <utillib/enum.h>

/**
 * \enum pascal_main_option_kind
 * Supported options.
 */
UTILLIB_ENUM_BEGIN(pascal_main_option_kind)
  UTILLIB_ENUM_ELEM(PASCAL_DUMP_TABLE)
  UTILLIB_ENUM_ELEM(PASCAL_DUMP_FIRST)
  UTILLIB_ENUM_ELEM(PASCAL_DUMP_FOLLOW)
UTILLIB_ENUM_END(pascal_main_option_kind);

UTILLIB_ARGP_OPTION_BEGIN()
  UTILLIB_ARGP_OPTION_ELEM("dump-table", PASCAL_DUMP_TABLE, "FILE", 
      "dump the LL(1) table in JSON format")
  UTILLIB_ARGP_OPTION_ELEM("dump-first", PASCAL_DUMP_FIRST, "FILE",
      "dump the LL(1) FIRST sets")
  UTILLIB_ARGP_OPTION_ELEM("dump-follow", PASCAL_DUMP_FIRST, "FILE",
      "dump the LL(1) FOLLOW sets")
UTILLIB_ARGP_OPTION_END();
  
int UTILLIB_ARGP_STATIC_PARSER (int key, char *arg, struct argp_state *state)
{
  switch(key) {
  case PASCAL_DUMP_TABLE:


  }
}

int main(int argc, char **argv) 
{


}
