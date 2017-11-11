AM_CPPFLAGS=-I$(srcdir)/../ -g3
ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libutillib.la

libutillib_la_SOURCES = opg_builder.c \
												ll1_builder.c \
												ll1_parser.c \
												ll1_generator.c \
												unordered_op.c \
												bitset.c \
												vector2.c \
												symbol.c \
												rule.c \
												scanner.c \
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
									scanner.h \
									ll1_builder.h \
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
									unordered_map.h \
									unordered_op.h \
									bitset.h \
									vector2.h \
									vector.h

check_PROGRAMS= utillib_test
utillib_test_SOURCES=test/driver.c \
										 test/non_ll1_sample_1.c \
										 test/non_ll1_sample_2.c \
										 test/ll1_generator.c \
										 test/slist.c \
										 test/print.c \
										 test/ll1_sample_1.c \
										 test/ll1_sample_2.c \
										 test/flags.c \
										 test/vector2.c \
										 test/ll1_parser.c \
										 test/scanner.c \
										 test/ll1_builder.c \
										 test/bitset.c \
										 test/rule.c \
										 test/symbol.c \
										 test/hash.c \
										 test/test.c \
										 test/json.c \
										 test/vector.c \
										 test/string.c

utillib_test_LDADD=libutillib.la

noinst_PROGRAMS=example-json \
								example-vector \
								example-string 

example_json_SOURCES=example/json.c
example_json_LDADD=libutillib.la

example_vector_SOURCES=example/vector.c
example_vector_LDADD=libutillib.la

example_string_SOURCES=example/string.c
example_string_LDADD=libutillib.la

