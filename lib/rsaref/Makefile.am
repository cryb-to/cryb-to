AM_CPPFLAGS = -I$(top_srcdir)/include $(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)

noinst_LTLIBRARIES = librsaref.la

librsaref_la_SOURCES = \
	desc.c \
	digit.c \
	md2c.c \
	md5c.c \
	nn.c \
	prime.c \
	r_dh.c \
	r_encode.c \
	r_enhanc.c \
	r_keygen.c \
	r_random.c \
	r_stdlib.c \
	rsa.c
librsaref_la_CFLAGS = -DPROTOTYPES=1
