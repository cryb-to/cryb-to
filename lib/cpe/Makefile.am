AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-cpe.la

libcryb_cpe_la_SOURCES = \
	cryb_cpe_clone.c \
	cryb_cpe_copy_attr.c \
	cryb_cpe_destroy.c \
	cryb_cpe_new.c \
	cryb_cpe_string.c \
	cryb_cpe_upgrade22.c \
	cryb_cpe_upgrade.c \
	cryb_cpe_uri.c \
	cryb_cpe_wfn.c \
	\
	cryb_cpe.c

libcryb_cpe_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_cpe_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-cpe.pc
