AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-mac.la

libcryb_mac_la_SOURCES = \
	cryb_hmac_sha1.c \
	cryb_hmac_sha224.c \
	cryb_hmac_sha256.c \
	cryb_hmac_sha384.c \
	cryb_hmac_sha512.c \
	\
	cryb_mac.c

libcryb_mac_la_CFLAGS = \
	$(CRYB_DIGEST_CFLAGS) \
	$(CRYB_CORE_CFLAGS)

libcryb_mac_la_LIBADD = \
	$(CRYB_DIGEST_LIBS) \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-mac.pc
