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
#include "scanner.h"
#include "token.h"
#include <assert.h>
#include <scanner/match.h>
#include <string.h>
#include <utillib/error.h>
#include <utillib/logging.h>

/**
 * \file prep/scanner.c
 * \breif Scanner implementation of the preprocessor.
 * Based on libscanner, plugin matcher functions of preprocessor.
 */

/**
 * \function prep_getc
 * Get char from scanner_input_buf while deleting
 * backslash newline sequence.
 *
 * \param self The scanner_input_buf.
 * \return A char or EOF.
 */

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

/** \function prep_scanner_match_sharp
 * Matches a `#' character at the beginning of a line.
 *
 * \para scan the scanner that does the matching.
 * \return SCANNER_MATCHED if matched else SCANNER_UNMATCHED.
 */

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

/**
 * \function get_match_entry
 * Returns the matcher function array of the preprocessor.
 * \param void.
 * \return Pointer to const static matcher array.
 */

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

/**
 * \function push_error
 * Push an Error into the error stack kept by self.
 * \param self The preprocessor scanner.
 * \param msg An error message.
 * \return void
 */

static void push_error(prep_scanner_t *self, char const *msg) {
  utillib_vector_push_back(
      self->psp_err,
      scanner_input_buf_make_error(&self->psp_buf, ERROR_LV_ERROR, msg));
}

/** \function prep_scanner_init
 * Initializes a preprocessor scanner.
 * \param self The preprocessor scanner.
 * \param file The stream to read from.
 * \param filename The name of the opened file.
 * \return void.
 */

void prep_scanner_init(prep_scanner_t *self, FILE *file, char const *filename,
                       utillib_vector *err) {
  scanner_input_buf_init(&self->psp_buf, file, filename);
  scanner_base_init(&self->psp_scan, &self->psp_buf, prep_getc,
                    get_match_entry());
  self->psp_err = err;
}

/** \function prep_scanner_yylex
 * Wraps the scanner_yylex and add error handling.
 * \param self The preprocessor scanner.
 * \return Code for token or a character.
 */

int prep_scanner_yylex(prep_scanner_t *self) {
  int r;
  scanner_base_t *scan = &self->psp_scan;
  scanner_input_buf *buf = &self->psp_buf;
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
      UTILLIB_UNREACHABLE("prep_scanner_yylex:");
    }
  case SCANNER_EOF: // EOF
    UTILLIB_LOG(DEBUG, "End of File hit");
    return PREP_EOF;
  case SCANNER_MATCHED:
    UTILLIB_LOG(DEBUG, "Maches");
    return scanner_get_val(scan);
  default:
    UTILLIB_UNREACHABLE("prep_scanner_yylex:");
  }
}

/**
 * \function prep_scanner_destroy
 * Close the stream and free up memory.
 * \param prep_scanner_t
 * \return void
 */

void prep_scanner_destroy(prep_scanner_t *self) {
  scanner_input_buf_destroy(&self->psp_buf);
  scanner_base_destroy(&self->psp_scan);
}

/**
 * \function prep_scanner_get_text
 * Back up the string returned by scanner_get_text.
 * \param prep_scanner_t.
 * \return the string of the token.
 */

char const *prep_scanner_get_text(prep_scanner_t *self) {
  return scanner_get_text(&self->psp_scan);
}
