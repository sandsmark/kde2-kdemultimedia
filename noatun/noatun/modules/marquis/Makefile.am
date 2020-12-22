INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunmarquis.la

libnoatunmarquis_la_SOURCES = marquis.cpp plugin.cpp

libnoatunmarquis_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunmarquis_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatunmarquis_la_METASOURCES = AUTO

noinst_HEADERS = marquis.h

data_DATA = marquis.plugin
datadir = $(kde_datadir)/noatun
