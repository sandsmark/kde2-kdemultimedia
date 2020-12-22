SUBDIRS = skins
INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunkaiman.la

libnoatunkaiman_la_SOURCES = \
	noatunui.cpp \
	userinterface.cpp \
	style.cpp \
	pref.cpp

libnoatunkaiman_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunkaiman_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatunkaiman_la_METASOURCES = AUTO

noinst_HEADERS = \
	userinterface.h \
	style.h \
	pref.h

data_DATA = kaiman.plugin kaimanui.rc
datadir = $(kde_datadir)/noatun
