# $Id: Makefile.am 97772 2001-05-19 01:39:13Z waba $

INCLUDES= -I$(kde_includes)/arts $(all_includes)

noinst_HEADERS = oggPlayObject_impl.h

lib_LTLIBRARIES = liboggarts.la
liboggarts_la_SOURCES = oggarts.cc oggPlayObject_impl.cpp
liboggarts_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liboggarts_la_LIBADD =  -lkmedia2_idl -lsoundserver_idl -lartsflow -lvorbisfile -lvorbis -logg
liboggarts_la_METASOURCES = AUTO

oggarts.cc: oggarts.h
oggarts.mcopclass: oggarts.h
oggarts.mcoptype: oggarts.h
oggarts.h: $(srcdir)/oggarts.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(kde_includes)/arts $(srcdir)/oggarts.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = oggarts.mcoptype oggarts.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = oggPlayObject.mcopclass
