add_executable(digraph digraph.c)
set(CMAKE_CXX_FLAGS "-std=c++11 -g3")
add_library(parser2
  ./clang.cpp
)

add_executable(cpp_parser parser2.cpp)
target_link_libraries(cpp_parser 
  parser2
  $ENV{HOME}/download/local/lib/libboost_serialization.a
)

add_executable(unique_map
  unique_map.cpp)

add_executable(grammar grammar.cpp)
