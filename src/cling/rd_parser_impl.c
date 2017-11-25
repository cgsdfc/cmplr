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
#include "rd_parser_impl.h"
#include "config.h"

#ifndef WITH_WINDOWS
UTILLIB_ETAB_BEGIN(cling_rd_parser_error_kind)

UTILLIB_ETAB_ELEM_INIT( ,"数字以0开头")
UTILLIB_ETAB_ELEM_INIT( ,  "数字以字母结尾")
UTILLIB_ETAB_ELEM_INIT( ,  "数字太大")
UTILLIB_ETAB_ELEM_INIT( ,  "Char类型变量字符非法")
UTILLIB_ETAB_ELEM_INIT( ,  "Char类型中的字符有多个")
UTILLIB_ETAB_ELEM_INIT( ,  "字符串中的字符非法")
UTILLIB_ETAB_ELEM_INIT( ,  "!非法使用")
UTILLIB_ETAB_ELEM_INIT( ,  "非法字符")
UTILLIB_ETAB_ELEM_INIT( ,  "源程序不完整")
UTILLIB_ETAB_ELEM_INIT( ,  "在char类型中变量为空")
UTILLIB_ETAB_ELEM_INIT( ,  "标识符非法")
UTILLIB_ETAB_ELEM_INIT( ,  "表达式类型不匹配")
UTILLIB_ETAB_ELEM_INIT( ,  "标识符或变量后应为=")
UTILLIB_ETAB_ELEM_INIT( ,  "int，char后应为标识符")
UTILLIB_ETAB_ELEM_INIT( ,  "Const后应为int或char")
UTILLIB_ETAB_ELEM_INIT( ,  "缺少分号’;’")
UTILLIB_ETAB_ELEM_INIT( ,  "0前面出现符号")
UTILLIB_ETAB_ELEM_INIT( ,  "缺少’)’")
UTILLIB_ETAB_ELEM_INIT( ,  "缺少’]’")
UTILLIB_ETAB_ELEM_INIT( ,  "缺少’}’")
UTILLIB_ETAB_ELEM_INIT( ,  "数组长度应为无符号整数")
UTILLIB_ETAB_ELEM_INIT( ,  "Main函数后有多余字符或定义")
UTILLIB_ETAB_ELEM_INIT( ,  "参数表定义出错")
UTILLIB_ETAB_ELEM_INIT( ,  "常量初始化出错")
UTILLIB_ETAB_ELEM_INIT( ,  "函数体定义缺少{")
UTILLIB_ETAB_ELEM_INIT( ,  "条件语句缺少(")
UTILLIB_ETAB_ELEM_INIT( ,  "将char值赋值给int类型标识符")
UTILLIB_ETAB_ELEM_INIT( ,  "Switch语句错误")
UTILLIB_ETAB_ELEM_INIT( ,  "不规范标识符")
UTILLIB_ETAB_ELEM_INIT( ,  "读写语句、返回语句格式错误，缺少’(’")
UTILLIB_ETAB_ELEM_INIT( ,  "读写语句的’")
UTILLIB_ETAB_ELEM_INIT( ,  "不可识别语句")
UTILLIB_ETAB_ELEM_INIT( ,  "未定义标识符")
UTILLIB_ETAB_ELEM_INIT( ,  "不规范的标示符类型")
UTILLIB_ETAB_ELEM_INIT( ,  "函数调用的参数个数或类型不匹配")
UTILLIB_ETAB_ELEM_INIT( ,  "重复定义标识符")
UTILLIB_ETAB_ELEM_INIT( ,  "无返回值函数不能作为因子")
UTILLIB_ETAB_ELEM_INIT( ,  "+-符号后应为数字")
UTILLIB_ETAB_ELEM_INIT( ,  "case后应为常量")
UTILLIB_ETAB_ELEM_INIT( ,  "case后应为:")
UTILLIB_ETAB_ELEM_INIT( ,  "default应为:")
UTILLIB_ETAB_ELEM_INIT( ,  "未定义main函数")
UTILLIB_ETAB_ELEM_INIT( ,  "main函数返回类型为void")
UTILLIB_ETAB_ELEM_INIT( ,  "错误的结束字符")
UTILLIB_ETAB_ELEM_INIT( ,  "return语句与函数类型不匹配")
UTILLIB_ETAB_ELEM_INIT( ,  "该标识符未定义或者不是函数")
UTILLIB_ETAB_ELEM_INIT( ,  "scanf语句只能接受非数组变量")
UTILLIB_ETAB_ELEM_INIT( ,  "switch语句中的标签与表达式类型不匹配")
UTILLIB_ETAB_ELEM_INIT( ,  "switch语句中的标签重复")
UTILLIB_ETAB_ELEM_INIT( ,  "程序不完整，可能未正确定义main函数，自动结束编译")
UTILLIB_ETAB_ELEM_INIT( ,  "标识符与保留字冲突或者保留字大小写出错")
UTILLIB_ETAB_ELEM_INIT( ,  "为函数或常量赋值")
UTILLIB_ETAB_ELEM_INIT( ,  "非法因子")
UTILLIB_ETAB_END(cling_rd_parser_error_kind);
