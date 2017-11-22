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
set(CMAKE_CXX_FLAGS "-std=c++11")
enable_testing()

if (MSVC OR WIN32)
  set(CMPLR_PLATFORM "WINDOWS")
else(MSVC OR WIN32)
  set(CMPLR_PLATFORM "LINUX")
endif(MSVC OR WIN32)

if (CMPLR_PLATFORM STREQUAL "LINUX")
  set(CMAKE_C_FLAGS "-g3 -std=c99")
endif(CMPLR_PLATFORM STREQUAL "LINUX")

## Utillib

add_library(utillib
  ./src/utillib/test_impl.c
  ./src/utillib/json_parser.c
  ./src/utillib/json_scanner.c
  ./src/utillib/json_parser_impl.c
  ./src/utillib/ll1_builder_impl.c
  ./src/utillib/ll1_generator.c
  ./src/utillib/opg_builder.c
  ./src/utillib/scanner.c
  ./src/utillib/ll1_parser.c
  ./src/utillib/vector2.c
  ./src/utillib/bitset.c
  ./src/utillib/unordered_op.c
  ./src/utillib/ll1_builder.c
  ./src/utillib/symbol.c
  ./src/utillib/rule.c
  ./src/utillib/pair.c
  ./src/utillib/json.c
  ./src/utillib/test.c
  ./src/utillib/print.c
  ./src/utillib/equal.c
  ./src/utillib/hash.c
  ./src/utillib/unordered_map.c
  ./src/utillib/slist.c
  ./src/utillib/vector.c
  ./src/utillib/string.c)

add_executable(utillib_test 
  ./src/utillib/test/json_scanner.c
  ./src/utillib/test/ll1_generator.c
  ./src/utillib/test/non_ll1_sample_1.c
  ./src/utillib/test/non_ll1_sample_2.c
  ./src/utillib/test/ll1_sample_1.c
  ./src/utillib/test/ll1_sample_2.c
  ./src/utillib/test/scanner.c
  ./src/utillib/test/ll1_parser.c
  ./src/utillib/test/print.c
  ./src/utillib/test/vector2.c
  ./src/utillib/test/bitset.c
  ./src/utillib/test/ll1_builder.c
  ./src/utillib/test/hash.c
  ./src/utillib/test/symbol.c
  ./src/utillib/test/rule.c
  ./src/utillib/test/json.c
  ./src/utillib/test/hash.c
  ./src/utillib/test/string.c
  ./src/utillib/test/test.c
  ./src/utillib/test/slist.c
  ./src/utillib/test/vector.c
  ./src/utillib/test/json_parser.c
  ./src/utillib/test/driver.c)

target_link_libraries(utillib_test utillib)
add_test(Utillib_Test utillib_test)

add_library(utillibcpp
  ./src/utillib/cpp/scanner.cpp
  ./src/utillib/cpp/json.cpp
)
target_link_libraries(utillibcpp utillib)

add_executable(example-json ./src/utillib/example/json.c)
target_link_libraries(example-json utillib)
add_executable(example-vector ./src/utillib/example/vector.c)
target_link_libraries(example-vector utillib)
add_executable(example-string ./src/utillib/example/string.c)
target_link_libraries(example-string utillib)


## Pascal 
add_library(pascal
  ./src/pascal/parser.c
  ./src/pascal/scanner.c
  ./src/pascal/symbols.c
  ./src/pascal/rules.c
  ./src/pascal/json_ast.c
)
target_link_libraries(pascal utillib)

add_executable(pascal_test
  ./src/pascal/test/scanner.c
  ./src/pascal/test/parser.c
  ./src/pascal/test/driver.c
  ./src/pascal/test/rules.c
  ./src/pascal/test/symbols.c
)

target_link_libraries(pascal_test pascal)
add_test(Pascal_Test pascal_test)
add_executable(pascal_ll1_table_gen ./src/pascal/ll1_table_gen.c)
target_link_libraries(pascal_ll1_table_gen pascal)

## Cling
add_library(cling
  ./src/cling/symbols.c 
  ./src/cling/rules.c 
  ./src/cling/scanner.c
  )	
target_link_libraries(cling utillib)

add_executable(cling_test
  ./src/cling/test/scanner.c
  ./src/cling/test/driver.c
  )
target_link_libraries(cling_test cling)
add_test(Cling_Test cling_test)
add_executable(cling-scanner ./src/cling/smain.c)
target_link_libraries(cling-scanner cling)
