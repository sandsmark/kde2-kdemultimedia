INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunnet.la

libnoatunnet_la_SOURCES = net.cpp

libnoatunnet_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunnet_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la

libnoatunnet_la_METASOURCES = AUTO

noinst_HEADERS = net.h

data_DATA = net.plugin
datadir = $(kde_datadir)/noatun
