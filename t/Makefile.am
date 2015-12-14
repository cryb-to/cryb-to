AM_CPPFLAGS = -I$(top_srcdir)/include

EXTRA_DIST =

if CRYB_TEST

# Individual libraries
libcipher = $(top_builddir)/lib/cipher/libcryb-cipher.la
libcore = $(top_builddir)/lib/core/libcryb-core.la
libdigest = $(top_builddir)/lib/digest/libcryb-digest.la
libenc = $(top_builddir)/lib/enc/libcryb-enc.la
libhash = $(top_builddir)/lib/hash/libcryb-hash.la
libmac = $(top_builddir)/lib/mac/libcryb-mac.la
libmpi = $(top_builddir)/lib/mpi/libcryb-mpi.la
liboath = $(top_builddir)/lib/oath/libcryb-oath.la
librand = $(top_builddir)/lib/rand/libcryb-rand.la
libt = $(top_builddir)/lib/test/libcryb-test.la

# Additional headers, flags and libraries for OpenSSL
if OPENSSL_TESTS
OPENSSL_INCLUDES = $(INCLUDES)
OPENSSL_CFLAGS = -DWITH_OPENSSL=1
OPENSSL_LDADD = -lcrypto
endif OPENSSL_TESTS

# Additional headers, flags and libraries for RSAREF
if RSAREF_TESTS
RSAREF_INCLUDES = $(INCLUDES) -I$(top_srcdir)/lib/rsaref
RSAREF_CFLAGS = -DWITH_RSAREF=1 -DPROTOTYPES=1
RSAREF_LDADD = $(top_builddir)/lib/rsaref/librsaref.la
endif RSAREF_TESTS

# tests
TESTS =

# libcryb-cipher
if CRYB_CIPHER
TESTS += t_aes t_rc4
t_aes_LDADD = $(libt) $(libcipher)
t_rc4_LDADD = $(libt) $(libcipher)
endif CRYB_CIPHER

# libcryb-core
if CRYB_CORE
TESTS += t_ctype t_endian t_memset_s t_strlcat t_strlcmp t_strlcpy
TESTS += t_string t_wstring
EXTRA_DIST += t__string.c
t_ctype_LDADD = $(libt) $(libcore)
t_endian_LDADD = $(libt) $(libcore)
t_memset_s_LDADD = $(libt) $(libcore)
t_strlcat_LDADD = $(libt) $(libcore)
t_strlcmp_LDADD = $(libt) $(libcore)
t_strlcpy_LDADD = $(libt) $(libcore)
t_string_LDADD = $(libt) $(libcore)
t_wstring_LDADD = $(libt) $(libcore)
endif CRYB_CORE

# libcryb-enc
if CRYB_ENC
TESTS += t_rfc3986 t_rfc4648
t_rfc3986_LDADD = $(libt) $(libenc)
t_rfc4648_LDADD = $(libt) $(libenc)
endif CRYB_ENC

# libcryb-digest
if CRYB_DIGEST
TESTS += t_md2 t_md4 t_md5
t_md2_LDADD = $(libt) $(libdigest) $(RT_LIBS)
t_md4_LDADD = $(libt) $(libdigest) $(RT_LIBS)
t_md5_LDADD = $(libt) $(libdigest) $(RT_LIBS)
if OPENSSL_TESTS
TESTS += t_md4_openssl t_md5_openssl
t_md4_openssl_SOURCES = t_md4.c
t_md4_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_md4_openssl_LDADD = $(libt) $(OPENSSL_LDADD) $(RT_LIBS)
t_md5_openssl_SOURCES = t_md5.c
t_md5_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_md5_openssl_LDADD = $(libt) $(OPENSSL_LDADD) $(RT_LIBS)
endif OPENSSL_TESTS
if RSAREF_TESTS
TESTS += t_md2_rsaref t_md5_rsaref
t_md2_rsaref_SOURCES = t_md2.c
t_md2_rsaref_CFLAGS = $(RSAREF_INCLUDES) $(RSAREF_CFLAGS)
t_md2_rsaref_LDADD = $(libt) $(RSAREF_LDADD) $(RT_LIBS)
t_md5_rsaref_SOURCES = t_md5.c
t_md5_rsaref_CFLAGS = $(RSAREF_INCLUDES) $(RSAREF_CFLAGS)
t_md5_rsaref_LDADD = $(libt) $(RSAREF_LDADD) $(RT_LIBS)
endif RSAREF_TESTS
TESTS += t_sha1 t_sha224 t_sha256 t_sha384 t_sha512
t_sha1_LDADD = $(libt) $(libdigest) $(RT_LIBS)
t_sha224_LDADD = $(libt) $(libdigest) $(RT_LIBS)
t_sha256_LDADD = $(libt) $(libdigest) $(RT_LIBS)
t_sha384_LDADD = $(libt) $(libdigest) $(RT_LIBS)
t_sha512_LDADD = $(libt) $(libdigest) $(RT_LIBS)
if OPENSSL_TESTS
TESTS += t_sha1_openssl t_sha224_openssl t_sha256_openssl t_sha384_openssl t_sha512_openssl
t_sha1_openssl_SOURCES = t_sha1.c
t_sha1_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_sha1_openssl_LDADD = $(libt) $(OPENSSL_LDADD) $(RT_LIBS)
t_sha256_openssl_SOURCES = t_sha256.c
t_sha256_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_sha256_openssl_LDADD = $(libt) $(OPENSSL_LDADD) $(RT_LIBS)
t_sha224_openssl_SOURCES = t_sha224.c
t_sha224_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_sha224_openssl_LDADD = $(libt) $(OPENSSL_LDADD) $(RT_LIBS)
t_sha384_openssl_SOURCES = t_sha384.c
t_sha384_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_sha384_openssl_LDADD = $(libt) $(OPENSSL_LDADD) $(RT_LIBS)
t_sha512_openssl_SOURCES = t_sha512.c
t_sha512_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_sha512_openssl_LDADD = $(libt) $(OPENSSL_LDADD) $(RT_LIBS)
endif OPENSSL_TESTS
endif CRYB_DIGEST

# libcryb-hash
if CRYB_HASH
TESTS += t_adler t_fletcher t_murmur3_32 t_pearson
t_adler_LDADD = $(libt) $(libhash)
t_fletcher_LDADD = $(libt) $(libhash)
t_murmur3_32_LDADD = $(libt) $(libhash)
t_pearson_LDADD = $(libt) $(libhash)
endif CRYB_HASH

# libcryb-mac
if CRYB_MAC
TESTS += t_hmac_sha1 t_hmac_sha224 t_hmac_sha256 t_hmac_sha384 t_hmac_sha512
t_hmac_sha1_LDADD = $(libt) $(libmac)
t_hmac_sha224_LDADD = $(libt) $(libmac)
t_hmac_sha256_LDADD = $(libt) $(libmac)
t_hmac_sha384_LDADD = $(libt) $(libmac)
t_hmac_sha512_LDADD = $(libt) $(libmac)
if OPENSSL_TESTS
TESTS += t_hmac_sha1_openssl t_hmac_sha224_openssl t_hmac_sha256_openssl t_hmac_sha384_openssl t_hmac_sha512_openssl
t_hmac_sha1_openssl_SOURCES = t_hmac_sha1.c
t_hmac_sha1_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_hmac_sha1_openssl_LDADD = $(libt) $(OPENSSL_LDADD)
t_hmac_sha256_openssl_SOURCES = t_hmac_sha256.c
t_hmac_sha256_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_hmac_sha256_openssl_LDADD = $(libt) $(OPENSSL_LDADD)
t_hmac_sha224_openssl_SOURCES = t_hmac_sha224.c
t_hmac_sha224_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_hmac_sha224_openssl_LDADD = $(libt) $(OPENSSL_LDADD)
t_hmac_sha384_openssl_SOURCES = t_hmac_sha384.c
t_hmac_sha384_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_hmac_sha384_openssl_LDADD = $(libt) $(OPENSSL_LDADD)
t_hmac_sha512_openssl_SOURCES = t_hmac_sha512.c
t_hmac_sha512_openssl_CFLAGS = $(OPENSSL_INCLUDES) $(OPENSSL_CFLAGS)
t_hmac_sha512_openssl_LDADD = $(libt) $(OPENSSL_LDADD)
endif OPENSSL_TESTS
endif CRYB_MAC

# libcryb-mpi
if CRYB_MPI
TESTS += t_mpi
t_mpi_LDADD = $(libt) $(libmpi)
endif CRYB_MPI

check_PROGRAMS = $(TESTS)

endif CRYB_TEST
