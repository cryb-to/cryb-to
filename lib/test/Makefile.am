AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-test.la
libcryb_test_la_SOURCES = \
	cryb_t_compare.c \
	cryb_t_const.c \
	cryb_t_file.c \
	cryb_t_main.c \
	cryb_t_malloc.c \
	\
	cryb_test.c

libcryb_test_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_test_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-test.pc
