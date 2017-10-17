#include "cli.h"
#include "error.h"
#include <errno.h>
#include <stdlib.h>

FILE *utillib_cli_single_file(int argc, char **argv, char const *mode) {
  // handle common single file as the only argument case
  // this should be called at the very beginning the program
  // because if it failed, it terminates the program immediately
  // by calling utillib_die.
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <FILE>\n", argv[0]);
    utillib_die("No input file specified");
  }
  FILE *file = fopen(argv[1], mode);
  if (file) {
    return file;
  }
  int errno_sv = errno;
  perror(argv[0]);
  exit(errno_sv);
}
