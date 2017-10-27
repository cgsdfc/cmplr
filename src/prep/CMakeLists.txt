project(prep C)
set(prep_source 
  ./parser.c
  ./scanner.c
)
set(prep_test_source
  ./test/scanner.c
  ./test/parser.c
)

add_library(prep_test_lib
  ${prep_test_source}
)

add_library(prep
  ${prep_source}
)

target_link_libraries(prep
  scanner
  utillib
)

add_executable(prep_test
  ./test/driver.c
)
target_link_libraries(prep_test
  prep_test_lib
  prep
  utillib
  )

