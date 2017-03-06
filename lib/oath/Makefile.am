AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-oath.la

libcryb_oath_la_SOURCES = \
	cryb_oath_hotp.c \
	cryb_oath_totp.c \
	cryb_oath_key_alloc.c \
	cryb_oath_key_create.c \
	cryb_oath_key_dummy.c \
	cryb_oath_key_from_uri.c \
	cryb_oath_key_free.c \
	cryb_oath_key_to_uri.c \
	cryb_oath_mode.c \
	\
	cryb_oath.c

libcryb_oath_la_LIBADD = \
	$(top_builddir)/lib/digest/libcryb-digest.la \
	$(top_builddir)/lib/enc/libcryb-enc.la \
	$(top_builddir)/lib/mac/libcryb-mac.la \
	$(top_builddir)/lib/rand/libcryb-rand.la \
	$(top_builddir)/lib/core/libcryb-core.la

pkgconfig_DATA = cryb-oath.pc
