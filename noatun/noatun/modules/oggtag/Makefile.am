INCLUDES= -I$(top_srcdir)/noatun/noatun/library -I$(kde_includes)/arts $(all_includes)
lib_LTLIBRARIES = libnoatunoggtag.la

libnoatunoggtag_la_SOURCES = oggtag.cpp

libnoatunoggtag_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunoggtag_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la

libnoatunoggtag_la_METASOURCES = AUTO

noinst_HEADERS = oggtag.h

data_DATA = oggtag.plugin
datadir = $(kde_datadir)/noatun
