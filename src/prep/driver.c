#include "scanner.h"

int main(int argc, char **argv) {
  prep_scanner_t scan;
  FILE *file = fopen("./t.c", "r");
  prep_scanner_init(&scan, file, "./t.c", NULL);
  scanner_read_all(&scan.psp_scan, stdout);
  prep_scanner_destroy(&scan);
}
