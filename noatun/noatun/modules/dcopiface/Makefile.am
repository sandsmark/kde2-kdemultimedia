INCLUDES= -I$(top_builddir)/noatun/noatun/library/noatunarts -I$(top_srcdir)/noatun/noatun/library $(all_includes) \
	 -I$(kde_includes)/arts 
lib_LTLIBRARIES = libnoatundcopiface.la

libnoatundcopiface_la_SOURCES = dcopiface.cpp dcopiface.skel

libnoatundcopiface_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libnoatundcopiface_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/noatun/library/libnoatun.la

libnoatundcopiface_la_METASOURCES = AUTO

noinst_HEADERS = dcopiface.h

data_DATA = dcopiface.plugin
datadir = $(kde_datadir)/noatun
