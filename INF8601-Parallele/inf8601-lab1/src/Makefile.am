bin_PROGRAMS = dragonizer

dragonizer_SOURCES = dragon_pthread.c dragon_pthread.h dragonizer.c
dragonizer_LDADD = libdragontbb.a libdragon.a
dragonizer_CFLAGS = $(OPENMP_CFLAGS)

noinst_LIBRARIES = libdragontbb.a libdragon.a

libdragon_a_SOURCES = color.c color.h utils.c utils.h dragon.c dragon.h
libdragon_a_CFLAGS = $(OPENMP_CFLAGS)

libdragontbb_a_SOURCES = dragon_tbb.cpp dragon_tbb.h TidMap.h TidMap.cpp
libdragontbb_a_LIBADD = libdragon.a
