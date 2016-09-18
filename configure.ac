AC_PREREQ([2.63])
AC_INIT([cryb.to], [0.20160918], [des@des.no], [cryb-to], [http://cryb.to/])
AC_CONFIG_SRCDIR([include/cryb/core.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz])
AM_CONFIG_HEADER([include/config.h])

############################################################################
#
# Toolchain
#

# C compiler and features
AC_LANG(C)
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CPP
AC_GNU_SOURCE
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_BIGENDIAN
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_INTMAX_T
AC_TYPE_INTPTR_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_TYPE_UINTMAX_T
AC_TYPE_UINTPTR_T

# libtool
LT_PREREQ([2.2.6])
LT_INIT()

# other programs
AC_PROG_INSTALL

############################################################################
#
# Headers and functions
#

AC_CHECK_HEADERS([endian.h sys/endian.h])
AX_GCC_BUILTIN([__builtin_bswap16])
AX_GCC_BUILTIN([__builtin_bswap32])
AX_GCC_BUILTIN([__builtin_bswap64])
AC_CHECK_DECLS([
    bswap16, bswap32, bswap64,
    be16enc, be16dec, le16enc, le16dec,
    be32enc, be32dec, le32enc, le32dec,
    be64enc, be64dec, le64enc, le64dec,
    htobe16, be16toh, htole16, le16toh,
    htobe32, be32toh, htole32, le32toh,
    htobe64, be64toh, htole64, le64toh,
    nothing
], [], [], [[
#if HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif
#if HAVE_ENDIAN_H
#include <endian.h>
#endif
]])
AC_CHECK_FUNCS([strlcat strlcmp strlcpy])
AC_CHECK_FUNCS([wcslcat wcslcmp wcslcpy])

# For tracing allocations in unit tests
AC_CHECK_HEADERS([sys/uio.h sys/ktrace.h], [], [], [[
#include <sys/param.h>
#if HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
]])
AC_CHECK_FUNCS([utrace])

# C11 features
# XXX our version has an incorrect prototype due to the lack of a test
# for the existence of rsize_t and RSIZE_MAX.
AC_CHECK_FUNCS([memset_s])

############################################################################
#
# Extra libraries
#

save_LIBS="${LIBS}"
LIBS=""
AC_SEARCH_LIBS([clock_gettime], [rt])
RT_LIBS="${LIBS}"
LIBS="${save_LIBS}"
AC_SUBST(RT_LIBS)

############################################################################
#
# Build options
#

# Documentation
AC_ARG_ENABLE([doc],
    AC_HELP_STRING([--disable-doc],
	[do not build the documentation]),
    [with_doc=$enableval])
AM_CONDITIONAL([WITH_DOC], [test x"$with_doc" != x"no"])

############################################################################
#
# Debugging
#

# OpenSSL versions of the unit tests for comparison
AC_ARG_ENABLE([openssl-tests],
    AC_HELP_STRING([--enable-openssl-tests],
	[build unit tests with OpenSSL support]))
AM_CONDITIONAL([OPENSSL_TESTS], [test x"$enable_openssl_tests" = x"yes"])

# RSAREF versions of the unit tests for comparison
AC_ARG_ENABLE([rsaref-tests],
    AC_HELP_STRING([--enable-rsaref-tests],
	[build unit tests enable RSAREF support]))
AM_CONDITIONAL([RSAREF_TESTS], [test x"enable_rsaref_tests" = x"yes"])

# Developer-friendly compiler flags
AC_ARG_ENABLE([developer-warnings],
    AS_HELP_STRING([--enable-developer-warnings],
	[enable strict warnings (default is NO)]),
    [CFLAGS="${CFLAGS} -Wall -Wextra -Wcast-qual -Wshadow -Wno-unused-result"])
AC_ARG_ENABLE([debugging-symbols],
    AS_HELP_STRING([--enable-debugging-symbols],
	[enable debugging symbols (default is NO)]),
    [CFLAGS="${CFLAGS} -O0 -g -fno-inline"])
AC_ARG_ENABLE([werror],
    AS_HELP_STRING([--enable-werror],
	[use -Werror (default is NO)]),
    [CFLAGS="${CFLAGS} -Werror"])

############################################################################
#
# Components
#

# Everything
AC_ARG_ENABLE([all],
    AC_HELP_STRING([--disable-all],
        [disable all libraries and tools]),
    [enable_all=$enableval],
    [enable_all=yes])

# Ciphers
elements="$elements cipher"
requires="$requires cipher:core"
AC_ARG_ENABLE([cryb-cipher],
    AC_HELP_STRING([--enable-cryb-cipher],
	[build the cipher library]),
    [enable_cryb_cipher=$enableval],
    [enable_cryb_cipher=$enable_all])
AM_CONDITIONAL([CRYB_CIPHER], [test x"$enable_cryb_cipher" = x"yes"])

# CPE
elements="$elements cpe"
requires="$requires cpe:core"
AC_ARG_ENABLE([cryb-cpe],
    AC_HELP_STRING([--enable-cryb-cpe],
	[build the CPE library]),
    [enable_cryb_cpe=$enableval],
    [enable_cryb_cpe=$enable_all])
AM_CONDITIONAL([CRYB_CPE], [test x"$enable_cryb_cpe" = x"yes"])

# Core
elements="$elements core"
AC_ARG_ENABLE([cryb-core],
    AC_HELP_STRING([--enable-cryb-core],
	[build the core library]),
    [enable_cryb_core=$enableval],
    [enable_cryb_core=$enable_all])
AM_CONDITIONAL([CRYB_CORE], [test x"$enable_cryb_core" = x"yes"])

# Message digests
elements="$elements digest"
requires="$requires digests:core"
AC_ARG_ENABLE([cryb-digest],
    AC_HELP_STRING([--enable-cryb-digest],
	[build the message digest library]),
    [enable_cryb_digest=$enableval],
    [enable_cryb_digest=$enable_all])
AM_CONDITIONAL([CRYB_DIGEST], [test x"$enable_cryb_digest" = x"yes"])

# Encodings
elements="$elements enc"
requires="$requires env:core"
AC_ARG_ENABLE([cryb-enc],
    AC_HELP_STRING([--enable-cryb-enc],
	[build the encoding library]),
    [enable_cryb_enc=$enableval],
    [enable_cryb_enc=$enable_all])
AM_CONDITIONAL([CRYB_ENC], [test x"$enable_cryb_enc" = x"yes"])

# Non-cryptographic hashes
elements="$elements hash"
requires="$requires hash:core"
AC_ARG_ENABLE([cryb-hash],
    AC_HELP_STRING([--enable-cryb-hash],
	[build the non-cryptographic hash library]),
    [enable_cryb_hash=$enableval],
    [enable_cryb_hash=$enable_all])
AM_CONDITIONAL([CRYB_HASH], [test x"$enable_cryb_hash" = x"yes"])

# Message authentication codes
elements="$elements mac"
requires="$requires mac:core"
AC_ARG_ENABLE([cryb-mac],
    AC_HELP_STRING([--enable-cryb-mac],
	[build the message authentication code library]),
    [enable_cryb_mac=$enableval],
    [enable_cryb_mac=$enable_all])
AM_CONDITIONAL([CRYB_MAC], [test x"$enable_cryb_digest" = x"yes"])

# Multiple-precision integers
elements="$elements mpi"
requires="$requires mpi:core"
AC_ARG_ENABLE([cryb-mpi],
    AC_HELP_STRING([--enable-cryb-mpi],
	[build the multiple-precision math library]),
    [enable_cryb_mpi=$enableval],
    [enable_cryb_mpi=$enable_all])
AM_CONDITIONAL([CRYB_MPI], [test x"$enable_cryb_digest" = x"yes"])

# OATH
elements="$elements oath"
requires="$requires oath:core oath:digest oath:mac oath:enc"
AC_ARG_ENABLE([cryb-oath],
    AC_HELP_STRING([--enable-cryb-oath],
	[build the OATH library]),
    [enable_cryb_oath=$enableval],
    [enable_cryb_oath=$enable_all])
AM_CONDITIONAL([CRYB_OATH], [test x"$enable_cryb_oath" = x"yes"])

# Pseudo-randomness
elements="$elements rand"
requires="$requires rand:core"
AC_ARG_ENABLE([cryb-rand],
    AC_HELP_STRING([--enable-cryb-rand],
	[build the pseudo-randomness library]),
    [enable_cryb_rand=$enableval],
    [enable_cryb_rand=$enable_all])
AM_CONDITIONAL([CRYB_RAND], [test x"$enable_cryb_rand" = x"yes"])

# Test framework
elements="$elements test"
requires="$requires test:core"
AC_ARG_ENABLE([cryb-test],
    AC_HELP_STRING([--enable-cryb-test],
	[build the test framework]),
    [enable_cryb_test=$enableval],
    [enable_cryb_test=$enable_all])
AM_CONDITIONAL([CRYB_TEST], [test x"$enable_cryb_test" = x"yes"])

# Check dependencies
AC_MSG_CHECKING([dependencies])
for req in $requires ; do
    lhs=${req%:*}
    lhs_ena=`eval echo \\\$enable_cryb_$lhs`
    if test x"$lhs_ena" = x"yes" ; then
        rhs=${req#*:}
    	rhs_ena=`eval echo \\\$enable_cryb_$rhs`
	if test x"$rhs_ena" != x"yes" ; then
	    AC_MSG_ERROR([cryb-$lhs requires cryb-$rhs])
	fi
    fi
done
AC_MSG_RESULT([ok])

############################################################################
#
# Output
#

AC_CONFIG_FILES([
    Makefile
    include/Makefile
    include/cryb/Makefile
    lib/Makefile
    lib/cipher/Makefile
    lib/core/Makefile
    lib/cpe/Makefile
    lib/digest/Makefile
    lib/enc/Makefile
    lib/hash/Makefile
    lib/mac/Makefile
    lib/mpi/Makefile
    lib/oath/Makefile
    lib/rand/Makefile
    lib/rsaref/Makefile
    lib/test/Makefile
    t/Makefile
])
AC_CONFIG_FILES([mkpkgng], [chmod +x mkpkgng])
AC_CONFIG_FILES([tools/coverage.sh], [chmod +x tools/coverage.sh])
AC_OUTPUT

echo
echo The following Cryb components will be built:
echo
for elem in $elements ; do
    enable=`eval echo \\\$enable_cryb_$elem`
    printf "%16s: %s\n" $elem ${enable:-no}
done
echo
