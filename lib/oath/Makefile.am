# $Id$

AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-oath.la

libcryb_oath_la_SOURCES = \
        oath_hotp.c \
        oath_totp.c \
        oath_key_alloc.c \
        oath_key_create.c \
        oath_key_dummy.c \
        oath_key_from_uri.c \
        oath_key_free.c \
        oath_key.c

libcryb_oath_la_LIBADD = \
	$(top_builddir)/lib/digest/libcryb-digest.la \
	$(top_builddir)/lib/mac/libcryb-mac.la \
	$(top_builddir)/lib/enc/libcryb-enc.la
