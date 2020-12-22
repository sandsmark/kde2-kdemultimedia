####### Various modules for artsmodules

SUBDIRS = mcopclass
INCLUDES= -I$(top_builddir)/arts/runtime -I$(srcdir)/freeverb -I$(arts_includes) $(all_includes)

lib_LTLIBRARIES = libartsmidi_idl.la libartsmidi.la

bin_PROGRAMS = midisend

midisend_SOURCES = midisend.cc midimsg.c
midisend_LDFLAGS = $(all_libraries) $(KDE_RPATH)
midisend_LDADD   = libartsmidi_idl.la

libartsmidi_idl_la_SOURCES = artsmidi.cc
libartsmidi_idl_la_LIBADD = -lmcop
libartsmidi_idl_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) \
  -no-undefined -module

libartsmidi_la_SOURCES = midimanager_impl.cc midiclient_impl.cc \
	miditest_impl.cc midimanagerport_impl.cc rawmidiport_impl.cc \
	systemmiditimer_impl.cc audiomiditimer_impl.cc miditimercommon.cc
libartsmidi_la_LIBADD = libartsmidi_idl.la -lartsflow
libartsmidi_la_LDFLAGS = $(all_libraries) -no-undefined -module

artsmidi.cc: artsmidi.h

artsmidi.h: $(srcdir)/artsmidi.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(arts_includes)/arts $(srcdir)/artsmidi.idl

####### install idl files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmidi.h artsmidi.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmidi.mcoptype artsmidi.mcopclass
