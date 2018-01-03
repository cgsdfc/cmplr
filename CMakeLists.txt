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
  set(CMAKE_C_FLAGS "-g3 -std=c99 -Wunused-variable")
endif(CMPLR_PLATFORM STREQUAL "LINUX")

add_library(cling-core
  ./src/cling/Error.cpp
  ./src/cling/Symbol.cpp
  ./src/cling/Utility.cpp
  ./src/cling/Scanner.cpp
  ./src/cling/Core.cpp
  ./src/cling/Parser.cpp
)
# this can generate file with dep but
# cannot be depended on by top-target
# in fact it can but in another signature
# You Know CMake always put different commands
# into the same name but different _signature_
# As if Overload happens. But this overload asks
# us to put the $OUTPUT into top-target's file list
# which cause us compile Grammar.c with other cpp files.
# We don't want that happens so we _combine_ 2 commands
# to both generate a file and get the top-target depend
# on that file.

set(LEMON_GRAMMAR ${CMAKE_SOURCE_DIR}/src/cling/Grammar.y)
set(LEMON_PARSER ${CMAKE_SOURCE_DIR}/src/cling/Grammar.c)

# let parser depend on grammar
# generate parser impl
add_custom_command(OUTPUT ${LEMON_PARSER}
  DEPENDS ${LEMON_GRAMMAR}
  COMMAND lemon ${LEMON_GRAMMAR})
# let library depend on parser
add_custom_target(BUILD_LEMON_PARSER
  DEPENDS ${LEMON_PARSER})
add_dependencies(cling-core BUILD_LEMON_PARSER)
# End handling lemon

add_executable(cling ./src/cling/cling.cpp)
target_link_libraries(cling cling-core)

