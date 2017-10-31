#include "action.h"
#include <scanner/input_buf.h>
#include <scanner/scanner.h>

void prep_act_parser_push_error(prep_parser_t * self, int lv,
				char const *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	utillib_error_push_vectorV(&(self->psp_errors), lv,
				   scanner_input_buf_current_pos(&
								 (self->
								  psp_cur_scan->
								  psp_buf)),
				   msg, ap);
}

int prep_act_do_define(prep_parser_t * self)
{
	int r = prep_scanner_yylex(self->psp_cur_scan);
	if (r != PREP_ID) {
		push_back_error(self, ERROR_LV_ERROR,
				"following `# define' is something other than identifier");
		return;
	}
	char const *key = prep_scanner_get_text(self->psp_cur_scan);
	utillib_pair_t *pair = find_key(self, key);
	if (pair) {
		push_back_error(self, ERROR_LV_WARNING,
				"macro `%s' was redefined",
				(char *) UTILLIB_PAIR_FIRST(pair));
	}
	// val is simplified to str
	utillib_string *str = utillib_make_string();
	while ((r = prep_scanner_yylex(self->psp_cur_scan)) != '\n') {
		switch (r) {
		case PREP_ID:
			utillib_string_append(str,
					      prep_scanner_get_text(self->
								    psp_cur_scan));
			break;
		default:
			utillib_string_append_char(str,
						   prep_scanner_get_val
						   (self->psp_cur_scan));
		}
	}
	insert_pair(self, key, str);
}

int prep_act_do_undef(prep_parser_t * self)
{
}

int prep_parser_parse(prep_parser_t * self)
{
	int directive;
	while (true) {
		switch (directive) {
		case DR_DEFINE:
			do_define(self);
			break;
		}
	}
