AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-hash.la

libcryb_hash_la_SOURCES = \
	cryb_adler32.c \
	cryb_fletcher16.c \
	cryb_fletcher32.c \
	cryb_fletcher64.c \
	cryb_fnv.c \
	cryb_murmur3_32.c \
	cryb_pearson.c \
	\
	cryb_hash.c

dist_man3_MANS = \
	cryb_adler32.3 \
	cryb_fletcher.3 \
	cryb_fnv.3 \
	cryb_pearson.3 \
	\
	cryb_hash.3

noinst_HEADERS = \
	cryb_fnv_impl.h

libcryb_hash_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_hash_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-hash.pc
