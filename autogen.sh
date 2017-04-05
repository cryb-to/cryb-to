#!/bin/sh
set -e
libtoolize --copy --force
aclocal -I m4
autoheader
automake --add-missing --copy --foreign
autoconf
