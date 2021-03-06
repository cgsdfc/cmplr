# 
#  Cmplr Library
#  Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
#  02110-1301 USA


ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign subdir-objects


PASCAL_TEST_DIR=-DPASCAL_TEST_DIR=\"$(abs_srcdir)/src/pascal/test/\"
PASCAL_SOURCE_DIR=-DPASCAL_SOURCE_DIR=\"$(abs_srcdir)/src/pascal/\"
UTILLIB_SOURCE_DIR=-DUTILLIB_SOURCE_DIR=\"$(abs_srcdir)/src/utillib/\"

AM_CFLAGS = -I$(top_srcdir)/src
AM_CFLAGS += -g3

AM_CFLAGS += -Wall -Wwrite-strings \
						 -Wno-sign-compare \
						 -std=c11
AM_CXXFLAGS = -std=c++11 -I$(top_srcdir)/src

## Utillib
utillibincludedir = $(includedir)/utillib

lib_LTLIBRARIES = libutillib.la

utillibinclude_HEADERS =  src/utillib/ll1_generator.h \
													src/utillib/hashmap.h \
													src/utillib/opg_builder.h \
													src/utillib/scanner.h \
													src/utillib/ll1_parser.h \
													src/utillib/vector2.h \
													src/utillib/bitset.h \
													src/utillib/ll1_builder.h \
													src/utillib/json_scanner.h \
													src/utillib/json_parser.h \
													src/utillib/symbol.h \
													src/utillib/rule.h \
													src/utillib/pair.h \
													src/utillib/json.h \
													src/utillib/test.h \
													src/utillib/print.h \
													src/utillib/slist.h \
													src/utillib/vector.h \
													src/utillib/string.h

utillib_impl_headers = src/utillib/ll1_builder_impl.h \
											 src/utillib/hashmap_impl.h \
											 src/utillib/json_impl.h \
											 src/utillib/json_parser_impl.h

libutillib_la_SOURCES =  $(libutillib_la_headers) \
												 $(utillib_impl_headers) \
												 src/utillib/hashmap_impl.c \
												 src/utillib/graph.c \
												 src/utillib/lr_builder.c \
												 src/utillib/strhash.c \
												 src/utillib/test_impl.c \
												 src/utillib/ll1_builder_impl.c \
												 src/utillib/json_parser.c \
												 src/utillib/json_scanner.c \
												 src/utillib/json_parser_impl.c \
												 src/utillib/ll1_generator.c \
												 src/utillib/opg_builder.c \
												 src/utillib/scanner.c \
												 src/utillib/ll1_parser.c \
												 src/utillib/vector2.c \
												 src/utillib/bitset.c \
												 src/utillib/ll1_builder.c \
												 src/utillib/symbol.c \
												 src/utillib/rule.c \
												 src/utillib/pair.c \
												 src/utillib/json.c \
												 src/utillib/test.c \
												 src/utillib/print.c \
												 src/utillib/hashmap.c \
												 src/utillib/slist.c \
												 src/utillib/vector.c \
												 src/utillib/string.c

noinst_PROGRAMS = utillib_test
TESTS = utillib_test
utillib_test_SOURCES = src/utillib/test/ll1_generator.c \
											 src/utillib/test/ll1_sample_1.c \
											 src/utillib/test/graph.c \
											 src/utillib/test/ll1_sample_2.c \
											 src/utillib/test/ll1_sample_1.h \
											 src/utillib/test/ll1_sample_2.h \
											 src/utillib/test/json_scanner.c \
											 src/utillib/test/non_ll1_sample_1.c \
											 src/utillib/test/scanner.c \
											 src/utillib/test/ll1_parser.c \
											 src/utillib/test/vector2.c \
											 src/utillib/test/bitset.c \
											 src/utillib/test/ll1_builder.c \
											 src/utillib/test/symbol.c \
											 src/utillib/test/rule.c \
											 src/utillib/test/json.c \
											 src/utillib/test/json_parser.c \
											 src/utillib/test/test.c \
											 src/utillib/test/print.c \
											 src/utillib/test/hashmap.c \
											 src/utillib/test/slist.c \
											 src/utillib/test/vector.c \
											 src/utillib/test/string.c \
											 src/utillib/test/driver.c 

utillib_test_LDADD = libutillib.la
utillib_test_CPPFLAGS = -DUTILLIB_TEST_DATA_DIR=\"$(abs_srcdir)/utillib/test/data/\"

noinst_PROGRAMS += json_parser_table_gen
json_parser_table_gen_SOURCES = src/utillib/json_parser_table_gen.c
json_parser_table_gen_LDADD = libutillib.la
json_parser_table_gen_CPPFLAGS = $(UTILLIB_SOURCE_DIR)

noinst_PROGRAMS += example-json
example_json_SOURCES = src/utillib/example/json.c
example_json_LDADD = libutillib.la

noinst_PROGRAMS += example-string
example_string_SOURCES = src/utillib/example/string.c
example_string_LDADD = libutillib.la

noinst_PROGRAMS += example-vector
example_vector_SOURCES = src/utillib/example/vector.c
example_vector_LDADD = libutillib.la

## CXX binding
lib_LTLIBRARIES += libutillib++.la
utillibinclude_HEADERS += src/utillib/utillib.hpp
libutillib___la_SOURCES = src/utillib/utillib.cpp
libutillib___la_LIBADD = libutillib.la

## Pascal
lib_LTLIBRARIES += libpascal.la
noinst_pascalinclude_HEADERS =  src/pascal/symbols.h \
																src/pascal/rules.h \
																src/pascal/scanner.h \
																src/pascal/parser.h

libpascal_la_SOURCES = src/pascal/symbols.c \
											 src/pascal/rules.c \
											 src/pascal/scanner.c \
											 src/pascal/parser.c \
											 $(libpascal_la_headers)

libpascal_la_LIBADD = libutillib.la

bin_PROGRAMS = pascal
pascal_SOURCES = src/pascal/main.c
pascal_LDADD = libpascal.la

noinst_PROGRAMS += pascal_ll1_table_gen
pascal_ll1_table_gen_SOURCES = src/pascal/ll1_table_gen.c 
pascal_ll1_table_gen_LDADD = libutillib.la libpascal.la
pascal_ll1_table_gen_CPPFLAGS = $(PASCAL_SOURCE_DIR)

noinst_pascalincludedir = $(includedir)/pascal
noinst_PROGRAMS += pascal_test
TESTS += pascal_test
pascal_test_SOURCES = src/pascal/test/symbols.c \
											src/pascal/test/rules.c \
											src/pascal/test/scanner.c \
											src/pascal/test/parser.c \
											src/pascal/test/driver.c
pascal_test_LDADD = libutillib.la libpascal.la
pascal_test_CPPFLAGS = $(PASCAL_TEST_DIR) -DPASCAL_TEST_DATA_DIR=\"$(abs_srcdir)/src/pascal/test/data/\"

## Cling
noinst_clingincludedir = $(includedir)/cling

lib_LTLIBRARIES += libcling.la
noinst_clinginclude_HEADERS = ./src/cling/symbols.h \
															./src/cling/rd_parser.h \
															./src/cling/symbol_table.h \
															./src/cling/opg_parser.h \
															./src/cling/error.h \
															./src/cling/scanner.h \
															./src/cling/ast_ir.h \
															./src/cling/ast_build.h \
															./src/cling/ast_check.h \
															./src/cling/ast_pretty.h \
															./src/cling/mips.h \
															./src/cling/misc.h

libcling_la_SOURCES = 	$(noinst_clinginclude_HEADERS) \
												./src/cling/symbol_table.c \
												./src/cling/symbols.c  \
												./src/cling/scanner.c \
												./src/cling/rd_parser.c \
												./src/cling/ast_build.c \
												./src/cling/ast_pretty.c \
												./src/cling/ast_check.c \
												./src/cling/ast_ir.c \
												./src/cling/error.c \
												./src/cling/opg_parser.c \
												./src/cling/mips.c \
												./src/cling/misc.c

libcling_la_LIBADD = libutillib.la

bin_PROGRAMS += cling-scanner
cling_scanner_SOURCES = ./src/cling/cling-scanner.c
cling_scanner_LDADD = libcling.la libutillib.la

bin_PROGRAMS += cling-parser
cling_parser_SOURCES = ./src/cling/cling-parser.c
cling_parser_LDADD = libcling.la libutillib.la

