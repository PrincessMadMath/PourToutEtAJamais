AC_INIT([INF8601-LAB3], 2.1.2)
AC_CONFIG_SRCDIR([src/exchng.c])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall])
AC_USE_SYSTEM_EXTENSIONS

LT_PATH_LD
LT_INIT

AC_CHECK_HEADERS(sys/types.h unistd.h fcntl.h strings.h pthread.h time.h errno.h stdarg.h limits.h signal.h stdlib.h)
AC_CHECK_HEADERS(inttypes.h math.h)
AC_CHECK_LIB(m, pow)
#AC_CHECK_LIB(stdc++, fclose)
PKG_CHECK_MODULES(PNG, libpng, [], AC_MSG_ERROR([ *** Checks for PNG library failed.]))
#PKG_CHECK_MODULES(GDK, gdk-pixbuf-2.0, [], AC_MSG_ERROR([ *** Checks for GDK library failed.]))
AC_OPENMP
CS_AC_TEST_MPI

AC_MSG_CHECKING(whether to enable debug mode)
debug_default="no"
AC_ARG_ENABLE(debug,
        AS_HELP_STRING([--enable-debug],[turn on debug mode [[default=no]]])
        , , enable_debug=$debug_default)
if test "$enable_debug" = "yes"; then
    AC_MSG_RESULT(yes)
    CFLAGS="$CFLAGS -Wall -g -O0 -fno-inline"
    AC_DEFINE([DEBUG],[],[Debug])
else
    AC_MSG_RESULT(no)
    CFLAGS="$CFLAGS -Wall -O3 -fomit-frame-pointer"
fi


# be silent by default
AM_SILENT_RULES([yes])

AC_PROG_LIBTOOL
AC_PROG_CC(mpicc)
#AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_RANLIB

AC_CONFIG_FILES([Makefile
    examples/Makefile
    tests/Makefile
    src/Makefile])
AC_OUTPUT
