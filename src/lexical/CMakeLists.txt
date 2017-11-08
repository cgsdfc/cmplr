project(scanner)

add_library(scanner
  ./token.c
  ./scanner.c
  ./input_buf.c
  ./match.c
)

add_library(scanner_test_lib
  ./test/token.c
  ./test/scanner.c
  ./test/input_buf.c
  ./test/match.c
)

add_executable(scanner_test
  ./test/driver.c
)

target_link_libraries(scanner_test
  scanner_test_lib
  scanner
  utillib
)
