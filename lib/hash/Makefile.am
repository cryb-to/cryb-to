AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-hash.la

libcryb_hash_la_SOURCES = \
	cryb_adler32.c \
	cryb_fletcher16.c \
	cryb_fletcher32.c \
	cryb_fletcher64.c \
	cryb_murmur3_32.c \
	cryb_pearson.c \
	\
	cryb_hash.c

dist_man3_MANS = \
	cryb_adler32.3 \
	cryb_fletcher.3 \
	cryb_pearson.3 \
	\
	cryb_hash.3
