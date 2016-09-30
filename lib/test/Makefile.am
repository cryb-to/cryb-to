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

libcryb_test_la_LIBADD = \
	$(top_builddir)/lib/core/libcryb-core.la
