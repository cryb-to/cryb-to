dnl -*- autoconf -*-
dnl
dnl Copyright (c) 2018 The University of Oslo
dnl All rights reserved.
dnl
dnl Redistribution and use in source and binary forms, with or without
dnl modification, are permitted provided that the following conditions
dnl are met:
dnl 1. Redistributions of source code must retain the above copyright
dnl    notice, this list of conditions and the following disclaimer.
dnl 2. Redistributions in binary form must reproduce the above copyright
dnl    notice, this list of conditions and the following disclaimer in the
dnl    documentation and/or other materials provided with the distribution.
dnl 3. The name of the author may not be used to endorse or promote
dnl    products derived from this software without specific prior written
dnl    permission.
dnl
dnl THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
dnl ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
dnl IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
dnl ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
dnl FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
dnl DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
dnl OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
dnl HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
dnl LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
dnl OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
dnl SUCH DAMAGE.
dnl

m4_define([CRYB_MACROS_VERSION], [0.20180506])

dnl
dnl CRYB_INIT
dnl ---------
dnl
dnl Initialize.
dnl
AC_DEFUN([CRYB_INIT], [
    AC_ARG_ENABLE([all],
        AS_HELP_STRING([--disable-all],
            [disable all components]),
        [enable_all=$enableval],
        [enable_all=yes])
])

dnl
dnl CRYB_LIB_PROVIDE(library-name, [dependencies])
dnl ----------------------------------------------
dnl
dnl Declare a Cryb library that we provide, and its dependencies.
dnl
AC_DEFUN([CRYB_LIB_PROVIDE], [
    m4_define([COMP], m4_toupper([$1]))
    m4_define([comp], m4_tolower([$1]))
    m4_set_add([cryb_provides], comp)
    m4_foreach_w([req], [$2], [m4_set_add([cryb_requires], req)])
    AC_ARG_ENABLE([cryb-]comp,
	AS_HELP_STRING([--enable-cryb-]comp,
	    [build the ]comp[ library]),
        [enable_cryb_]comp[=$enableval],
        [enable_cryb_]comp[=$enable_all])
    if test [x"$enable_cryb_]comp[" = x"yes"] ; then
	AC_DEFINE([HAVE_CRYB_]COMP, [1], [Define to 1 if you have $1])
	AC_SUBST([CRYB_]COMP[_VERSION], [$PACKAGE_VERSION])
        AC_SUBST([CRYB_]COMP[_CFLAGS], [])
        AC_SUBST([CRYB_]COMP[_LIBS],
	    ['\$(top_builddir)/lib/]comp[/libcryb-]comp[.la]')
        AM_CONDITIONAL([HAVE_CRYB_]COMP, [true])
    else
        AX_PKG_CONFIG_CHECK([cryb-]comp, [$PACKAGE_VERSION])
    fi
    AM_CONDITIONAL([CRYB_]COMP, [test [x"$enable_cryb_]comp[" = x"yes"]])
])

dnl
dnl CRYB_LIB_REQUIRE(library-name)
dnl ------------------------------
dnl
dnl Declare a Cryb library that we require.
dnl
AC_DEFUN([CRYB_LIB_REQUIRE], [
    m4_foreach_w([req], [$1], [m4_set_add([cryb_requires], req)])
])

dnl
dnl CRYB_CHECK_DEPENDENCIES
dnl -----------------------
dnl
dnl Verify that all dependencies have been satisfied.
dnl
AC_DEFUN([CRYB_CHECK_DEPENDENCIES], [
    AC_MSG_CHECKING([required Cryb elements])
    m4_set_foreach([cryb_requires], [req], [
        if test [x"$enable_cryb_]req[" != x"yes"] && \
	   test [x"$ax_pc_cv_have_cryb_]req[" != x"yes"] ; then
            AC_MSG_ERROR([missing cryb-]req)
	fi
    ])
    AC_MSG_RESULT([ok])
])

dnl
dnl CRYB_SUMMARY
dnl ------------
dnl
dnl Print a summary of enabled components.
dnl
AC_DEFUN([CRYB_SUMMARY], [
    AS_ECHO([])
    AS_ECHO(["The following Cryb components will be built:"])
    AS_ECHO([])
    m4_set_foreach([cryb_provides], [comp], [
        [printf "%16s: %s\n" ]comp[ ${enable_cryb_]comp[:-no}]
    ])
])
