INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunui.la

libnoatunui_la_SOURCES = noatunui.cpp userinterface.cpp

libnoatunui_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunui_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                        $(top_builddir)/noatun/noatun/library/libnoatuncontrols.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatunui_la_METASOURCES = AUTO

noinst_HEADERS = userinterface.h

data_DATA = noatunui.plugin
datadir = $(kde_datadir)/noatun
