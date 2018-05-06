ACLOCAL_AMFLAGS = -I m4

SUBDIRS = include lib t

EXTRA_DIST = \
	CREDITS \
	HISTORY \
	INSTALL \
	LICENSE \
	README \
	RELNOTES \
	autogen.sh \
	m4/ax_gcc_builtin.m4 \
	m4/ax_pkg_config.m4 \
	tools/coverage.sh.in \
	tools/setprops.sh

if CRYB_CORE
aclocaldir = $(datarootdir)/aclocal
aclocal_DATA = m4/cryb_to.m4
endif CRYB_CORE
