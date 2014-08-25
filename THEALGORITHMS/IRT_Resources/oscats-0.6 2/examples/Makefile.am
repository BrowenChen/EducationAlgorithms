## Makefile.am -- Process this file with automake to produce Makefile.in

EXTRA_DIST = README		\
		ex01.py ex01.pl ex01.php ex01.java ex01_java.m	\
		ex02.py ex02.pl ex02.php ex02.java ex02_java.m	\
		ex03.py \
		ex03-items.dat ex03-person.dat			\
		ex04.py

bin_PROGRAMS = ex01 ex02 ex03 # ex04
ex01_CFLAGS = -I$(top_srcdir)/src/liboscats $(GLIB_CFLAGS) $(GSL_CFLAGS)
ex01_LDADD = $(top_builddir)/src/liboscats/liboscats.la
ex02_CFLAGS = -I$(top_srcdir)/src/liboscats $(GLIB_CFLAGS) $(GSL_CFLAGS)
ex02_LDADD = $(top_builddir)/src/liboscats/liboscats.la
ex03_CFLAGS = -I$(top_srcdir)/src/liboscats $(GLIB_CFLAGS) $(GSL_CFLAGS)
ex03_LDADD = $(top_builddir)/src/liboscats/liboscats.la
ex04_CFLAGS = -I$(top_srcdir)/src/liboscats $(GLIB_CFLAGS) $(GSL_CFLAGS)
ex04_LDADD = $(top_builddir)/src/liboscats/liboscats.la
