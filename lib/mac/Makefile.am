AM_CPPFLAGS = -I$(top_srcdir)/include $(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)

lib_LTLIBRARIES = libcryb-mac.la

libcryb_mac_la_SOURCES = \
	cryb_hmac_sha1.c \
	cryb_hmac_sha224.c \
	cryb_hmac_sha256.c \
	cryb_hmac_sha384.c \
	cryb_hmac_sha512.c \
	\
	cryb_mac.c

libcryb_mac_la_LIBADD = \
	$(top_builddir)/lib/core/libcryb-core.la \
	$(top_builddir)/lib/digest/libcryb-digest.la

pkgconfig_DATA = cryb-mac.pc
