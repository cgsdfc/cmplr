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
#include <utillib/scanner.h>
#include <stdio.h>
struct option;
struct rd_parser;

struct scanner 
{
        struct utillib_char_scanner input;
        struct utillib_string buffer;
        struct rd_parser *parser;
        struct option const *option;
        size_t context;
        size_t lookahead;
};

/* ＜字符串＞ ::=  "｛十进制编码为32,33,35-126的ASCII字符｝" */
/* ＜无符号整数＞  ::= ＜非零数字＞｛＜数字＞｝ */
/* ＜整数＞    ::= ［＋｜－］＜无符号整数＞｜０ */
/* ＜字符＞  ::= '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞' */

void scanner_init(struct scanner *self,
                struct option const *option,
                struct rd_parser *parser, FILE *file);
void scanner_destroy(struct scanner *self);
size_t scanner_lookahead(struct scanner const *self);
void scanner_shiftaway(struct scanner *self);
char const *scanner_semantic(struct scanner const *self);
void scanner_context(struct scanner *self, size_t context);

#endif /* CLING_SCANNER_H */
