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
add_definitions(-DPASCAL_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}/src/pascal/\")
add_definitions(-DPASCAL_TEST_DIR=\"${CMAKE_SOURCE_DIR}/src/pascal/test/\")
add_definitions(-DPASCAL_TEST_DATA_DIR=\"${CMAKE_SOURCE_DIR}/src/pascal/test/data/\")
add_definitions(-DUTILLIB_TEST_DATA_DIR=\"${CMAKE_SOURCE_DIR}/src/utillib/test/data/\")

# For strdup
add_definitions(-D_POSIX_C_SOURCE=200809L)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
enable_testing()

if (MSVC OR WIN32)
  set(CMPLR_PLATFORM "WINDOWS")
else(MSVC OR WIN32)
  set(CMPLR_PLATFORM "LINUX")
endif(MSVC OR WIN32)

if (CMPLR_PLATFORM STREQUAL "LINUX")
  set(CMAKE_C_FLAGS "-g3 -std=c99 -Wunused-variable")
endif(CMPLR_PLATFORM STREQUAL "LINUX")

## Utillib

add_library(utillib
  ./src/utillib/hashmap.c
  ./src/utillib/graph.c
  ./src/utillib/strhash.c
  ./src/utillib/hashmap_impl.c
  ./src/utillib/test_impl.c
  ./src/utillib/json_parser.c
  ./src/utillib/json_scanner.c
  ./src/utillib/json_parser_impl.c
  ./src/utillib/ll1_builder_impl.c
  ./src/utillib/ll1_generator.c
  ./src/utillib/scanner.c
  ./src/utillib/ll1_parser.c
  ./src/utillib/vector2.c
  ./src/utillib/bitset.c
  ./src/utillib/ll1_builder.c
  ./src/utillib/symbol.c
  ./src/utillib/rule.c
  ./src/utillib/pair.c
  ./src/utillib/json.c
  ./src/utillib/test.c
  ./src/utillib/print.c
  ./src/utillib/slist.c
  ./src/utillib/vector.c
  ./src/utillib/string.c)

## Cling
add_library(cling
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
  )	
target_link_libraries(cling utillib)

# add_executable(cling_test
#   ./src/cling/test/scanner.c
#   ./src/cling/test/symbol_table.c
#   ./src/cling/test/driver.c
#   )
# target_link_libraries(cling_test cling)
# add_test(Cling_Test cling_test)
# add_executable(cling-scanner ./src/cling/smain.c)
# target_link_libraries(cling-scanner cling)
set(cling-app cling-parser)
foreach(app ${cling-app})
  add_executable(${app} ./src/cling/${app}.c)
  target_link_libraries(${app} cling)
endforeach()

# add_library(cling++ ./src/cling/cling.cpp)
# target_link_libraries(cling++ cling utillib++)
# add_executable(clingcppmain ./src/cling/cppmain.cpp)
# target_link_libraries(clingcppmain cling++)

