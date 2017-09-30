cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS "-g3")
include_directories(${CMAKE_SOURCE_DIR})
set(UTILLIB_SOURCE
  ./slist.c
  ./vector.c
  ./string.c
)
add_library(utillib
  ${UTILLIB_SOURCE}
)

