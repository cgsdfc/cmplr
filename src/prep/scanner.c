#define _BSD_SOURCE
#include "scanner.h"
#include "token.h"
#include <assert.h>
#include <scanner/match.h>
#include <string.h>
#include <utillib/error.h>

static int prep_getc(scanner_input_buf *self) {
  int c;
  if ((c = scanner_input_buf_getc(self)) == '\\') {
    if ((c = scanner_input_buf_getc(self)) == '\n') {
      // deleted
      return scanner_input_buf_getc(self);
    }
    scanner_input_buf_ungetc(self, c);
  }
  return c;
}

static int prep_scanner_match_sharp(scanner_base_t *scan) {
  int c;
  if (scanner_input_buf_begin_of_line(SCANNER_CHAR_BUF(scan))) {
    if ((c = scanner_getc(scan)) == '#') {
      return SCANNER_MATCHED;
    }
    scanner_ungetc(scan, c);
  }
  return SCANNER_UNMATCHED;
}

static scanner_match_entry_t const *get_match_entry(void) {
  SCANNER_MATCH_ENTRY_BEGIN(parser_match)
  SCANNER_MATCH_ENTRY(scanner_skip_c_comment, PREP_COMMENT)
  SCANNER_MATCH_ENTRY(scanner_skip_cpp_comment, PREP_COMMENT)
  SCANNER_MATCH_ENTRY(scanner_match_identifier, PREP_ID)
  SCANNER_MATCH_ENTRY(scanner_match_string_double, PREP_STR_D)
  SCANNER_MATCH_ENTRY(scanner_match_string_angle, PREP_STR_A)
  SCANNER_MATCH_ENTRY(scanner_match_string_single, PREP_STR_S)
  SCANNER_MATCH_ENTRY(prep_scanner_match_sharp, PREP_CHAR_SHARP)
  SCANNER_MATCH_ENTRY_END(parser_match)
  return parser_match;
}
static void push_error(prep_scanner_t *self, char const *msg) {
  utillib_vector_push_back(
      self->psp_err,
      scanner_input_buf_make_error(&(self->psp_buf), ERROR_LV_ERROR, msg));
}

void prep_scanner_init(prep_scanner_t *self, FILE *file, char const *filename,
                       utillib_vector *err) {
  scanner_input_buf_init(&(self->psp_buf), file, filename);
  scanner_base_init(&(self->psp_scan), &(self->psp_buf), prep_getc,
                    get_match_entry());
  self->psp_err = err;
}

int prep_scanner_yylex(prep_scanner_t *self) {
  int r;
  scanner_base_t *scan = &(self->psp_scan);
  scanner_input_buf *buf = &(self->psp_buf);
  switch (r = scanner_yylex(scan)) {
  case SCANNER_ERROR:
    switch (r = scanner_get_errc(scan)) {
    case PREP_STR_D:
      push_error(self, "missing '\"' terminator");
      return PREP_ERR;
    case PREP_STR_S:
      push_error(self, "missing '\'' terminator");
      return PREP_ERR;
    case PREP_STR_A:
      push_error(self, "missing '>' terminator");
      return PREP_ERR;
    case PREP_COMMENT:
      push_error(self, "nested or unterminated comment");
      return PREP_ERR;
    default:
      assert(false);
    }
  case SCANNER_EOF: // EOF
    return PREP_EOF;
  case SCANNER_MATCHED:
    return scanner_get_val(scan);
  default:
    assert(false);
  }
}

void prep_scanner_destroy(prep_scanner_t *self) {
  scanner_input_buf_destroy(&(self->psp_buf));
  scanner_base_destroy(&(self->psp_scan));
}

char const *prep_scanner_get_text(prep_scanner_t *self) {
  return strdup(scanner_get_text(&(self->psp_scan)));
}
