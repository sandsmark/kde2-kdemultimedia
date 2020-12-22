INCLUDES= -I$(top_srcdir)/noatun/noatun/library -I$(kde_includes)/arts $(all_includes)
lib_LTLIBRARIES = libnoatunid3tag.la

libnoatunid3tag_la_SOURCES = id3tag.cpp editor.cpp

libnoatunid3tag_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunid3tag_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la

libnoatunid3tag_la_METASOURCES = AUTO

noinst_HEADERS = id3tag.h editor.h

data_DATA = id3tag.plugin
datadir = $(kde_datadir)/noatun
