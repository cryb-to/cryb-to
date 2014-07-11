AC_PREREQ([2.63])
AC_REVISION([$Cryb$])
AC_INIT([cryb.to], [trunk], [des@des.no], [crybto], [http://cryb.to/])
AC_CONFIG_SRCDIR([include/cryb/to.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([include/config.h])

# C compiler and features
AC_LANG(C)
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CPP
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_BIGENDIAN

# libtool
LT_PREREQ([2.2.6])
LT_INIT()

# other programs
AC_PROG_INSTALL

# OpenSSL versions of the unit tests for comparison
AC_ARG_WITH([openssl],
    AC_HELP_STRING([--with-openssl],
	[build unit tests with OpenSSL support]),
    [],
    [with_openssl=no])
AM_CONDITIONAL([WITH_OPENSSL], [test x"$with_openssl" = x"yes"])

# RSAREF versions of the unit tests for comparison
AC_ARG_WITH([rsaref],
    AC_HELP_STRING([--with-rsaref],
	[build unit tests with RSAREF support]),
    [],
    [with_rsaref=no])
AM_CONDITIONAL([WITH_RSAREF], [test x"$with_rsaref" = x"yes"])

# Documentation
AC_ARG_WITH([doc],
    AC_HELP_STRING([--without-doc],
	[do not build the documentation]),
    [],
    [with_doc=yes])
AM_CONDITIONAL([WITH_DOC], [test x"$with_doc" = x"yes"])

# Message digests
AC_ARG_WITH([digest],
    AC_HELP_STRING([--without-digest],
	[do not build the message digest library and tools]),
    [],
    [with_digest=yes])
AM_CONDITIONAL([WITH_DIGEST], [test x"$with_digest" = x"yes"])

# Message authentication codes
AC_ARG_WITH([mac],
    AC_HELP_STRING([--without-mac],
	[do not build the message authentication code library and tools]),
    [],
    [with_mac=yes])
AM_CONDITIONAL([WITH_MAC], [test x"$with_digest" = x"yes"])

# OATH
AC_ARG_WITH([oath],
    AC_HELP_STRING([--without-oath],
	[do not build the OATH library and tools]),
    [],
    [with_oath=yes])
AM_CONDITIONAL([WITH_OATH], [test x"$with_oath" = x"yes"])

# Developer-friendly compiler flags
AC_ARG_ENABLE([developer-warnings],
    AS_HELP_STRING([--enable-developer-warnings],
	[enable strict warnings (default is NO)]),
    [CFLAGS="${CFLAGS} -Wall -Wextra"])
AC_ARG_ENABLE([debugging-symbols],
    AS_HELP_STRING([--enable-debugging-symbols],
	[enable debugging symbols (default is NO)]),
    [CFLAGS="${CFLAGS} -O0 -g -fno-inline"])
AC_ARG_ENABLE([werror],
    AS_HELP_STRING([--enable-werror],
	[use -Werror (default is NO)]),
    [CFLAGS="${CFLAGS} -Werror"])

AC_CHECK_HEADERS([endian.h sys/endian.h])

AC_CHECK_FUNCS([strlcat strlcmp strlcpy])

AC_CONFIG_FILES([
    Makefile
    include/Makefile
    include/cryb/Makefile
    lib/Makefile
    lib/core/Makefile
    lib/digest/Makefile
    lib/enc/Makefile
    lib/mac/Makefile
    lib/oath/Makefile
    lib/rand/Makefile
    lib/rsaref/Makefile
    t/Makefile
])
AC_OUTPUT
