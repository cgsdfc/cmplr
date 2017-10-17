#include "second_pass.h"
#include <ctype.h>
#include <scanner/match.h>
#include <utillib/error.h>
#include <utillib/input_buf.h>

static int prep_scanner_match_sharp(scanner_base_t *scan) {
  int c;
  if ((c = scanner_getchar(scan)) == '#') {
    return SCANNER_MATCHED;
  }
  scanner_ungetchar(scan, c);
  return SCANNER_UNMATCHED;
}

static int prep_isspace(int c) {
  // to skipped by prep_scanner_skip_space
  return isspace(c) && c != '\n';
}
static int prep_scanner_skip_space(scanner_base_t *scan) {
  int c;
  int cnt = 0;
  do {
    c = scanner_getchar(scan);
    cnt++;
  } while (prep_isspace(c));
  return cnt > 1 ? SCANNER_MATCHED : SCANNER_UNMATCHED;
}

static int prep_scanner_match_BOL(scanner_base_t *scan) {
  utillib_input_buf *buf = SCANNER_CHAR_BUF(scan);
  int c;
  if (utillib_input_buf_ftell(buf) == 0) {
    return SCANNER_MATCHED;
  }
  if ((c = scanner_getchar(scan)) == '\n') {
    return SCANNER_MATCHED;
  }
  scanner_ungetchar(scan, c);
  return SCANNER_UNMATCHED;
}

static scanner_str_entry_t const *prep_get_pdmacro(void) {
  SCANNER_STR_ENTRY_BEGIN(predefined_macro)
  SCANNER_STR_ENTRY_ELEM("__FILE__", PREP_FILE)
  SCANNER_STR_ENTRY_ELEM("__LINE__", PREP_LINE)
  SCANNER_STR_ENTRY_ELEM("__DATE__", PREP_DATE)
  SCANNER_STR_ENTRY_ELEM("__TIME__", PREP_TIME)
  SCANNER_STR_ENTRY_ELEM("__STDC__", PREP_STDC)
  SCANNER_STR_ENTRY_ELEM("__GNUC__", PREP_GNUC)
  SCANNER_STR_ENTRY_ELEM("__STDC_VERSION__", PREP_STDC_VERSION)
  SCANNER_STR_ENTRY_ELEM("__GNUC_MINOR__", PREP_GNUC_MINOR)
  SCANNER_STR_ENTRY_ELEM("__GNUG__", PREP_GNUG)
  SCANNER_STR_ENTRY_ELEM("__cplusplus", PREP_CPLUS_PLUS)
  SCANNER_STR_ENTRY_ELEM("__STRICT_ANSI_", PREP_STRICT_ANSI)
  SCANNER_STR_ENTRY_ELEM("__BASE_FILE__", PREP_BASE_FILE)
  SCANNER_STR_ENTRY_ELEM("__INCLUDE_LEVEL__", PREP_INCLUDE_LEVEL)
  SCANNER_STR_ENTRY_ELEM("__VERSION__", PREP_VERSION)
  SCANNER_STR_ENTRY_ELEM("__OPTIMIZE__", PREP_OPTIMIZE)
  SCANNER_STR_ENTRY_ELEM("__REGISTER_PREFIX__", PREP_REGISTER_PREFIX)
  SCANNER_STR_ENTRY_ELEM("__USER_LABEL_PREFIX__", PREP_USER_LABEL_PREFIX)
  SCANNER_STR_ENTRY_END(predefined_macro)
  return predefined_macro;
}

static scanner_match_entry_t const *prep_get_match_entry(void) {
  SCANNER_MATCH_ENTRY_BEGIN(second_pass_match)
  SCANNER_MATCH_ENTRY(prep_scanner_match_BOL, PREP_BOL)
  SCANNER_MATCH_ENTRY(scanner_match_identifier, PREP_ID)
  SCANNER_MATCH_ENTRY(scanner_match_string_double, PREP_STR_D)
  SCANNER_MATCH_ENTRY(scanner_match_string_angle, PREP_STR_A)
  SCANNER_MATCH_ENTRY(scanner_match_string_single, PREP_STR_S)
  SCANNER_MATCH_ENTRY(prep_scanner_match_sharp, PREP_CHAR_SHARP)
  SCANNER_MATCH_ENTRY(prep_scanner_skip_space, -1)
  SCANNER_MATCH_ENTRY_END(second_pass_match)
  return second_pass_match;
}

void prep_second_pass_init(prep_second_pass_t *self, char *input, utillib_vector *err) {
  int r;
  if ((r=prep_first_pass_init(&(self->psc_first), input, err))!=0) {
    utillib_die("prep_make_scanner: prep_first_pass_init failed");
  }
  scanner_init(&(self->psc_scan), &(self->psc_first),
      prep_first_pass_getft(),
      prep_get_match_entry(), prep_get_pdmacro(),
      SCANNER_MATCH_STR_AS_ID | SCANNER_MATCH_ANY_CHAR);
  self->psc_err=err;
}

void prep_second_pass_destroy(prep_second_pass_t *self)
{
  prep_first_pass_destroy(&(self->psc_first));
  scanner_destroy(&(self->psc_scan));
}

int prep_second_pass_yylex(prep_second_pass_t *self) {


}
