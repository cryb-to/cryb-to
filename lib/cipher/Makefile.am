AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-cipher.la

libcryb_cipher_la_SOURCES = \
	cryb_aes.c \
	cryb_rc4.c \
	\
	cryb_cipher.c

libcryb_cipher_la_LIBADD = \
	$(top_builddir)/lib/core/libcryb-core.la
