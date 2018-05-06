AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-enc.la

libcryb_enc_la_SOURCES = \
	cryb_base32_decode.c \
	cryb_base32_encode.c \
	cryb_base64_decode.c \
	cryb_base64_encode.c \
	cryb_percent_decode.c \
	cryb_percent_encode.c \
	\
	cryb_enc.c

dist_man3_MANS = \
	cryb_percent.3

libcryb_enc_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_enc_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-enc.pc
