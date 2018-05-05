AM_CPPFLAGS = -I$(top_srcdir)/include

EXTRA_DIST =
noinst_HEADERS =

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
OPENSSL_INCLUDES = 
OPENSSL_CFLAGS = -DWITH_OPENSSL=1
OPENSSL_LDADD = -lcrypto
endif OPENSSL_TESTS

# Additional headers, flags and libraries for RSAREF
if RSAREF_TESTS
RSAREF_INCLUDES = -I$(top_srcdir)/lib/rsaref
RSAREF_CFLAGS = -DWITH_RSAREF=1 -DPROTOTYPES=1
RSAREF_LDADD = $(top_builddir)/lib/rsaref/librsaref.la
endif RSAREF_TESTS

# tests
TESTS =

# c++ header test
TESTS += t_cxx
t_cxx_SOURCES = t_cxx.cc
t_cxx_CPPFLAGS = $(AM_CPPFLAGS)
t_cxx_LDADD =
if CRYB_CIPHER
t_cxx_CPPFLAGS += -DWITH_CRYB_CIPHER
t_cxx_LDADD += $(libcipher)
endif CRYB_CIPHER
if CRYB_CORE
t_cxx_CPPFLAGS += -DWITH_CRYB_CORE
t_cxx_LDADD += $(libcore)
endif CRYB_CORE
if CRYB_DIGEST
t_cxx_CPPFLAGS += -DWITH_CRYB_CORE
t_cxx_LDADD += $(libdigest)
endif CRYB_DIGEST
if CRYB_ENC
t_cxx_CPPFLAGS += -DWITH_CRYB_ENC
t_cxx_LDADD += $(libenc)
endif CRYB_ENC
if CRYB_HASH
t_cxx_CPPFLAGS += -DWITH_CRYB_HASH
t_cxx_LDADD += $(libhash)
endif CRYB_HASH
if CRYB_MAC
t_cxx_CPPFLAGS += -DWITH_CRYB_MAC
t_cxx_LDADD += $(libmac)
endif CRYB_MAC
if CRYB_MPI
t_cxx_CPPFLAGS += -DWITH_CRYB_MPI
t_cxx_LDADD += $(libmpi)
endif CRYB_MPI
if CRYB_OATH
t_cxx_CPPFLAGS += -DWITH_CRYB_OATH
t_cxx_LDADD += $(liboath)
endif CRYB_OATH
if CRYB_RAND
t_cxx_CPPFLAGS += -DWITH_CRYB_RAND
t_cxx_LDADD += $(librand)
endif CRYB_RAND

# libcryb-cipher
if CRYB_CIPHER
TESTS += t_cipher
t_cipher_LDADD = $(libt) $(libcipher)
TESTS += t_aes t_chacha t_des t_rc4 t_salsa
t_aes_LDADD = $(libt) $(libcipher)
t_chacha_LDADD = $(libt) $(libcipher)
t_des_LDADD = $(libt) $(libcipher)
t_rc4_LDADD = $(libt) $(libcipher)
t_salsa_LDADD = $(libt) $(libcipher)
endif CRYB_CIPHER

# libcryb-core
if CRYB_CORE
TESTS += t_core
t_core_LDADD = $(libt) $(libcore)
TESTS += t_assert
t_assert_LDADD = $(libt) $(libcore)
TESTS += t_ctype t_endian t_memset_s t_strchrnul t_strlcat t_strlcmp t_strlcpy
t_ctype_LDADD = $(libt) $(libcore)
t_endian_LDADD = $(libt) $(libcore)
t_memset_s_LDADD = $(libt) $(libcore)
t_strchrnul_LDADD = $(libt) $(libcore)
t_strlcat_LDADD = $(libt) $(libcore)
t_strlcmp_LDADD = $(libt) $(libcore)
t_strlcpy_LDADD = $(libt) $(libcore)
TESTS += t_rolror
t_rolror_LDADD = $(libt) $(libcore)
TESTS += t_string t_wstring
EXTRA_DIST += t__string.c
t_string_LDADD = $(libt) $(libcore)
t_wstring_LDADD = $(libt) $(libcore)
endif CRYB_CORE

# libcryb-digest
if CRYB_DIGEST
TESTS += t_digest
t_digest_LDADD =  $(libt) $(libdigest)
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

# libcryb-enc
if CRYB_ENC
TESTS += t_enc
t_enc_LDADD = $(libt) $(libenc)
TESTS += t_rfc3986 t_rfc4648
t_rfc3986_LDADD = $(libt) $(libenc)
t_rfc4648_LDADD = $(libt) $(libenc)
endif CRYB_ENC

# libcryb-hash
if CRYB_HASH
TESTS += t_hash
t_hash_LDADD = $(libt) $(libhash)
TESTS += t_adler t_fletcher t_murmur3_32 t_pearson
t_adler_LDADD = $(libt) $(libhash)
t_fletcher_LDADD = $(libt) $(libhash)
t_murmur3_32_LDADD = $(libt) $(libhash)
t_pearson_LDADD = $(libt) $(libhash)
endif CRYB_HASH

# libcryb-mac
if CRYB_MAC
TESTS += t_mac
t_mac_LDADD = $(libt) $(libmac)
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
TESTS += t_mpi t_mpi_addsub t_mpi_compar t_mpi_gcd t_mpi_muldiv
t_mpi_LDADD = $(libt) $(libmpi)
t_mpi_addsub_LDADD = $(libt) $(libmpi)
t_mpi_compar_LDADD = $(libt) $(libmpi)
t_mpi_gcd_LDADD = $(libt) $(libmpi)
t_mpi_muldiv_LDADD = $(libt) $(libmpi)
noinst_HEADERS += t_mpi.h
endif CRYB_MPI

# libcryb-oath
if CRYB_OATH
TESTS += t_oath
t_oath_LDADD = $(libt) $(liboath)
endif CRYB_OATH

# libcryb-rand
if CRYB_RAND
TESTS += t_rand
t_rand_LDADD = $(libt) $(librand)
endif CRYB_RAND

# libcryb-test
if CRYB_TEST
TESTS += t_test
t_test_LDADD = $(libt) $(libtest)
TESTS += t_malloc
t_malloc_LDADD = $(libt) $(libtest)
endif CRYB_TEST

check_PROGRAMS = $(TESTS)

endif CRYB_TEST
