AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-rand.la

libcryb_rand_la_SOURCES = \
	cryb_rand_bytes.c \
	\
	cryb_rand.c

libcryb_rand_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_rand_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-rand.pc
