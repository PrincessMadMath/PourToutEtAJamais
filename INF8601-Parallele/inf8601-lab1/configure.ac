AC_INIT([INF8601-LAB1-master], 2.1.0)
AC_CONFIG_SRCDIR([src/dragonizer.c])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([color-tests])

LT_INIT

AC_CHECK_HEADERS(sys/types.h unistd.h fcntl.h strings.h pthread.h time.h errno.h stdarg.h limits.h signal.h stdlib.h)
AC_CHECK_HEADERS(inttypes.h math.h tbb/tbb.h)
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_LIB(tbb, TBB_runtime_interface_version)
AC_CHECK_LIB(m, pow)
AC_CHECK_LIB(stdc++, fclose)

# Fedora has no pkg-config for tbb
#PKG_CHECK_MODULES(TBB, [tbb])

AC_MSG_CHECKING(whether to enable debug mode)
debug_default="no"
AC_ARG_ENABLE(debug,
        AS_HELP_STRING([--enable-debug],[turn on debug mode [[default=no]]])
        , , enable_debug=$debug_default)
if test "$enable_debug" = "yes"; then
    AC_MSG_RESULT(yes)
    CFLAGS="-Wall -g -O0 -fno-inline"
    CXXFLAGS="-Wall -g -O0 -fno-inline -std=c++0x"
    AC_DEFINE([DEBUG],[],[Debug])
else
    AC_MSG_RESULT(no)
    CFLAGS="-Wall -O2 -fomit-frame-pointer"
    CXXFLAGS="-Wall -O2 -fomit-frame-pointer -std=c++0x"
fi

AC_OPENMP

# be silent by default
AM_SILENT_RULES([yes])

AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_RANLIB
AC_CONFIG_FILES([Makefile
    tests/Makefile
    src/Makefile])
AC_OUTPUT

echo "
	C Compiler.....: $CC $CFLAGS
	C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
"
