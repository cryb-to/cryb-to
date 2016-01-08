AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-core.la

libcryb_core_la_SOURCES = \
	cryb_core.c \
	cryb_memset_s.c \
	string.c \
	cryb_strlcat.c \
	cryb_strlcpy.c \
	cryb_strlcmp.c \
	wstring.c

EXTRA_DIST = _string.c

dist_man3_MANS = \
	cryb_strlcat.3 \
	cryb_strlcpy.3
