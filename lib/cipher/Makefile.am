AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-cipher.la

libcryb_cipher_la_SOURCES = \
	cryb_aes.c \
	cryb_chacha.c \
	cryb_des.c \
	cryb_rc4.c \
	cryb_salsa.c \
	\
	cryb_cipher.c

libcryb_cipher_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_cipher_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-cipher.pc
