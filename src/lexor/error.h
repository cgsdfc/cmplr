#ifndef TKNZR_ERROR_H
#define TKNZR_ERROR_H 1
#include "utillib/enum.h"

typedef enum cmplr_error {
	ER_NO_INPUT = 1,
	ER_BAD_TOKEN,
	ER_NOMEM,
	ER_COMMENT_UNTERMINATED,
	ER_DIGIT_NOT_OCTONARY,
	ER_DIGIT_NOT_HEXADECIMAL,
	ER_BAD_INTEGER_SUFFIX,
	ER_INCOMPLETED_FLOAT,
	ER_BAD_FLOAT_SUFFIX,
	ER_STRING_UNTERMINATED,
	ER_FLOAT_EXP_NO_DIGIT,
	ER_CHAR_UNTERMINATED,
	ER_EMPTY_CHAR_LITERAL,
	ER_INVALID_CHAR,
} cmplr_error;

typedef enum error_level {
	LV_WARNING,
	LV_ERROR,
	LV_FATAL,
	LV_NOTE,
} error_level;

typedef struct err_entry {
	char *errmsg;
	int lv;
} err_entry;

struct char_buffer;
void tknzr_error_set(int err);
int tknzr_error_handle(struct char_buffer *);

#endif
