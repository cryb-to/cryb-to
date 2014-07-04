#!/bin/sh
#
# $Cryb$
#

libtoolize --copy --force
aclocal -I m4
autoheader
automake -a -c --foreign
autoconf
