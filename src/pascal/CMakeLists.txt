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

project(pascal  C)

include_directories(${CMAKE_SOURCE_DIR})
set(PASCAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
set(PASCAL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/)
add_definitions(-DPASCAL_TEST_DIR=\"${PASCAL_TEST_DIR}\")

add_library(pascal
  ./parser.c
  ./scanner.c
  ./symbols.c
  ./rules.c
  ./ast.c
)

add_executable(pascal_test
  ./test/scanner.c
  ./test/parser.c
  ./test/driver.c
  ./test/rules.c
  ./test/symbols.c
)

target_link_libraries(pascal_test
  pascal
  utillib
)

add_executable(pascal_ll1_table_gen ./ll1_table_gen.c)
target_link_libraries(ll1_generator pascal utillib)

