lib_LTLIBRARIES = libartsgui_idl.la libartsgui.la

INCLUDES= -I$(arts_includes) -I$(top_builddir)/arts/runtime $(all_includes)

libartsgui_idl_la_SOURCES = artsgui.cc
libartsgui_idl_la_LIBADD = -lmcop $(LIBDL)
libartsgui_idl_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) \
  -no-undefined -module

libartsgui_la_SOURCES = genericguifactory_impl.cc
libartsgui_la_LIBADD = libartsgui_idl.la
libartsgui_la_LDFLAGS = $(all_libraries) -no-undefined -module

artsgui.h: $(srcdir)/artsgui.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(includedir)/arts $(srcdir)/artsgui.idl

artsgui.cc: artsgui.h
artsgui.mcoptype: artsgui.h
artsgui.mcopclass: artsgui.h

####### install idl files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsgui.h artsgui.idl

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = GenericGuiFactory.mcopclass

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsgui.mcoptype artsgui.mcopclass
