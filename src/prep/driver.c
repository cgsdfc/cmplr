#include "first_pass.h"
#include <utillib/cli.h>

int main(int argc, char **argv) {
  FILE *input = utillib_cli_single_file(argc, argv, "r");
  prep_first_pass_t first_pass;
  prep_first_pass_init(&first_pass, input, stdout);
  prep_first_pass_scan(&first_pass);
}
