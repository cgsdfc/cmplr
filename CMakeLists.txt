# 
#  Cmplr Library
#  Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
#  02110-1301 USA

project(cmplr C CXX)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file (STRINGS "VERSION" CMPLR_VERSION)
include_directories(src)

# For strdup
add_definitions(-D_POSIX_C_SOURCE=200809L)
set(CMAKE_CXX_FLAGS "-std=c++11 -g")

if (MSVC OR WIN32)
  set(CMPLR_PLATFORM "WINDOWS")
else(MSVC OR WIN32)
  set(CMPLR_PLATFORM "LINUX")
endif(MSVC OR WIN32)

if (CMPLR_PLATFORM STREQUAL "LINUX")
  set(CMAKE_C_FLAGS "-g3 -std=c99 -Wunused-variable -Wno-incompatible-pointer-types -Wno-discarded-qualifiers")
endif(CMPLR_PLATFORM STREQUAL "LINUX")

## Utillib

add_library(utillib
  ./src/utillib/list.c 
  ./src/utillib/hashmap.c
  ./src/utillib/graph.c
  ./src/utillib/strhash.c
  ./src/utillib/hashmap_impl.c
  ./src/utillib/scanner.c
  ./src/utillib/vector2.c
  ./src/utillib/bitset.c
  ./src/utillib/symbol.c
  ./src/utillib/pair.c
  ./src/utillib/json.c
  ./src/utillib/print.c
  ./src/utillib/slist.c
  ./src/utillib/vector.c
  ./src/utillib/string.c)

## Cling
add_library(cling-core
  ./src/cling/symbol_table.c
  ./src/cling/symbols.c 
  ./src/cling/scanner.c
  ./src/cling/rd_parser.c
  ./src/cling/ast_build.c
  ./src/cling/ast_pretty.c
  ./src/cling/ast_check.c
  ./src/cling/ast_ir.c
  ./src/cling/error.c
  ./src/cling/opg_parser.c
  ./src/cling/mips.c
  ./src/cling/misc.c
  ./src/cling/lcse.c
  ./src/cling/basic_block.c
  ./src/cling/optimize.c
  ./src/cling/mips_interp.c
  ./src/cling/cling-core.c
  )	
target_link_libraries(cling-core utillib)

add_executable(cling ./src/cling/cling.c)
target_link_libraries(cling cling-core)
