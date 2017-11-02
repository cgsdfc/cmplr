AM_CPPFLAGS=-I$(srcdir)/../
ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libutillib.la

libutillib_la_SOURCES = enum.c \
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
		  typedef.h \
		  union.h \
		  unordered_map.h \
		  utillib.h \
		  vector.h

check_LTLIBRARIES=libutillib_test.la
libutillib_test_la_SOURCES= test/json.c \
			    test/hash.c \
			    test/test.c \
			    test/vector.c \
			    test/string.c

check_PROGRAMS= utillib_test
utillib_test_SOURCES=test/driver.c
utillib_test_LDADD=libutillib.la libutillib_test.la
