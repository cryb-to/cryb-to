AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-core.la

libcryb_core_la_SOURCES = \
	cryb_core.c \
	cryb_memset_s.c \
	cryb_string.c \
	cryb_strlcat.c \
	cryb_strlcmp.c \
	cryb_strlcpy.c \
	cryb_wcslcat.c \
	cryb_wcslcmp.c \
	cryb_wcslcpy.c \
	cryb_wstring.c

EXTRA_DIST = cryb_string_impl.c

dist_man3_MANS = \
	cryb_strlcat.3 \
	cryb_strlcpy.3
