AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-digest.la

libcryb_digest_la_SOURCES = \
	cryb_md2.c \
	cryb_md4.c \
	cryb_md5.c \
	cryb_sha1.c \
	cryb_sha224.c \
	cryb_sha256.c \
	cryb_sha384.c \
	cryb_sha512.c \
	\
	cryb_digest.c

dist_man3_MANS = \
	cryb_md2.3 \
	cryb_md4.3 \
	cryb_md5.3 \
	cryb_sha1.3 \
	cryb_sha224.3 \
	cryb_sha256.3 \
	cryb_sha384.3 \
	cryb_sha512.3 \
	\
	cryb_digest.3

libcryb_digest_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_digest_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-digest.pc
