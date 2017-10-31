/*
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/
#define _BSD_SOURCE
#include "parser.h"
#include "arg.h"
#include "directive.h"
#include "token.h"
#include <ctype.h>
#include <errno.h>
#include <scanner/input_buf.h>
#include <scanner/match.h>
#include <string.h>
#include <utillib/error.h>

static void push_error(prep_parser_t * self, int lv, char const *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	utillib_vector_push_back(&self->psp_errors,
				 scanner_input_buf_make_errorV(&self->
							       psp_cur_scan->
							       psp_buf, lv,
							       fmt, ap));
	va_end(ap);
}

/**
 * \function get_directives
 * Returns an array of string-int pair corresponding to
 * the preprocessor's directives.
 * \param void.
 * \return A pointer to the array.
 */

static scanner_str_entry_t const *get_directives(void)
{
	SCANNER_STR_ENTRY_BEGIN(directive)
	    SCANNER_STR_ENTRY_ELEM("define", DR_DEFINE)
	    SCANNER_STR_ENTRY_ELEM("undef", DR_UNDEF)
	    SCANNER_STR_ENTRY_ELEM("if", DR_IF)
	    SCANNER_STR_ENTRY_ELEM("ifndef", DR_IFNDEF)
	    SCANNER_STR_ENTRY_ELEM("defined", DR_DEFINED)
	    SCANNER_STR_ENTRY_ELEM("error", DR_ERROR)
	    SCANNER_STR_ENTRY_ELEM("line", DR_LINE)
	    SCANNER_STR_ENTRY_ELEM("include", DR_INCLUDE)
	    SCANNER_STR_ENTRY_ELEM("ifdef", DR_IFDEF)
	    SCANNER_STR_ENTRY_ELEM("assert", DR_ASSERT)
	    SCANNER_STR_ENTRY_ELEM("cpu", DR_CPU)
	    SCANNER_STR_ENTRY_ELEM("elif", DR_ELIF)
	    SCANNER_STR_ENTRY_ELEM("else", DR_ELSE)
	    SCANNER_STR_ENTRY_ELEM("include_next", DR_INCLUDE_NEXT)
	    SCANNER_STR_ENTRY_ELEM("pragma", DR_PRAGMA)
	    SCANNER_STR_ENTRY_ELEM("system", DR_SYSTEM)
	    SCANNER_STR_ENTRY_ELEM("unassert", DR_UNASSERT)
	    SCANNER_STR_ENTRY_ELEM("warning", DR_WARNING)
	    SCANNER_STR_ENTRY_END(directive)
}

/**
 * \function get_pdmacro
 * Returns an array of string-int pair corresponding to
 * the preprocessor's predefined macros.
 * \param void.
 * \return A pointer to the array.
 */

static scanner_str_entry_t const *get_pdmacro(void)
{
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
	    SCANNER_STR_ENTRY_ELEM("__REGISTER_PREFIX__",
				   PREP_REGISTER_PREFIX)
	    SCANNER_STR_ENTRY_ELEM("__USER_LABEL_PREFIX__",
				   PREP_USER_LABEL_PREFIX)
	    SCANNER_STR_ENTRY_END(predefined_macro)
	    return predefined_macro;
}

/**
 * \function make_scanner
 * Creates and initilizes a scanner on the heap.
 * \param file the opened file.
 * \param filename the name of the file.
 * \return the newly created scanner.
 */

static prep_scanner_t *make_scanner(FILE * file, char const *filename,
				    utillib_vector * err)
{
	prep_scanner_t *scan = malloc(sizeof *scan);
	prep_scanner_init(scan, file, filename, err);
	return scan;
}

/**
 * \function prep_parser_init
 * Initializes the preprocessor parser.
 * \param self.
 * \param file The base file passed in by -Ifile to preprocess.
 * \param filename The cannonicialized name of the file.
 * \return void.
 */

void prep_parser_init(prep_parser_t * self, prep_arg_t * arg)
{
	utillib_vector_init(&self->psp_errors);
	self->psp_cur_scan =
	    make_scanner(arg->arg_file, arg->arg_filename,
			 &self->psp_errors);
	utillib_unordered_map_ft func =
	    utillib_unordered_map_const_charp_ft();
	utillib_slist_init(&self->psp_incl_stack);

	utillib_unordered_map_init_from_array(&self->psp_macro_map, func,
					      get_pdmacro());
	utillib_unordered_map_init_from_array(&self->psp_directives, func,
					      get_directives());
	UTILLIB_VECTOR_FOREACH(char const *, macro, &arg->arg_defs) {
		utillib_unordered_map_emplace(&self->psp_macro_map, macro,
					      "1");
	}
}

/**
 * \function destroy_include_stack
 * Destroys the slist for include stack and all the preprocessor scanners
 * in it.
 * \param inc the include stack.
 * \return void.
 */

static void destroy_include_stack(utillib_slist * inc)
{
	UTILLIB_SLIST_FOREACH(prep_scanner_t *, scan, inc) {
		prep_scanner_destroy(scan);
	}
	utillib_slist_destroy(inc);
}

/**
 * \function prep_parser_destroy
 * Destroys preprocessor parser, free up memory.
 * \param self.
 * \return void.
 */

void prep_parser_destroy(prep_parser_t * self)
{
	utillib_unordered_map_destroy(&self->psp_macro_map);
	utillib_unordered_map_destroy(&self->psp_directives);
	destroy_include_stack(&self->psp_incl_stack);
}

/**
 * \function push_scanner
 * Postpone the scanning of the current file by pushing current scanner
 * onto the include stack. Then set the current scanner to scan.
 * \param self.
 * \param scan The scanner just been created.
 * \return void
 */

static void push_scanner(prep_parser_t * self, prep_scanner_t * scan)
{
	utillib_slist_push_front(&self->psp_incl_stack,
				 self->psp_cur_scan);
	self->psp_cur_scan = scan;
}

/**
 * \function pop_scanner
 * Finalizes the scanning of the current file and restores
 * scanning of the scanner on the top of the include stack.
 * \param self
 * \return void
 */

static void pop_scanner(prep_parser_t * self)
{
	prep_scanner_destroy(self->psp_cur_scan);
	self->psp_cur_scan = utillib_slist_front(&self->psp_incl_stack);
	utillib_slist_pop_front(&self->psp_incl_stack);
}

/**
 * \function do_include
 * Called after `#include' was seen.
 * Parses the line and searches for a correct file to
 * process by switching a scanning context.
 * \param self
 * \return 0 on success, PREP_ERR if failed and
 * an error would have been pushed.
 */

static int do_include(prep_parser_t * self)
{
	int token_code;
	int lv = ERROR_LV_OK;
	char const *filename;
	FILE *file;
	switch (token_code = prep_scanner_yylex(self->psp_cur_scan)) {
	case PREP_STR_D:
		// "file"

		break;
	case PREP_STR_A:
		// <file>
		break;
	case PREP_ID:
		// ID
		break;
	default:
		lv = ERROR_LV_ERROR;
		push_error(self, lv,
			   "#include expects \"FILENAME\" or <FILENAME>");
		return lv;
	}
	file = fopen(filename, "r");
	if (!file) {
		lv = ERROR_LV_FATAL;
		push_error(self, lv, strerror(errno));
		return lv;
	}
	push_scanner(self,
		     make_scanner(file, filename, &self->psp_errors));
	return lv;
}

/**
 * \function find_directive
 * Looks up the following token in directive map
 * to find its code.
 * \param self.
 * \return if the token is an identifier and recognized
 * as a known directive, returns its code; or else returns
 * PREP_ERR.
 */

static size_t find_directive(prep_parser_t * self)
{
	int token_code = prep_scanner_yylex(self->psp_cur_scan);
	const char *text;
	utillib_pair_t *pair;
	if (token_code != PREP_ID) {
		return PREP_ERR;
	}
	text = prep_scanner_get_text(self->psp_cur_scan);
	if (!
	    (pair =
	     utillib_unordered_map_find(&self->psp_directives, text))) {
		return PREP_ERR;
	}
	return (size_t) UTILLIB_PAIR_SECOND(pair);
}

/** \function do_unknown
 * Handles case when the identifier after `#' is not
 * a valid directive.
 * \param self.
 * \return void.
 */

static void do_unknown(prep_parser_t * self)
{
	char const *unknown = prep_scanner_get_text(self->psp_cur_scan);
	int ch;
	push_error(self, ERROR_LV_WARNING,
		   "ignoring unknown directive `%s'", unknown);
	do {
		ch = prep_scanner_yylex(self->psp_cur_scan);
	} while (ch != '\n' || ch != PREP_EOF);
}

static int do_define(prep_parser_t * self)
{
	int token_code = prep_scanner_yylex(self->psp_cur_scan);
	int lv = ERROR_LV_OK;
	char const *macro;
	prep_macro_entry_t *entry;
	if (token_code != PREP_ID) {
		lv = ERROR_LV_ERROR;
		push_error(self, lv, "macro names must be identifiers");
		return lv;
	}
	macro = strdup(prep_scanner_get_text(self->psp_cur_scan));
	// do a lookahead for the ' ' or '('
	token_code = prep_scanner_yylex(self->psp_cur_scan);
}

static void do_undef(prep_parser_t * self)
{
	switch (prep_scanner_yylex(self->psp_cur_scan)) {
	case PREP_ID:
		utillib_unordered_map_erase(&self->psp_macro_map,
					    prep_scanner_get_text(self->
								  psp_cur_scan));
		break;
	case PREP_ERR:
		break;
	default:
		push_error(self, ERROR_LV_ERROR,
			   "expected identifier after `#undef' directive");
		break;
	}
}

/**
 * \function do_error
 * pushes a fatal error with user_msg.
 * \param self.
 * \param user_msg The `#error "..."' parsed from source code.
 * \return void.
 */

static void do_display(prep_parser_t * self, int lv, char const *errmsg)
{
	char const *user_msg;
	static const char *incomplete = "<incomplete string>";
	switch (prep_scanner_yylex(self->psp_cur_scan)) {
	case PREP_ERR:
		user_msg = incomplete;
		// scanner has push an error;
		break;
	case PREP_STR_D:
		user_msg = prep_scanner_get_text(self->psp_cur_scan);
		break;
	default:
		// other illegal token.
		user_msg = incomplete;
		push_error(self, ERROR_LV_ERROR,
			   "expected `\"...\"' after `%s'", errmsg);
		break;
	}
	push_error(self, lv, user_msg);
}

static void do_error(prep_parser_t * self)
{
	do_display(self, ERROR_LV_FATAL, "#error");
}

static void do_warning(prep_parser_t * self)
{
	do_display(self, ERROR_LV_WARNING, "#warning");
}

/**
 * \function prep_parser_analyze
 * Analyzes the source code and do preprocessing.
 * \param self.
 * \return if a fatal error occurred during analyzing,
 * it returns ERROR_LV_FATAL immediately.
 * In other cases, it does every possible recovery and
 * returns ERROR_LV_OK.
 */

int prep_parser_analyze(prep_parser_t * self)
{
	int token_code;
	int lv;
	while (true) {
		switch (token_code =
			prep_scanner_yylex(self->psp_cur_scan)) {
		case PREP_EOF:
			if (utillib_slist_size(&self->psp_incl_stack)) {
				pop_scanner(self);
				continue;
			}	// the final scanner hit EOF
			prep_scanner_destroy(self->psp_cur_scan);
			return ERROR_LV_OK;
		case PREP_CHAR_SHARP:
			switch (find_directive(self)) {
			default:
				do_unknown(self);
				break;
			case DR_ERROR:
				do_error(self);
				return ERROR_LV_FATAL;
			case DR_WARNING:
				do_warning(self);
				break;
			case DR_INCLUDE:
				if (ERROR_LV_FATAL ==
				    (lv = do_include(self))) {
					return lv;
				}
				break;
			case DR_DEFINE:
				do_define(self);
				break;
			case DR_UNDEF:
				do_undef(self);
				break;
			}
		}
	}
}
