bin_PROGRAMS = exchng heatsim

exchng_SOURCES = exchng.c
exchng_LDADD = libinf.a

heatsim_SOURCES = heatsim.c
heatsim_LDADD = libinf.a $(PNG_LIBS)

noinst_LIBRARIES = libinf.a

libinf_a_SOURCES = memory.c memory.h util.h part.c part.h \
					grid.c grid.h cart.h cart.c heat.h heat.c \
					color.c color.h image.c image.h

libinf_a_CFLAGS = $(PNG_CFLAGS)
