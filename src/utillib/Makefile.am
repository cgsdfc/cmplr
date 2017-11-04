AM_CPPFLAGS=-I$(srcdir)/../ -g3
ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libutillib.la

libutillib_la_SOURCES = enum.c \
												bitset.c \
												symbol.c \
												rule.c \
												equal.c \
												error.c \
												hash.c \
												json.c \
												logging.c \
												pair.c \
												position.c \
												print.c \
												slist.c \
												string.c \
												strref.c \
												test.c \
												unordered_map.c \
												vector.c 

include_HEADERS = flags.h \
									symbol.h \
									color.h \
									enum.h \
									equal.h \
									error.h \
									hash.h \
									json.h \
									logging.h \
									pair.h \
									position.h \
									print.h \
									slist.h \
									string.h \
									strref.h \
									test.h \
									types.h \
									union.h \
									unordered_map.h \
									unordered_op.h \
									bitset.h \
									vector.h

check_LTLIBRARIES=libutillib_test.la
libutillib_test_la_SOURCES= test/json.c \
														test/bitset.c \
														test/rule.c \
														test/symbol.c \
														test/hash.c \
														test/test.c \
														test/vector.c \
														test/string.c

check_PROGRAMS= utillib_test
utillib_test_SOURCES=test/driver.c
utillib_test_LDADD=libutillib.la libutillib_test.la

noinst_PROGRAMS=example_json

example_json_SOURCES=example/json.c
example_json_LDADD=-lutillib
