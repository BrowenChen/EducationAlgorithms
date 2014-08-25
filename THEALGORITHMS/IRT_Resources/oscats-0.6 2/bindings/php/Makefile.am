## Makefile.am -- Process this file with automake to produce Makefile.in

## Installation note: libphp_oscats.so must be put in:
##     $(PHP_EXTENSIONS_DIR)/php_oscats.so

DIST_SUBDIRS = generator
EXTRA_DIST = LICENSE oscats.overrides php_gtk.h php_gtk_module.h php_oscats.c

if WINDOWS
WIN_PHP_CFLAGS = -DPHP_WIN32=1 -DZEND_WIN32=1 -DZEND_DEBUG=0
WIN_PHP_LDFLAGS = -L$(PHP_EXTENSIONS_DIR) -lphp_gtk2 -lphp5
PHP_LIB_NAME = libphp_oscats-0
LDFLAGS += -no-undefined
SHREXT = dll
else
WIN_PHP_CFLAGS =
WIN_PHP_LDFLAGS =
PHP_LIB_NAME = libphp_oscats
SHREXT = so
endif

lib_LTLIBRARIES = libphp_oscats.la
libphp_oscats_la_SOURCES = php_oscats.c
nodist_libphp_oscats_la_SOURCES = gen_oscats.c 
libphp_oscats_la_CFLAGS = $(PHP_CFLAGS) $(WIN_PHP_CFLAGS) $(GLIB_CFLAGS) $(GTK_CFLAGS) -I$(srcdir) -I$(top_srcdir)/src/liboscats -DHAVE_PHP_GTK -DCOMPILE_DL_OSCATS
libphp_oscats_la_LIBADD = $(GLIB_LIBS) $(top_builddir)/src/liboscats/liboscats.la
libphp_oscats_la_LDFLAGS = -module $(WIN_PHP_LDFLAGS)

gen_oscats.c : $(srcdir)/../oscats-common.defs $(srcdir)/oscats.overrides
	cp $(srcdir)/../oscats-common.defs oscats.defs
	$(PHP) $(srcdir)/generator/generator.php 			\
	       -o $(srcdir)/oscats.overrides				\
	       -f gen_oscats.c -p oscats  oscats.defs			\
	&& grep -h "^PHP_GTK_EXPORT_CE" $@ | 				\
	   sed -e "s!^!extern !" >> gen_oscats.h

install-exec-local: libphp_oscats.la
	cp .libs/$(PHP_LIB_NAME).$(SHREXT) $(PHP_EXTENSIONS_DIR)/php_oscats.$(SHREXT)

uninstall-local:
	rm -f $(PHP_EXTENSIONS_DIR)/php_oscats.so
