#ifndef PREP_TOKEN_H
#define PREP_TOKEN_H

#include "utillib/enum.h"

UTILLIB_ENUM_BEGIN(prep_token_t)
UTILLIB_ENUM_ELEM(PREP_CHAR_SHARP)
UTILLIB_ENUM_ELEM(PREP_CHAR_COMMA)
UTILLIB_ENUM_ELEM(PREP_CHAR_LP)
UTILLIB_ENUM_ELEM(PREP_CHAR_RP)
UTILLIB_ENUM_ELEM(PREP_BOL)
UTILLIB_ENUM_ELEM(PREP_COMMENT)
UTILLIB_ENUM_ELEM(PREP_ESNL)
UTILLIB_ENUM_ELEM(PREP_ID)
UTILLIB_ENUM_ELEM(PREP_STR_A)
UTILLIB_ENUM_ELEM(PREP_STR_S)
UTILLIB_ENUM_ELEM(PREP_STR_D)
UTILLIB_ENUM_ELEM(PREP_FILE)
UTILLIB_ENUM_ELEM(PREP_LINE)
UTILLIB_ENUM_ELEM(PREP_DATE)
UTILLIB_ENUM_ELEM(PREP_TIME)
UTILLIB_ENUM_ELEM(PREP_STDC)
UTILLIB_ENUM_ELEM(PREP_STDC_VERSION)
UTILLIB_ENUM_ELEM(PREP_GNUC)
UTILLIB_ENUM_ELEM(PREP_GNUC_MINOR)
UTILLIB_ENUM_ELEM(PREP_GNUG)
UTILLIB_ENUM_ELEM(PREP_CPLUS_PLUS)
UTILLIB_ENUM_ELEM(PREP_STRICT_ANSI)
UTILLIB_ENUM_ELEM(PREP_BASE_FILE)
UTILLIB_ENUM_ELEM(PREP_INCLUDE_LEVEL)
UTILLIB_ENUM_ELEM(PREP_VERSION)
UTILLIB_ENUM_ELEM(PREP_OPTIMIZE)
UTILLIB_ENUM_ELEM(PREP_REGISTER_PREFIX)
UTILLIB_ENUM_ELEM(PREP_USER_LABEL_PREFIX)

UTILLIB_ENUM_END(prep_token_t)

#endif // PREP_TOKEN_H