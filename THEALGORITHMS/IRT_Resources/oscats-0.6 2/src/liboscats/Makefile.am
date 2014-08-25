## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES = -DG_LOG_DOMAIN=\"OSCATS\"

if WINDOWS
LDFLAGS += -no-undefined
endif

lib_LTLIBRARIES = liboscats.la
liboscats_la_SOURCES = bitarray.c gsl.c random.c space.c point.c        \
			model.c administrand.c item.c			\
			itembank.c examinee.c marshal.c test.c		\
			algorithm.c covariates.c integrate.c		\
			models/l1p.c					\
			models/l2p.c					\
			models/l3p.c					\
			models/nominal.c				\
			models/gr.c					\
			models/gpc.c					\
			models/pc.c					\
			models/hlgr.c					\
			models/dina.c					\
			models/nida.c					\
			algorithms/pick_rand.c				\
			algorithms/chooser.c				\
			algorithms/stratify.c				\
			algorithms/astrat.c				\
			algorithms/closest_diff.c			\
			algorithms/max_fisher.c				\
			algorithms/max_kl.c				\
			algorithms/simulate.c				\
			algorithms/exposure_counter.c			\
			algorithms/class_rates.c			\
			algorithms/estimate.c				\
			algorithms/fixed_length.c
liboscats_la_CFLAGS = $(GLIB_CFLAGS) $(GSL_CFLAGS) -Wall -Werror
liboscats_la_LIBADD = $(GLIB_LIBS) $(GSL_LIBS)
liboscatsincludedir = $(includedir)/liboscats
liboscatsinclude_HEADERS = oscats.h bitarray.h gsl.h random.h space.h point.h  \
			   model.h administrand.h item.h		\
			   itembank.h examinee.h marshal.h test.h       \
			   algorithm.h algorithms.h models.h		\
			   covariates.h integrate.h
liboscatsmodelsincludedir = $(liboscatsincludedir)/models
liboscatsmodelsinclude_HEADERS = models/l1p.h				\
			models/l2p.h					\
			models/l3p.h					\
			models/nominal.h				\
			models/gr.h					\
			models/gpc.h					\
			models/pc.h					\
			models/hlgr.h					\
			models/dina.h					\
			models/nida.h
liboscatsalgincludedir = $(liboscatsincludedir)/algorithms
liboscatsalginclude_HEADERS = algorithms/pick_rand.h                    \
			algorithms/chooser.h				\
			algorithms/stratify.h				\
			algorithms/astrat.h				\
			algorithms/closest_diff.h			\
			algorithms/max_fisher.h				\
			algorithms/max_kl.h				\
			algorithms/simulate.h				\
			algorithms/exposure_counter.h			\
			algorithms/class_rates.h			\
			algorithms/estimate.h				\
			algorithms/fixed_length.h

enum_headers = space.h

oscats-enum-types.h: $(enum_headers) Makefile.am
	glib-mkenums  \
		--fhead "#ifndef __OSCATS_ENUM_TYPES_H__\n#define __OSCATS_ENUM_TYPES_H__\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
		--fprod "/* enumerations from \"@filename@\" */\n" \
		--vhead "#define OSCATS_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\nGType @enum_name@_get_type (void);\n" 	\
		--ftail "G_END_DECLS\n#endif /* __OSCATS_ENUM_TYPES_H__ */" \
		$(enum_headers) > oscats-enum-types.h

oscats-enum-types.c: oscats-enum-types.h Makefile.am
	glib-mkenums  \
		--fhead "#include <oscats.h>" \
		--fprod "\n/* enumerations from \"@filename@\" */" \
		--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" 	\
		--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
		--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(enum_headers) > oscats-enum-types.c
