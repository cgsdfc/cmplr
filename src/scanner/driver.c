#define _GNU_SOURCE
#include "match.h"
#include "scanner.h"
#include <utillib/error.h>
#include <utillib/input_buf.h>
#include <stdio.h>

static const scanner_match_entry_t main_match[] = {
    SCANNER_MATCH_ENTRY(scanner_match_identifier, 1)
        SCANNER_MATCH_ENTRY(scanner_match_string_double, 2) NULL,
};
int main() {
  /* utillib_input_buf buf; */
  char s[] = "\"aaaaaaaaaaaa\"";
  FILE *buf = fmemopen(s, sizeof s, "r");
  scanner_base_t scan;
  /* utillib_input_buf_init(&buf, INPUT_BUF_STRING, "integer "); */
  scanner_init(&scan, buf,
               /* utillib_input_buf_ft(), */
               utillib_get_FILE_ft(), main_match, NULL, SCANNER_MATCH_ANY_CHAR);
  int ch;
  while (true) {
    switch (scanner_yylex(&scan)) {
    case SCANNER_EOF:
      return 0;
    case SCANNER_MATCHED:
      puts(scanner_get_text(&scan));
      printf("scanner_get_val=%d\n", scanner_get_val(&scan));
      break;
    case SCANNER_ERROR:
      ch = scanner_get_val(&scan);
      /* utillib_input_buf_perror(&buf, */
      /*     stderr, ERROR_LV_FATAL); */
      fprintf(stderr, "unrecognized char `%c'\n", ch);
      return 1;
    }
  }
}
