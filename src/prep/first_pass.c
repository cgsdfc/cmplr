#include "first_pass.h"
#include "token.h"
#include <utillib/error.h>
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

int prep_first_pass_init(prep_first_pass_t *self, char *arg, utillib_vector *err)
{
  int r;
  if ((r=utillib_input_buf_init(&(self->fps_buf), arg, mode))!=0) {
    // file not existing
    return r;
  }
  self->fps_err=err;
  scanner_init(&(self->fps_scan), &(self->fps_buf),
      utillib_get_FILE_ft(), prep_first_pass_match(),
      NULL, SCANNER_MATCH_ANY_CHAR);
}

void prep_first_pass_destroy(prep_first_pass_t *self) {
  utillib_input_buf_destroy(&(self->fps_buf));
  scanner_destroy(&(self->fps_scan));
}

int prep_first_pass_getc(prep_first_pass_t *self) {
  static const char comment_replace = ' ';
  scanner_base_t *scan = &self->fps_scan;
  utillib_input_buf *buf = &self->fps_buf;
  while (true) {
    switch (scanner_yylex(scan)) {
      case SCANNER_ERROR:
        switch (scanner_get_val(scan)) {
          default:
          case PREP_COMMENT:
            utillib_vector_push_back(self->fps_err, 
                utillib_make_error(ERROR_LV_ERROR,
                  utillib_input_buf_current_pos(buf),
                  "unterminated or nested comment"));
            return PREP_ERROR_TOKEN;
        }
        break;
      case SCANNER_EOF:
        return PREP_EOF;
      case SCANNER_MATCHED:
        switch (scanner_get_val(scan)) {
          case PREP_ESNL: // delete 
            break;
          case PREP_COMMENT: // replace with ' '
            scanner_ungetchar(scan, comment_replace);
            break;
          default:
            assert(false);
        }
        break;
      case SCANNER_ANY_CHAR:
        return scanner_get_val(scan);
        break;
      default:
        assert(false);
    }
  }
}

int prep_first_pass_ungetc(prep_first_pass_t *self, int c) {
  return scanner_ungetchar(&(self->fps_scan), c);
}

utillib_char_buf_ft const * prep_first_pass_getft(void)
{
  static const utillib_char_buf_ft first_pass_ft={
    .cb_getc=(utillib_getc_func_t*) prep_first_pass_getc,
    .cb_ungetc=(utillib_ungetc_func_t*) prep_first_pass_ungetc,
  };
  return & first_pass_ft;
}
