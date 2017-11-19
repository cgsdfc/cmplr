set(CMAKE_C_FLAGS "-g3 -std=c99")
include_directories(${CMAKE_SOURCE_DIR})
add_library(utillib
  ./test_impl.c
  ./json_parser.c
  ./json_parser_impl.c
  ./ll1_builder_impl.c
  ./ll1_generator.c
  ./opg_builder.c
  ./scanner.c
  ./ll1_parser.c
  ./vector2.c
  ./bitset.c
  ./unordered_op.c
  ./ll1_builder.c
  ./symbol.c
  ./rule.c
  ./pair.c
  ./json.c
  ./test.c
  ./print.c
  ./equal.c
  ./hash.c
  ./unordered_map.c
  ./slist.c
  ./vector.c
  ./string.c
  )

link_libraries(utillib)

add_executable(utillib_test 
  ./test/ll1_generator.c
  ./test/non_ll1_sample_1.c
  ./test/non_ll1_sample_2.c
  ./test/ll1_sample_1.c
  ./test/ll1_sample_2.c
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
  ./test/json_parser.c
  ./test/driver.c
  )

add_executable(example-json example/json.c)
