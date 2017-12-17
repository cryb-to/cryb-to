AM_CPPFLAGS = -I$(top_srcdir)/include $(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)

lib_LTLIBRARIES = libcryb-cipher.la

libcryb_cipher_la_SOURCES = \
	cryb_aes.c \
	cryb_chacha.c \
	cryb_des.c \
	cryb_rc4.c \
	cryb_salsa.c \
	\
	cryb_cipher.c

libcryb_cipher_la_LIBADD = \
	$(top_builddir)/lib/core/libcryb-core.la

pkgconfig_DATA = cryb-cipher.pc
