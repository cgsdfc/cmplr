#ifndef SCANNER_MATCH_H
#define SCANNER_MATCH_H
/* provides routines to match the charachers
 * against tokens.
 * on successful match, they return SCANNER_MATCHED.
 * on unmatch, they return SCANNER_UNMATCHED.
 * on error when matching, i.e. unterminated
 * string or comment, they return SCANNER_ERROR.
 */
#include "scanner.h"
typedef utillib_pair_t scanner_str_entry_t;
int scanner_match_any_char(scanner_base_t *);
int scanner_match_identifier(scanner_base_t *);
int scanner_match_string_double(scanner_base_t *);
int scanner_match_string_single(scanner_base_t *);
int scanner_match_string_angle(scanner_base_t *);
int scanner_skip_cpp_comment(scanner_base_t *);
int scanner_skip_c_comment(scanner_base_t *);
int scanner_skip_space(scanner_input_buf *);
scanner_str_entry_t const *scanner_search_string(scanner_str_entry_t const *,
                                                 char const *);
#endif // SCANNER_MATCH_H
