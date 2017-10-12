project(scanner)
set(scanner_source
  ./scanner.c
# ./token.c
# ./reserved.c
  ./match.c
)
add_library(scanner
  ${scanner_source}
)
add_executable(scan
  ./driver.c
)
target_link_libraries(scan
  scanner
  utillib
)
