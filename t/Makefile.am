# $Cryb$

AM_CPPFLAGS = -I$(top_srcdir)/include

if WITH_BENCHMARKS
AM_CPPFLAGS += -DBENCHMARKS
endif

noinst_HEADERS = t.h

# Common support code
check_LTLIBRARIES = libt.la
libt_la_SOURCES = t_main.c t_file.c

# Link in the test driver and the full cryb library
LDADD = $(builddir)/libt.la \
	$(top_builddir)/lib/core/libcryb-core.la \
	$(top_builddir)/lib/enc/libcryb-enc.la \
	$(top_builddir)/lib/digest/libcryb-digest.la \
	$(top_builddir)/lib/mac/libcryb-mac.la \
	$(top_builddir)/lib/rand/libcryb-rand.la \
	$(top_builddir)/lib/oath/libcryb-oath.la

# Additional headers, flags and libraries for OpenSSL
if WITH_OPENSSL
OPENSSL_INCLUDES = $(INCLUDES)
OPENSSL_CFLAGS = -DWITH_OPENSSL=1
OPENSSL_LDADD = $(LDADD) -lcrypto
endif

# Additional headers, flags and libraries for RSAREF
if WITH_RSAREF
RSAREF_INCLUDES = $(INCLUDES) -I$(top_srcdir)/lib/rsaref
RSAREF_CFLAGS = -DWITH_RSAREF=1 -DPROTOTYPES=1
RSAREF_LDADD = $(LDADD) $(top_builddir)/lib/rsaref/librsaref.la
endif

# tests
TESTS =

# libcryb-core
TESTS += t_ctype

# libcryb-enc
TESTS += t_rfc3986 t_rfc4648

# libcryb-digest
TESTS += t_md5
if WITH_OPENSSL
TESTS += t_md5_openssl
t_md5_openssl_SOURCES = t_md5.c
t_md5_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_md5_openssl_LDADD = $(OPENSSL_LDADD)
endif
if WITH_RSAREF
TESTS += t_md5_rsaref
t_md5_rsaref_SOURCES = t_md5.c
t_md5_rsaref_CFLAGS = $(RSAREF_INCLUDES) $(RSAREF_CFLAGS)
t_md5_rsaref_LDADD = $(RSAREF_LDADD)
endif

TESTS += t_sha1
if WITH_OPENSSL
TESTS += t_sha1_openssl
t_sha1_openssl_SOURCES = t_sha1.c
t_sha1_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_sha1_openssl_LDADD = $(OPENSSL_LDADD)
endif

# libcryb-mac
TESTS += t_hmac
if WITH_OPENSSL
TESTS += t_hmac_openssl
t_hmac_openssl_SOURCES = t_hmac.c
t_hmac_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_hmac_openssl_LDADD = $(OPENSSL_LDADD)
endif

check_PROGRAMS = $(TESTS)
