set(CMAKE_C_FLAGS "-g3")
include_directories(${CMAKE_SOURCE_DIR})
set(UTILLIB_SOURCE
  ./input_buf.c
  ./enum.c
  ./slist.c
  ./vector.c
  ./string.c
)
add_library(utillib
  ${UTILLIB_SOURCE}
)

add_executable(utillib_test
  ./driver.c
)
target_link_libraries(utillib_test
  utillib
)
