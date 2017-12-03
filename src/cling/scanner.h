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
#ifndef CLING_SCANNER_H
#define CLING_SCANNER_H
#include <stdio.h> // FILE*
#include <utillib/enum.h>
#include <utillib/scanner.h>

UTILLIB_ENUM_BEGIN(cling_scanner_error_kind)
UTILLIB_ENUM_ELEM_INIT(CL_EBADNEQ, 1)
UTILLIB_ENUM_ELEM(CL_EUNTCHAR)
UTILLIB_ENUM_ELEM(CL_EUNTSTR)
UTILLIB_ENUM_ELEM(CL_ESTRCHAR)
UTILLIB_ENUM_ELEM(CL_ECHRCHAR)
UTILLIB_ENUM_ELEM(CL_ELEADZERO)
UTILLIB_ENUM_ELEM(CL_EUNKNOWN)
UTILLIB_ENUM_END(cling_scanner_error_kind)

/* ＜字符串＞ ::=  "｛十进制编码为32,33,35-126的ASCII字符｝" */
/* ＜无符号整数＞  ::= ＜非零数字＞｛＜数字＞｝ */
/* ＜整数＞    ::= ［＋｜－］＜无符号整数＞｜０ */
/* ＜字符＞  ::= '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞' */

void cling_scanner_init(struct utillib_token_scanner *self, FILE *file);
void cling_scanner_destroy(struct utillib_token_scanner *self);
extern bool cling_scanner_signed_number;

#endif /* CLING_SCANNER_H */
