set(CMAKE_C_FLAGS "-g3 -std=c99")
include_directories(${CMAKE_SOURCE_DIR})
add_library(utillib
  ./scanner.c
  ./ll1_parser.c
  ./ll1_parser.c
  ./vector2.c
  ./bitset.c
  ./unordered_op.c
  ./ll1_builder.c
  ./symbol.c
  ./rule.c
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

link_libraries(utillib)

add_executable(utillib_test 
  ./test/scanner.c
  ./test/ll1_parser.c
  ./test/print.c
  ./test/vector2.c
  ./test/bitset.c
  ./test/ll1_builder.c
  ./test/hash.c
  ./test/symbol.c
  ./test/rule.c
  ./test/json.c
  ./test/hash.c
  ./test/string.c
  ./test/test.c
  ./test/slist.c
  ./test/flags.c
  ./test/vector.c
  ./test/strref.c
  ./test/logging.c
  ./test/driver.c
  )

add_executable(example-json example/json.c)
