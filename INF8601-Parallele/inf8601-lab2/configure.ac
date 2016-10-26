AC_INIT([INF8601-LAB2], 2.1.2)
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_SRCDIR([src/sinoscope.c])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS

LT_PATH_LD
LT_INIT

AC_CHECK_HEADERS(sys/types.h unistd.h fcntl.h strings.h pthread.h time.h errno.h stdarg.h limits.h signal.h stdlib.h)
AC_CHECK_HEADERS(inttypes.h math.h)
AC_CHECK_LIB(m, pow)
AC_CHECK_LIB(stdc++, fclose)
#PKG_CHECK_MODULES(GLEW, glew, [], AC_MSG_ERROR([ *** Checks for GLEW library failed.]))

AC_OPENMP

AC_ARG_WITH([include],
            [AS_HELP_STRING([--with-include],
                            [Add include directory path])],
                            [CPPFLAGS="$CPPFLAGS -I$withval"],
                            [])


AC_CHECK_HEADERS(CL/cl.h, [], [
    AC_CHECK_HEADERS(CL/cl.h, [], [
        echo "OpenCL header files not found in your system"
        exit -1
    ])
])


AC_MSG_CHECKING(whether host is Mac OS)
case "${host_os}" in
    darwin* )
        LDFLAGS="$LDFLAGS -framework OpenCL"
        AC_MSG_RESULT(yes)
    ;;
    *)
        AC_MSG_RESULT(no)
        AC_CHECK_LIB(OpenCL, clGetPlatformIDs, [],[ 
            echo "OpenCL library not found in your system"
            exit -1
        ])
esac

AC_MSG_CHECKING(whether to enable debug mode)
debug_default="no"
AC_ARG_ENABLE(debug,
        AS_HELP_STRING([--enable-debug],[turn on debug mode [[default=no]]])
        , , enable_debug=$debug_default)
if test "$enable_debug" = "yes"; then
    AC_MSG_RESULT(yes)
    CFLAGS="-Wall -g -O0 -fno-inline"
    AC_DEFINE([DEBUG],[],[Debug])
else
    AC_MSG_RESULT(no)
    CFLAGS="-Wall -O3 -fomit-frame-pointer"
fi



OPENCLCC=openclcc-embed
AC_SUBST(OPENCLCC)

# be silent by default
AM_SILENT_RULES([yes])

AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_RANLIB

AC_CONFIG_FILES([Makefile
    tests/Makefile
    encode/Makefile
    src/Makefile])
AC_OUTPUT
