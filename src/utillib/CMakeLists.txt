set(CMAKE_C_FLAGS "-g3 -std=c99")
include_directories(${CMAKE_SOURCE_DIR})
add_library(utillib
  ./pair.c
  ./json.c
  ./logging.c
  ./strref.c
  ./test.c
  ./print.c
  ./equal.c
  ./hash.c
  ./unordered_map.c
  ./error.c
  ./enum.c
  ./slist.c
  ./vector.c
  ./string.c
)

add_library(utillib_test_lib
  ./test/json.c
  ./test/string.c
  ./test/test.c
  ./test/slist.c
  ./test/argp.c
  ./test/vector.c
  ./test/strref.c
  ./test/logging.c
)

add_executable(utillib_test 
  ./test/driver.c
)

target_link_libraries(utillib_test
  utillib_test_lib
  utillib
)

link_libraries(utillib)
add_executable(example-json example/json.c)
