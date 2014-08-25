## Makefile.am -- Process this file with automake to produce Makefile.in

CLEANFILES = generator.jar glib.jar oscats.jar

if WINDOWS
CP_SEP = \;
LDFLAGS += -no-undefined -Wl,--kill-at
else
CP_SEP = :
endif

generator.jar: generator/generator.files $(GENERATOR_FILES)
	if test ! -d .libs/generator; then mkdir -p .libs/generator; fi
	if test ! -d generator ; then ln -s $(srcdir)/generator . ; fi
	$(JAVAC) @$(srcdir)/generator/generator.files -d .libs/generator
	(cd .libs/generator && $(JAR) cf ../../generator.jar * )

glib.jar: $(BINDINGS_FILES) $(GLIB_FILES)
	if test ! -d .libs/glib; then mkdir -p .libs/glib; fi
	if test ! -d .libs/include; then mkdir -p .libs/include; fi
	if test ! -d oscats; then ln -s $(srcdir)/oscats . ; fi
	$(JAVAC) -d .libs/glib oscats/bindings/*java oscats/glib/*java
	$(JAVAH) -jni -d .libs/include -classpath .libs/glib \
		`cat oscats/glib.pkgs`
	cp oscats/glib/glib.typeMapping .libs/glib
	(cd .libs/glib && $(JAR) cf ../../glib.jar * )

oscats.defs: ../oscats-common.defs oscats-java.defs
	cat $(srcdir)/../oscats-common.defs $(srcdir)/oscats-java.defs > oscats.defs

defs/GBitArray.defs: oscats.defs
	if test ! -d defs; then mkdir defs; fi
	rm -f defs/*.defs
	(cd defs &&		\
	  $(PYTHON) ../$(srcdir)/generator/demux.py \
	            ../oscats.defs > ../missing.txt)

oscats.jar: generator.jar glib.jar defs/GBitArray.defs oscats/oscats.pkgs $(OSCATS_FILES)
	if test ! -d .libs/oscats; then mkdir -p .libs/oscats; fi
	if test ! -d oscats; then ln -s $(srcdir)/oscats . ; fi
	if test ! -d generated ; then mkdir generated ; fi
	$(JAVA) -classpath generator.jar BindingsGenerator
	$(JAVAC) -d .libs/oscats -classpath glib.jar	\
		oscats/*.java generated/oscats/*.java
	$(JAVAH) -jni -d .libs/include -classpath .libs/oscats$(CP_SEP)glib.jar \
		`cat oscats/oscats.pkgs`
	cp generated/typeMapping.properties .libs/oscats/oscats.typeMapping
	(cd .libs/oscats && $(JAR) cf ../../oscats.jar * )

lib_LTLIBRARIES = libglibjni.la liboscatsjni.la

BUILT_SOURCES = glib.jar oscats.jar

libglibjni_la_DEPENDENCIES = glib.jar
libglibjni_la_SOURCES = oscats/bindings/Environment.c	\
	oscats/bindings/Internationalization.c	\
	oscats/bindings/Plumbing.c	\
	oscats/bindings/Time.c	\
	oscats/glib/GBoxed.c	\
	oscats/glib/GObject.c	\
	oscats/glib/GValue.c	\
	oscats/glib/GlibMisc.c	\
	oscats/glib/Plumbing-glib.c \
	jni/bindings_java_convert.c \
	jni/bindings_java_memory.c \
	jni/bindings_java_signal.c \
	jni/bindings_java_threads.c \
	jni/bindings_java_type.c \
	jni/bindings_java_util.c
libglibjni_la_CFLAGS = $(GLIB_CFLAGS) -I.libs/include $(JAVA_CPPFLAGS0) \
			-I$(srcdir)/jni -I$(top_srcdir)/src/liboscats
#glibjni_la_LDFLAGS = -module
libglibjni_la_LIBADD = $(GLIB_LIBS) -lgthread-2.0 $(JAVA_LIBS0)

liboscatsjni_la_DEPENDENCIES = oscats.jar libglibjni.la
liboscatsjni_la_SOURCES = oscats/OscatsAdministrandOverride.c \
			oscats/OscatsExamineeOverride.c 
nodist_liboscatsjni_la_SOURCES = $(OSCATS_GENERATED_SOURCES)
liboscatsjni_la_CFLAGS = $(GLIB_CFLAGS) -I.libs/include $(JAVA_CPPFLAGS0) \
			-I$(srcdir)/jni -I$(top_srcdir)/src/liboscats
#oscatsjni_la_LDFLAGS = -module
liboscatsjni_la_LIBADD = $(top_builddir)/src/liboscats/liboscats.la \
			libglibjni.la

EXTRA_DIST = LICENSE jni/bindings_java.h jni/oscats_glib_GlibMisc.h \
	generator/generator.files generator/definitions.py	\
	generator/demux.py generator/defsparser.py  generator/scmexpr.py \
	$(GENERATOR_FILES) $(BINDINGS_FILES) $(GLIB_FILES) \
	oscats/glib/glib.typeMapping oscats/glib.pkgs		\
	oscats-java.defs oscats/oscats.pkgs $(OSCATS_FILES)

clean-local:
	rm -rf defs generated

GENERATOR_FILES = generator/BindingsGenerator.java  \
	generator/com/operationaldynamics/codegen/AccessorGenerator.java  \
	generator/com/operationaldynamics/codegen/ArrayThing.java  \
	generator/com/operationaldynamics/codegen/BlacklistedThing.java  \
	generator/com/operationaldynamics/codegen/BoxedGenerator.java  \
	generator/com/operationaldynamics/codegen/BoxedThing.java  \
	generator/com/operationaldynamics/codegen/ConstantArrayThing.java  \
	generator/com/operationaldynamics/codegen/ConstructorGenerator.java  \
	generator/com/operationaldynamics/codegen/EntityGenerator.java  \
	generator/com/operationaldynamics/codegen/EntityThing.java  \
	generator/com/operationaldynamics/codegen/EnumGenerator.java  \
	generator/com/operationaldynamics/codegen/EnumThing.java  \
	generator/com/operationaldynamics/codegen/FixmeThing.java  \
	generator/com/operationaldynamics/codegen/FlagsGenerator.java  \
	generator/com/operationaldynamics/codegen/FlagsThing.java  \
	generator/com/operationaldynamics/codegen/FunctionGenerator.java  \
	generator/com/operationaldynamics/codegen/FundamentalArrayThing.java  \
	generator/com/operationaldynamics/codegen/FundamentalThing.java  \
	generator/com/operationaldynamics/codegen/Generator.java  \
	generator/com/operationaldynamics/codegen/GErrorThing.java  \
	generator/com/operationaldynamics/codegen/GetterGenerator.java  \
	generator/com/operationaldynamics/codegen/GListThing.java  \
	generator/com/operationaldynamics/codegen/InterfaceGenerator.java  \
	generator/com/operationaldynamics/codegen/InterfaceThing.java  \
	generator/com/operationaldynamics/codegen/MethodGenerator.java  \
	generator/com/operationaldynamics/codegen/ObjectGenerator.java  \
	generator/com/operationaldynamics/codegen/ObjectThing.java  \
	generator/com/operationaldynamics/codegen/ProxiedArrayThing.java  \
	generator/com/operationaldynamics/codegen/ProxiedThing.java  \
	generator/com/operationaldynamics/codegen/SetterGenerator.java  \
	generator/com/operationaldynamics/codegen/StringArrayThing.java  \
	generator/com/operationaldynamics/codegen/StringThing.java  \
	generator/com/operationaldynamics/codegen/Thing.java  \
	generator/com/operationaldynamics/codegen/TypedefEnumThing.java  \
	generator/com/operationaldynamics/codegen/TypedefFundamentalThing.java  \
	generator/com/operationaldynamics/codegen/TypeGenerator.java  \
	generator/com/operationaldynamics/codegen/ValueThing.java  \
	generator/com/operationaldynamics/codegen/VirtualGenerator.java  \
	generator/com/operationaldynamics/defsparser/AccessorBlock.java  \
	generator/com/operationaldynamics/defsparser/Block.java  \
	generator/com/operationaldynamics/defsparser/BoxedBlock.java  \
	generator/com/operationaldynamics/defsparser/DefsLineNumberReader.java  \
	generator/com/operationaldynamics/defsparser/DefsParseException.java  \
	generator/com/operationaldynamics/defsparser/DefsParser.java  \
	generator/com/operationaldynamics/defsparser/EntityBlock.java  \
	generator/com/operationaldynamics/defsparser/EnumBlock.java  \
	generator/com/operationaldynamics/defsparser/FlagsBlock.java  \
	generator/com/operationaldynamics/defsparser/FunctionBlock.java  \
	generator/com/operationaldynamics/defsparser/GetterBlock.java  \
	generator/com/operationaldynamics/defsparser/InterfaceBlock.java  \
	generator/com/operationaldynamics/defsparser/MethodBlock.java  \
	generator/com/operationaldynamics/defsparser/ObjectBlock.java  \
	generator/com/operationaldynamics/defsparser/SetterBlock.java  \
	generator/com/operationaldynamics/defsparser/TypeBlock.java  \
	generator/com/operationaldynamics/defsparser/UnnecessaryCodeException.java  \
	generator/com/operationaldynamics/defsparser/VirtualBlock.java  \
	generator/com/operationaldynamics/driver/DefsFile.java  \
	generator/com/operationaldynamics/driver/ImproperDefsFileException.java

BINDINGS_FILES = oscats/bindings/BlacklistedMethodError.java \
	oscats/bindings/Constant.java \
	oscats/bindings/Debug.java \
	oscats/bindings/DoubleConstant.java \
	oscats/bindings/Environment.java \
	oscats/bindings/FIXME.java \
	oscats/bindings/FatalError.java \
	oscats/bindings/Flag.java \
	oscats/bindings/Internationalization.java \
	oscats/bindings/Plumbing.java \
	oscats/bindings/Pointer.java \
	oscats/bindings/Proxy.java \
	oscats/bindings/Time.java \
	oscats/bindings/Version.java

GLIB_FILES = oscats/glib/Boxed.java \
	oscats/glib/FatalError.java \
	oscats/glib/GBoxed.java \
	oscats/glib/GObject.java \
	oscats/glib/GValue.java \
	oscats/glib/Glib.java \
	oscats/glib/GlibException.java \
	oscats/glib/Object.java \
	oscats/glib/Plumbing.java \
	oscats/glib/Signal.java \
	oscats/glib/Value.java \
	oscats/glib/GlibMisc.java

OSCATS_FILES = oscats/Plumbing.java \
	oscats/Random.java	   \
	oscats/AlgAstrat.java \
	oscats/AlgClosestDiff.java \
	oscats/AlgEstimate.java \
	oscats/AlgExposureCounter.java \
	oscats/AlgClassRates.java \
	oscats/AlgFixedLength.java \
	oscats/AlgPickRand.java \
	oscats/AlgChooser.java \
	oscats/AlgMaxFisher.java \
	oscats/AlgMaxKl.java \
	oscats/AlgSimulate.java \
	oscats/AlgStratify.java \
	oscats/Algorithm.java \
	oscats/BitArray.java \
	oscats/Examinee.java \
	oscats/OscatsExamineeOverride.java \
	oscats/Covariates.java \
	oscats/GslMatrix.java \
	oscats/GslPermutation.java \
	oscats/GslVector.java \
	oscats/Integrate.java \
	oscats/Space.java \
	oscats/Point.java \
	oscats/Model.java \
	oscats/Administrand.java \
	oscats/OscatsAdministrandOverride.java \
	oscats/Item.java \
	oscats/ItemBank.java \
	oscats/ModelL1p.java \
	oscats/ModelL2p.java \
	oscats/ModelL3p.java \
	oscats/ModelNominal.java \
	oscats/ModelGr.java \
	oscats/ModelHetlgr.java \
	oscats/ModelGpc.java \
	oscats/ModelPc.java \
	oscats/ModelDina.java \
	oscats/ModelNida.java \
	oscats/Test.java

OSCATS_GENERATED_SOURCES = generated/oscats/GBitArray.c \
	generated/oscats/GGslMatrix.c \
	generated/oscats/GGslPermutation.c \
	generated/oscats/GGslVector.c \
	generated/oscats/OscatsIntegrate.c \
	generated/oscats/OscatsAlgAstrat.c \
	generated/oscats/OscatsAlgClosestDiff.c \
	generated/oscats/OscatsAlgEstimate.c \
	generated/oscats/OscatsAlgExposureCounter.c \
	generated/oscats/OscatsAlgClassRates.c \
	generated/oscats/OscatsAlgFixedLength.c \
	generated/oscats/OscatsAlgPickRand.c \
	generated/oscats/OscatsAlgChooser.c \
	generated/oscats/OscatsAlgMaxFisher.c \
	generated/oscats/OscatsAlgMaxKl.c \
	generated/oscats/OscatsAlgSimulate.c \
	generated/oscats/OscatsAlgStratify.c \
	generated/oscats/OscatsAlgorithm.c \
	generated/oscats/OscatsExaminee.c \
	generated/oscats/OscatsCovariates.c \
	generated/oscats/OscatsSpace.c \
	generated/oscats/OscatsPoint.c \
	generated/oscats/OscatsModel.c \
	generated/oscats/OscatsModelL1p.c \
	generated/oscats/OscatsModelL2p.c \
	generated/oscats/OscatsModelL3p.c \
	generated/oscats/OscatsModelNominal.c \
	generated/oscats/OscatsModelGr.c \
	generated/oscats/OscatsModelHetlgr.c \
	generated/oscats/OscatsModelGpc.c \
	generated/oscats/OscatsModelPc.c \
	generated/oscats/OscatsModelDina.c \
	generated/oscats/OscatsModelNida.c \
	generated/oscats/OscatsAdministrand.c \
	generated/oscats/OscatsItem.c \
	generated/oscats/OscatsItemBank.c \
	generated/oscats/OscatsTest.c \
	generated/oscats/OscatsRandom.c
