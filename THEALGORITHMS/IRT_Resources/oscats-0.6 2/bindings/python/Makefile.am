## Makefile.am -- Process this file with automake to produce Makefile.in

DIST_SUBDIRS = codegen

INCLUDES = $(PYTHON_INCLUDES) $(PYGOBJECT_CFLAGS) -I$(top_srcdir)/src/liboscats

if WINDOWS
LDFLAGS += -no-undefined
SHREXT = -shrext ".pyd"
WIN_MODULE = oscats.pyd
else
SHREXT =
WIN_MODULE =
endif

pyexec_LTLIBRARIES = oscats.la
oscats_la_CFLAGS = $(GLIB_CFLAGS) $(GSL_CFLAGS)
oscats_la_LDFLAGS = -module $(SHREXT) -avoid-version -export-symbols-regex initoscats
oscats_la_LIBADD = $(top_builddir)/src/liboscats/liboscats.la $(PYTHON_LIBS)
oscats_la_SOURCES = oscatsmodule.c pyalgorithm.c
nodist_oscats_la_SOURCES = oscats.c
oscats.c: oscats.defs oscats.override
pyexec_DATA = $(WIN_MODULE)
CLEANFILES = oscats.c

EXTRA_DIST = LICENSE oscats.override oscats-py.defs pyalgorithm.h

oscats.defs: ../oscats-common.defs oscats-py.defs
	(cd $(srcdir) \
	 && cat ../oscats-common.defs oscats-py.defs > oscats.defs)

oscats.pyd: oscats.la

#oscats-py.defs:
#	python codegen/h2defs.py *.h

.defs.c:
	(cd $(srcdir) \
	 && $(PYTHON) codegen/codegen.py --override $*.override \
	     --prefix py$* $*.defs $*-py.defs) > gen-$*.c \
	 && cp gen-$*.c $*.c \
	 && rm -f gen-$*.c

