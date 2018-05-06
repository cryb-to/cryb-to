AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-oath.la

libcryb_oath_la_SOURCES = \
	cryb_oath_hotp.c \
	cryb_oath_totp.c \
	\
	cryb_oath_key_create.c \
	cryb_oath_key_destroy.c \
	cryb_oath_key_dummy.c \
	cryb_oath_key_from_uri.c \
	cryb_oath_key_to_uri.c \
	cryb_oath_mode.c \
	\
	cryb_oath.c

libcryb_oath_la_CFLAGS = \
	$(CRYB_DIGEST_CFLAGS) \
	$(CRYB_ENC_CFLAGS) \
	$(CRYB_MAC_CFLAGS) \
	$(CRYB_RAND_CFLAGS) \
	$(CRYB_CORE_CFLAGS)

libcryb_oath_la_LIBADD = \
	$(CRYB_DIGEST_LIBS) \
	$(CRYB_ENC_LIBS) \
	$(CRYB_MAC_LIBS) \
	$(CRYB_RAND_LIBS) \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-oath.pc
