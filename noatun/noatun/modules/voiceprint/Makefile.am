INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunvoiceprint.la

libnoatunvoiceprint_la_SOURCES = voiceprint.cpp prefs.cpp

libnoatunvoiceprint_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunvoiceprint_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la -lm

libnoatunvoiceprint_la_METASOURCES = AUTO

noinst_HEADERS = voiceprint.h prefs.h

data_DATA = voiceprint.plugin
datadir = $(kde_datadir)/noatun
