
INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunhtmlexport.la

libnoatunhtmlexport_la_SOURCES = htmlexport.cpp
noinst_HEADERS = htmlexport.h

libnoatunhtmlexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunhtmlexport_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatunhtmlexport_la_METASOURCES = AUTO


data_DATA = htmlexport.plugin
datadir = $(kde_datadir)/noatun
