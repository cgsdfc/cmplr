#include "first_pass.h"
#include "scanner/match.h"
#include "scanner/scanner.h"
#include "utillib/error.h"
#include <assert.h>

static int prep_delete_escape_newline(scanner_base_t *scan) {
  int c;
  if ((c = scanner_getchar(scan)) == '\\') {
    if ((c = scanner_getchar(scan)) == '\n') {
      return SCANNER_MATCHED;
    }
    scanner_ungetchar(scan, c);
  }
  scanner_ungetchar(scan, c);
  return SCANNER_UNMATCHED;
}
static scanner_match_entry_t const *prep_first_pass_match(void) {
  SCANNER_MATCH_ENTRY_BEGIN(first_pass_match)
  SCANNER_MATCH_ENTRY(scanner_skip_c_comment, PREP_COMMENT)
  SCANNER_MATCH_ENTRY(scanner_skip_cpp_comment, PREP_COMMENT)
  SCANNER_MATCH_ENTRY(prep_delete_escape_newline, PREP_ESNL)
  SCANNER_MATCH_ENTRY_END(first_pass_match)
  return first_pass_match;
}
int prep_first_pass_init(prep_first_pass_t *self, FILE *input, FILE *output) {
  int r;
  r = scanner_init(&(self->fps_scan), input, utillib_get_FILE_ft(),
                   prep_first_pass_match(),
                   NULL, // str_tab
                   SCANNER_MATCH_ANY_CHAR);
  if (r != 0) {
    return -1;
  }
  self->fps_out = output;
  return 0;
}

void prep_first_pass_destroy(prep_first_pass_t *self) {
  fclose(self->fps_out);
  scanner_destroy(&(self->fps_scan));
}

int prep_first_pass_scan(prep_first_pass_t *self) {
  static const char *comment_replace = "<comment>";
  int r;
  scanner_base_t *scan = &self->fps_scan;
  while (true) {
    switch (scanner_yylex(scan)) {
    case SCANNER_ERROR:
      switch (scanner_get_val(scan)) {
      default:
      case PREP_COMMENT:
        prep_first_pass_destroy(self);
        utillib_die("unterminated or nested comment");
      }
      break;
    case SCANNER_EOF:
      return 0;
    case SCANNER_MATCHED:
      switch (scanner_get_val(scan)) {
      case PREP_ESNL:
        break;
      case PREP_COMMENT:
        fputs(comment_replace, self->fps_out);
        break;
      default:
        break;
      }
      break;
    case SCANNER_ANY_CHAR:
      fputc(scanner_get_val(scan), self->fps_out);
      break;
    default:
      assert(false);
    }
  }
}
