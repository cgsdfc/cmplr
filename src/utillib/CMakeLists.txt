set(CMAKE_C_FLAGS "-g3")
include_directories(${CMAKE_SOURCE_DIR})
set(UTILLIB_SOURCE
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
add_library(utillib
  ${UTILLIB_SOURCE}
)

set(UTILLIB_TEST_SOURCE 
  ./test/test.c
  ./test/slist.c
  ./test/argp.c
  ./test/vector.c
  ./test/strref.c
  ./test/logging.c
)
add_library(utillib_test_lib
  ${UTILLIB_TEST_SOURCE}
)

add_executable(utillib_test 
  ./test/driver.c
)

target_link_libraries(utillib_test
  utillib_test_lib
  utillib
)

