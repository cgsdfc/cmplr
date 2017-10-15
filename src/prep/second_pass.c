#include "second_pass.h"
#include <ctype.h>
#include <utillib/error.h>
#include <utillib/input_buf.h>
#include <scanner/match.h>

static int prep_scanner_match_sharp(scanner_base_t *scan)
{
  int c;
  if ((c=scanner_getchar(scan)) == '#') {
    return SCANNER_MATCHED;
  }
  scanner_ungetchar(scan, c);
  return SCANNER_UNMATCHED;
}

static int prep_isspace(int c)
{
  // to skipped by prep_scanner_skip_space
  return isspace(c) && c!='\n';
}
static int prep_scanner_skip_space(scanner_base_t *scan)
{
  int c;
  int cnt=0;
  do { c=scanner_getchar(scan);cnt++; }
  while (prep_isspace(c));
  return cnt>1?SCANNER_MATCHED:SCANNER_UNMATCHED;
}

static int prep_scanner_match_BOL(scanner_base_t *scan)
{
  utillib_input_buf * buf=SCANNER_CHAR_BUF(scan);
  int c;
  if (utillib_input_buf_ftell(buf)==0) {
    return SCANNER_MATCHED;
  }
  if ((c=scanner_getchar(scan)) == '\n') {
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

static scanner_match_entry_t const * prep_get_match_entry(void)
{
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

static scanner_base_t * prep_make_scanner(char *input)
{
  static const char * last_words="prep_make_scanner failed";
  utillib_input_buf *buf;
  scanner_base_t *scan;
  buf = malloc(sizeof *buf);
  if (!buf) {
    utillib_die(last_words);
  }
  if (0 != utillib_input_buf_init(buf, input, INPUT_BUF_FILE)) {
    free(buf);
    utillib_die(last_words);
  }
  scan=malloc(sizeof *scan);
  if (!scan) {
    free(buf);
    utillib_die(last_words);
  }
  if (0 !=  scanner_init(scan, buf, utillib_input_buf_getft(),
        prep_get_match_entry(), prep_get_pdmacro(),
        SCANNER_MATCH_STR_AS_ID | SCANNER_MATCH_ANY_CHAR))
  {
    free(buf); free(scan);
    utillib_die(last_words);
  }
  return scan;
}

static void prep_destroy_scanner(scanner_base_t *scan)
{
  utillib_input_buf * buf=SCANNER_CHAR_BUF(scan);
  utillib_input_buf_destroy(buf);
  free(buf);
  scanner_destroy(scan);
  free(scan);
}

int prep_second_pass_init(prep_second_pass_t *self, char *input) {
  scanner_base_t * scan=prep_make_scanner(input);
  self->cur_scan=scan;
  utillib_slist_init(&(self->incl_stack));
  utillib_slist_push_front(&(self->incl_stack), scan);
  utillib_unordered_map_init(&(self->macro_map), utillib_unordered_map_const_charp_ft());
}


void prep_second_pass_destroy(prep_second_pass_t *self) {
  utillib_unordered_map_destroy(&(self->macro_map));
  UTILLIB_SLIST_FOREACH(scanner_base_t*, scan, &(self->incl_stack)) {
    prep_destroy_scanner(scan);
  }
  utillib_slist_destroy(&(self->incl_stack));
}

static prep_second_pass_t * prep_static_instance(void) {
  static prep_second_pass_t x;
  return &x;
}

static void prep_second_pass_cleanup(void)
{
  prep_second_pass_destroy(prep_static_instance());
}

prep_second_pass_t * prep_static_init(char * input)
{
  prep_second_pass_t * self=prep_static_instance();
  prep_second_pass_init(self, input);
  utillib_error_cleanup_func(prep_second_pass_cleanup);
  return self;
}

