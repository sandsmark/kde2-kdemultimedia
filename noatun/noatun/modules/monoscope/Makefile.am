INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunmonoscope.la

libnoatunmonoscope_la_SOURCES = monoscope.cpp 
#prefs.cpp

libnoatunmonoscope_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunmonoscope_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la -lm

libnoatunmonoscope_la_METASOURCES = AUTO

noinst_HEADERS = monoscope.h
#prefs.h

data_DATA = monoscope.plugin
datadir = $(kde_datadir)/noatun
