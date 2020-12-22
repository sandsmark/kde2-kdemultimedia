INCLUDES= -I$(top_srcdir)/noatun/noatun/library $(all_includes)
lib_LTLIBRARIES = libnoatunexcellent.la

libnoatunexcellent_la_SOURCES = noatunui.cpp\
	userinterface.cpp 

libnoatunexcellent_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatunexcellent_la_LIBADD = $(LIB_KFILE) \
	$(top_builddir)/noatun/noatun/library/libnoatun.la \
	$(top_builddir)/noatun/noatun/library/libnoatuncontrols.la \
	-lqtmcop -lkmedia2_idl -lsoundserver_idl

libnoatunexcellent_la_METASOURCES = AUTO

noinst_HEADERS = userinterface.h

data_DATA = excellent.plugin excellentui.rc
datadir = $(kde_datadir)/noatun
