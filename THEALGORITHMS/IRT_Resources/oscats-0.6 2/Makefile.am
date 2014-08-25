## Makefile.am -- Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src bindings doc
DIST_SUBDIRS = $(SUBDIRS) examples

EXTRA_DIST = autogen.sh config/getsp.java config/getsp.class oscats.pc.in

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = oscats.pc
