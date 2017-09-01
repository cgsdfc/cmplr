add_executable(digraph digraph.c)
set(CMAKE_CXX_FLAGS "-std=c++11 -g3")
add_library(parser2
  ./language.cpp
  ./small_lang.cpp
  ./clang.cpp
  ./grammar.cpp
  ./grammar_base.cpp
)

add_executable(cpp_parser parser2.cpp)
target_link_libraries(cpp_parser 
  parser2
  $ENV{HOME}/download/local/lib/libboost_serialization.a
)


