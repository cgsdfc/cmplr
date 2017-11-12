AC_PREREQ([2.59])
AC_INIT([cmplr], [1.0.0], [cgsdfc@126.com])

AC_CONFIG_SRCDIR([README.md])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([dist-zip])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile])
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([limits.h stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset pow strdup strerror strtoul])

AC_OUTPUT

