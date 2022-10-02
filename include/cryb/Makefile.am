crybdir = $(includedir)/cryb

cryb_HEADERS =

if CRYB_CIPHER
cryb_HEADERS += \
	aes.h \
	chacha.h \
	des.h \
	rc4.h \
	salsa.h \
	\
	cipher.h
endif CRYB_CIPHER

if CRYB_CORE
cryb_HEADERS += \
	algorithm.h \
	assert.h \
	bitwise.h \
	core.h \
	ctype.h \
	defs.h \
	endian.h \
	memcpy_s.h \
	memset_s.h \
	strchrnul.h \
	string.h \
	strlcat.h \
	strlcmp.h \
	strlcpy.h \
	types.h \
	wcschrnul.h \
	wcslcat.h \
	wcslcmp.h \
	wcslcpy.h \
	wstring.h \
	\
	to.h
endif CRYB_CORE

if CRYB_CPE
cryb_HEADERS += \
	\
	cpe.h
endif CRYB_CPE

if CRYB_DIGEST
cryb_HEADERS += \
	md.h \
	md2.h \
	md4.h \
	md5.h \
	sha.h \
	sha1.h \
	sha224.h \
	sha256.h \
	sha384.h \
	sha512.h \
	\
	digest.h
endif CRYB_DIGEST

if CRYB_ENC
cryb_HEADERS += \
	rfc3986.h \
	rfc4648.h \
	\
	enc.h
endif CRYB_ENC

if CRYB_HASH
cryb_HEADERS += \
	\
	hash.h
endif CRYB_HASH

if CRYB_MAC
cryb_HEADERS += \
	hmac.h \
	hmac_md5.h \
	hmac_sha1.h \
	hmac_sha224.h \
	hmac_sha256.h \
	hmac_sha384.h \
	hmac_sha512.h \
	\
	mac.h
endif CRYB_MAC

if CRYB_MPI
cryb_HEADERS += \
	\
	mpi.h
endif CRYB_MPI

if CRYB_OATH
cryb_HEADERS += \
	oath_constants.h \
	oath_hotp.h \
	oath_totp.h \
	oath_types.h \
	\
	oath.h
endif CRYB_OATH

if CRYB_RAND
cryb_HEADERS += \
	\
	rand.h
endif CRYB_RAND

if CRYB_TEST
cryb_HEADERS += \
	\
	coverage.h \
	test.h
endif CRYB_TEST

noinst_HEADERS = \
	impl.h \
	nocoverage.h
