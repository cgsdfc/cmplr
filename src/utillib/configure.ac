#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([utillib], [0.0.1], [cgsdfc@126.com])
AC_CONFIG_SRCDIR([unordered_map.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([float.h stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup strerror])

AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([m4])
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_OUTPUT
