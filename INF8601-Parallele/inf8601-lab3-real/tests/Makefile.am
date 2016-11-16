TESTS_ENVIRONMENT = \
  PATH='$(abs_top_builddir)/src$(PATH_SEPARATOR)'"$$PATH" \
  abs_top_builddir='$(abs_top_builddir)' \
  abs_top_srcdir='$(abs_top_srcdir)' \
  LANG=en_US

check_SCRIPTS = check-heatsim.sh
check_PROGRAMS = test test_grid test_cart test_heat test_image

AM_CFLAGS = -I$(top_srcdir)/src
CC=gcc

test_SOURCES = test_part.c
test_LDADD = $(abs_top_srcdir)/src/libinf.a libunittest.a

test_grid_SOURCES = test_grid.c
test_grid_LDADD = $(abs_top_srcdir)/src/libinf.a libunittest.a

test_cart_SOURCES = test_cart.c
test_cart_LDADD = $(abs_top_srcdir)/src/libinf.a libunittest.a

test_heat_SOURCES = test_heat.c
test_heat_LDADD = $(abs_top_srcdir)/src/libinf.a libunittest.a

test_image_SOURCES = test_image.c
test_image_LDADD = $(abs_top_srcdir)/src/libinf.a libunittest.a  $(PNG_LIBS)

noinst_LIBRARIES = libunittest.a

libunittest_a_SOURCES = unittest.c unittest.h

TESTS = $(check_PROGRAMS) $(check_SCRIPTS)

EXTRA_DIST = check-heatsim.sh simple.png
