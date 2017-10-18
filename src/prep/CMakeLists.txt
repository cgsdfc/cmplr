project(prep C)
set(prep_source 
  ./parser.c
  ./scanner.c
)

add_library(prep
  ${prep_source}
)

target_link_libraries(prep
  scanner
  utillib
)

add_executable(pp
  ./driver.c
)
target_link_libraries(pp
  prep
  )

