#! /bin/sh
# Run this to generate all the initial makefiles, etc.

glibtoolize || exit 1
gtkdocize || exit 1
aclocal -Im4 \
&& automake --gnu --add-missing \
&& autoconf
