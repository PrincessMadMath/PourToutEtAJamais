bin_PROGRAMS = sinoscope

sinoscope_SOURCES = sinoscope.c sinoscope.h util.h sinoscope_openmp.c sinoscope_openmp.h sinoscope_serial.c sinoscope_serial.h color.c color.h
sinoscope_CFLAGS = $(OPENMP_CFLAGS)
sinoscope_LDFLAGS = -lglut -lGL -lGLU -lGLEW -lOpenCL
sinoscope_LDADD = libbcl.a

noinst_LIBRARIES = libbcl.a

libbcl_a_SOURCES = sinoscope_opencl.cpp sinoscope_opencl.h memory.c memory.h sinoscope_kernel.cl
libbcl_a_CXXFLAGS = $(CXXFLAGS)

.cl.o:
	$(OPENCLCC) $@ $<

