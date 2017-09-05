add_executable(digraph digraph.c)
set(CMAKE_CXX_FLAGS "-std=c++11 -g3")
  add_library(parser2
# ./debug.cpp
# ./symbol_stream.cpp
# ./parser.cpp
# ./printer.cpp
  ./language.cpp
  ./rule_tree.cpp
# ./small_lang.cpp
# ./clang.cpp
# ./grammar.cpp
  )

# add_executable(
#  cpp_parser 
#  driver.cpp
# )
# target_link_libraries(
#  cpp_parser 
#  parser2
#  $ENV{HOME}/download/local/lib/libboost_serialization.a
# )
#add_executable(
#  dumper
#  ./dumper.cpp
#)
#target_link_libraries(
#  dumper
#  parser2
#)

add_executable(
  unique_map
  ./unique_map.cpp
)

target_link_libraries(
  unique_map
  parser2
)
