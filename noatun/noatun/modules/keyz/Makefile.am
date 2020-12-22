INCLUDES= -I$(top_srcdir)/noatun/noatun/library  -I$(kde_includes)/arts $(all_includes)
lib_LTLIBRARIES = libnoatunkeyz.la

libnoatunkeyz_la_SOURCES = keyz.cpp

libnoatunkeyz_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunkeyz_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la

libnoatunkeyz_la_METASOURCES = AUTO

noinst_HEADERS = keyz.h

data_DATA = keyz.plugin
datadir = $(kde_datadir)/noatun
