INCLUDES= -I$(top_builddir)/arts/gui/common -I$(top_srcdir)/arts/gui/common -I$(kde_includes)/arts $(all_includes)
lib_LTLIBRARIES = libartseffects.la
#libartseffectsui.la

libartseffects_la_SOURCES = artseffects.cc extrastereo_impl.cc
libartseffects_la_LDFLAGS = -L$(top_builddir)/arts/gui/common $(all_libraries) \
                            -module -avoid-version -no-undefined
libartseffects_la_LIBADD =  -lartsgui_idl -lkmedia2_idl -lsoundserver_idl -lartsflow

#libartseffectsui_la_SOURCES = artseffects.cc
#libartseffectsui_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libartseffectsui_la_LIBADD = -lqtmcop -lkmedia2_idl -lsoundserver_idl

noinst_HEADERS =

artseffects.cc: artseffects.h
artseffects.h: $(srcdir)/artseffects.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(top_builddir)/arts/gui/common -I$(top_srcdir)/arts/gui/common -I$(kde_includes)/arts $(srcdir)/artseffects.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artseffects.mcoptype artseffects.mcopclass

mcopclassdir = $(libdir)/mcop
mcopclass_DATA = ExtraStereo.mcopclass VoiceRemoval.mcopclass RawWriter.mcopclass ExtraStereoGuiFactory.mcopclass

mcoptypeddir = $(libdir)
mcoptyped_DATA = artseffects.mcoptype artseffects.mcopclass


