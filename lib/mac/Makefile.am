# $Cryb$

AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-mac.la

libcryb_mac_la_SOURCES = \
	hmac_sha1.c

libcryb_mac_la_LIBADD = \
	$(top_builddir)/lib/digest/libcryb-digest.la
